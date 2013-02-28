#include "WorldRenderer.h"
#include "EntityList.h"
#include "Entity.h"

#undef min
#undef max

WorldRenderer::WorldRenderer( World* world, GraphicsEngine* graphics ) : 
	zWorld(world),
	zGraphics(graphics),
	zShowAIMap(false)
{
	zWorld->AddObserver(this);

	zGraphics->SetSunLightProperties(
		zWorld->GetSunDir(),
		zWorld->GetSunColor(),
		zWorld->GetSunIntensity() );

	// Render Loaded SEctors
	auto loadedSectors = zWorld->GetLoadedSectors();
	if ( !loadedSectors.empty() )
	{
		zTerrain.resize( zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() );
		for( auto i = loadedSectors.cbegin(); i != loadedSectors.cend(); ++i )
		{
			CreateTerrain(*i);
		}
	}
}


WorldRenderer::~WorldRenderer()
{
	// Stop Observing
	zWorld->RemoveObserver(this);

	// Clean Terrain
	for( unsigned int x=0; x<zTerrain.size(); ++x )
	{
		if ( zTerrain[x] )
			zGraphics->DeleteTerrain(zTerrain[x]);
	}
	zTerrain.clear();

	// Clean Entities
	for( auto i = zEntities.begin(); i != zEntities.end(); ++i )
	{
		i->first->RemoveObserver(this);
		zGraphics->DeleteMesh( i->second );
	}

	zEntities.clear();
}


void WorldRenderer::OnEvent( Event* e )
{
	if ( WorldLoadedEvent* WLE = dynamic_cast<WorldLoadedEvent*>(e) )
	{
		if ( WLE->world == zWorld )
		{
			zGraphics->SetSunLightProperties(
				WLE->world->GetSunDir(),
				WLE->world->GetSunColor(),
				WLE->world->GetSunIntensity() );
		}
	}
	else if ( SectorUnloadedEvent* SUE = dynamic_cast<SectorUnloadedEvent*>(e) )
	{
		if ( SUE->world == zWorld )
		{
			unsigned int tIndex = SUE->sectorY * SUE->world->GetNumSectorsWidth() + SUE->sectorX;
			zGraphics->DeleteTerrain(zTerrain[tIndex]);

			// Remove AI Grid
			auto grid = zAIGrids.find(zTerrain[tIndex]);
			if ( grid != zAIGrids.end() )
				zAIGrids.erase(grid);

			zTerrain[tIndex] = 0;
		}
	}
	else if ( WorldSunChanged* WSC = dynamic_cast<WorldSunChanged*>(e) )
	{
		zGraphics->SetSunLightProperties(
			WSC->world->GetSunDir(),
			WSC->world->GetSunColor(),
			WSC->world->GetSunIntensity() );
	}
	else if ( SectorLoadedEvent* SLE = dynamic_cast<SectorLoadedEvent*>(e) )
	{
		if ( SLE->world == zWorld )
		{
			CreateTerrain(Vector2UINT(SLE->x, SLE->y));
		}
	}
	else if ( SectorHeightMapChanged* SHMC = dynamic_cast<SectorHeightMapChanged*>(e) )
	{
		if ( SHMC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SHMC->sectorx, SHMC->sectory) ];
			u = (UPDATEENUM)(u | UPDATE_HEIGHTMAP);
		}
	}
	else if ( SectorNormalChanged* SNC = dynamic_cast<SectorNormalChanged*>(e) )
	{
		if ( SNC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SNC->sectorx, SNC->sectory) ];
			u = (UPDATEENUM)(u | UPDATE_HEIGHTMAP);
		}
	}
	else if ( EntityChangedTypeEvent* ECTE = dynamic_cast<EntityChangedTypeEvent*>(e) )
	{
		SetEntityGraphics(ECTE->entity);
	}
	else if ( EntityLoadedEvent* ELE = dynamic_cast<EntityLoadedEvent*>(e) )
	{
		CreateEntity(ELE->entity);
	}
	else if ( EntitySelectedEvent* ESE = dynamic_cast<EntitySelectedEvent*>(e) )
	{
		if(ESE->entity->GetSelected())
			zEntities[ESE->entity]->SetSpecialColor(COLOR::RED_COLOR);
		else
			zEntities[ESE->entity]->SetSpecialColor(COLOR::NULL_COLOR);
	}
	else if ( EntityUpdatedEvent* EUE = dynamic_cast<EntityUpdatedEvent*>(e) )
	{
		SetEntityTransformation(EUE->entity);
	}
	else if ( EntityRemovedEvent* ERE = dynamic_cast<EntityRemovedEvent*>(e) )
	{
		DeleteEntity(ERE->entity);
	}
	else if ( SectorBlendMapChanged* SHMC = dynamic_cast<SectorBlendMapChanged*>(e) )
	{
		if ( SHMC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SHMC->sectorx, SHMC->sectory) ];
			u = (UPDATEENUM)(u | UPDATE_BLENDMAP);
		}
	}
	else if ( SectorBlendTexturesChanged* SBTC = dynamic_cast<SectorBlendTexturesChanged*>(e) )
	{
		if ( SBTC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SBTC->sectorX, SBTC->sectorY) ];
			u = (UPDATEENUM)(u | UPDATE_TEXTURES);
		}
	}
	else if ( SectorAIGridChanged* SBTC = dynamic_cast<SectorAIGridChanged*>(e) )
	{
		if ( SBTC->world == zWorld )
		{
			UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SBTC->sectorX, SBTC->sectorY) ];
			u = (UPDATEENUM)(u | UPDATE_AIGRID);
		}
	}
}

float WorldRenderer::GetYPosFromHeightMap( float x, float y )
{
	if(zWorld == NULL)
		return std::numeric_limits<float>::infinity();

	unsigned int tIndex = (unsigned int)(y/(float)SECTOR_WORLD_SIZE) * zWorld->GetNumSectorsWidth() + (unsigned int)(x/(float)SECTOR_WORLD_SIZE);
	
	if ( tIndex >= zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() )
		return std::numeric_limits<float>::infinity();

	if ( zTerrain[tIndex])
	{
		return zTerrain[tIndex]->GetYPositionAt(fmod(x, (float)SECTOR_WORLD_SIZE), fmod(y, (float)SECTOR_WORLD_SIZE));
	}
	
	return std::numeric_limits<float>::infinity();
}

CollisionData WorldRenderer::Get3DRayCollisionDataWithGround()
{
	// Get Applicable Sectors
	std::set< Vector2UINT > sectors;
	zWorld->GetSectorsInCicle( zGraphics->GetCamera()->GetPosition().GetXZ(), zGraphics->GetEngineParameters().FarClip, sectors );

	CollisionData returnData;
	returnData.distance = std::numeric_limits<float>::max();

	// Check For Collision
	for( auto i = sectors.begin(); i != sectors.end(); ++i )
	{
		unsigned int sectorIndex = i->y * zWorld->GetNumSectorsWidth() + i->x;

		CollisionData cd = zGraphics->GetPhysicsEngine()->GetSpecialCollisionRayTerrain(
			zGraphics->GetCamera()->GetPosition(), 
			zGraphics->GetCamera()->Get3DPickingRay(), 
			zTerrain[sectorIndex],
			(float)SECTOR_WORLD_SIZE / (float)(SECTOR_HEIGHT_SIZE-1));
		
		if ( cd.collision && cd.distance < returnData.distance ) returnData = cd;
	}

	// Return default collision
	return returnData;
}

Entity* WorldRenderer::Get3DRayCollisionWithMesh()
{
	Entity* returnPointer = NULL;

	iCamera* cam = zGraphics->GetCamera();
	Vector3 camPos = cam->GetPosition();
	
	std::set<Entity*> closeEntities;
	zWorld->GetEntitiesInCircle(Vector2(cam->GetPosition().x, cam->GetPosition().z), 200.0f, closeEntities);

	float curDistance = std::numeric_limits<float>::max();
	returnPointer = 0;

	for( auto i = closeEntities.begin(); i != closeEntities.end(); ++i )
	{
		CollisionData cd = zGraphics->GetPhysicsEngine()->GetCollisionRayMesh(
			cam->GetPosition(), 
			cam->Get3DPickingRay(), 
			zEntities[*i]);

		if(cd.collision)
		{
			float thisDistance = (Vector3(cd.posx,cd.posy,cd.posz) - camPos).GetLength();

			if( cam->Get3DPickingRay().GetDotProduct(Vector3(cd.posx,cd.posy,cd.posz) - camPos) > 0.0f && thisDistance < curDistance)
			{
				returnPointer = *i;
				curDistance = thisDistance;
			}
		}
	}

	return returnPointer;
}

void WorldRenderer::UpdateSectorHeightMap( const Vector2UINT& sectorCoords )
{
	if ( iTerrain* T = GetTerrain(sectorCoords) )
	{
		// Set Heightmap
		T->SetHeightMap( zWorld->GetSector(sectorCoords)->GetHeightMap() );

		// Set Normals
		T->SetNormals(zWorld->GetSector(sectorCoords)->GetNormals());
	}
}

void WorldRenderer::UpdateSectorBlendMap( const Vector2UINT& sectorCoords )
{
	if ( iTerrain* T = GetTerrain(sectorCoords) )
	{
		T->SetHeightMap( zWorld->GetSector(sectorCoords)->GetHeightMap() );

		float* data[2] = { zWorld->GetSector(sectorCoords)->GetBlendMap(), zWorld->GetSector(sectorCoords)->GetBlendMap2()  };
		unsigned int sizes[2] = { SECTOR_BLEND_SIZE, SECTOR_BLEND_SIZE };

		T->SetBlendMaps( 
			2,
			&sizes[0],
			&data[0] );
	}
}

void WorldRenderer::UpdateSectorTextures( const Vector2UINT& sectorCoords )
{
	if ( iTerrain* T = GetTerrain(sectorCoords) )
	{
		const char* terrainTextures[8];
		std::string files[8];
		for( unsigned int x=0; x<8; ++x )
		{
			files[x] = "Media/Textures/";
			files[x] += zWorld->GetSector(sectorCoords)->GetTextureName(x);
			terrainTextures[x] = &files[x][0];
		}
		T->SetTextures(terrainTextures);
	}
}

void WorldRenderer::Update()
{
	while ( !zUpdatesRequired.empty() )
	{
		auto i = zUpdatesRequired.begin();

		if ( ( i->second & UPDATE_BLENDMAP ) == UPDATE_BLENDMAP )
			UpdateSectorBlendMap(i->first);

		if ( ( i->second & UPDATE_HEIGHTMAP ) )
			UpdateSectorHeightMap(i->first);

		if ( ( i->second & UPDATE_TEXTURES ) == UPDATE_TEXTURES )
			UpdateSectorTextures(i->first);

		if ( ( i->second & UPDATE_AIGRID ) == UPDATE_AIGRID )
			UpdateSectorAIGrid(i->first);

		zUpdatesRequired.erase(i);
	}
}

void WorldRenderer::ToggleAIGrid( bool state )
{
	zShowAIMap = state;

	if ( !zTerrain.empty() )
	{
		for( unsigned int i=0; i<zWorld->GetNumSectorsWidth()*zWorld->GetNumSectorsHeight(); ++i )
		{
			if ( zTerrain[i] )
			{
				// TODO Terrain Show Grid
				zTerrain[i]->UseAIMap(state);
			}
		}
	}
}

void WorldRenderer::UpdateSectorAIGrid( const Vector2UINT& sectorCoords )
{
	if ( iTerrain* T = GetTerrain(sectorCoords) )
	{
		// Data Access
		AIGrid& sectorGrid = zWorld->GetSector(sectorCoords)->GetAIGrid();
		std::vector<unsigned char> &graphicsGrid = zAIGrids[T];
		graphicsGrid.resize(SECTOR_AI_GRID_SIZE*SECTOR_AI_GRID_SIZE);

		// Bit To Byte
		for( unsigned int x=0; x<SECTOR_AI_GRID_SIZE*SECTOR_AI_GRID_SIZE; ++x )
		{
			graphicsGrid[x] = sectorGrid[x] * 255;
		}

		T->SetAIGrid(SECTOR_AI_GRID_SIZE, &graphicsGrid[0]);
		T->SetAIGridThickness();
		T->UseAIMap(zShowAIMap);
	}
}

iTerrain* WorldRenderer::GetTerrain( const Vector2UINT& sectorCoords )
{
	if ( zTerrain.empty() ) return 0;
	unsigned int tIndex = sectorCoords.y * zWorld->GetNumSectorsWidth() + sectorCoords.x;
	return zTerrain[tIndex];
}

iMesh* WorldRenderer::GetEntityMesh( Entity* entity )
{
	return this->zEntities[entity];
}

void WorldRenderer::CreateTerrain( const Vector2UINT& sectorCoords )
{
	if ( zTerrain.empty() )
		zTerrain.resize( zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() );

	// Create if it doesn't exist
	unsigned int tIndex = sectorCoords.y * zWorld->GetNumSectorsWidth() + sectorCoords.x;
	if ( !zTerrain[tIndex] )
	{
		Vector3 pos;
		pos.x = sectorCoords.x * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;
		pos.y = 0.0f;
		pos.z = sectorCoords.y * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;

		zTerrain[tIndex] = zGraphics->CreateTerrain(pos, Vector3(FSECTOR_WORLD_SIZE, 1.0f, FSECTOR_WORLD_SIZE), SECTOR_HEIGHT_SIZE);
		zTerrain[tIndex]->SetTextureScale(10.0f);

		UPDATEENUM& u = zUpdatesRequired[sectorCoords];
		u = (UPDATEENUM)(u | UPDATE_ALL);
	}
}

void WorldRenderer::CreateEntity( Entity* e )
{
	e->AddObserver(this);
	SetEntityGraphics(e);
}

void WorldRenderer::SetEntityGraphics( Entity* e )
{
	// Delete Old Graphics
	auto i = zEntities.find(e);
	if ( i != zEntities.end() )
	{
		zGraphics->DeleteMesh(i->second);
	}

	// New Graphics
	const std::string& model = GetEntModel(e->GetType());
	if ( model == "water" )
	{
		zEntities[e] = zGraphics->CreateWaterPlane(e->GetPosition(), "Media/WaterTexture.png");
	}
	else
	{
		float billboardDistance = GetEntBillboardDistance(e->GetType());
		if ( billboardDistance > 0.0f )
		{
			zEntities[e] = zGraphics->CreateMesh(
				model.c_str(), 
				e->GetPosition(), 
				GetEntBillboard(e->GetType()).c_str(),
				billboardDistance);
		}
		else
		{
			zEntities[e] = zGraphics->CreateMesh(model.c_str(), e->GetPosition());
		}
	}

	SetEntityTransformation(e);
}

void WorldRenderer::SetEntityTransformation( Entity* e )
{
	auto i = zEntities.find(e);
	if ( i != zEntities.end() )
	{
		i->second->SetPosition(e->GetPosition());
		i->second->SetQuaternion(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		i->second->RotateAxis(Vector3(1.0f, 0.0f, 0.0f), e->GetRotation().x * (3.1415f / 180.0f));
		i->second->RotateAxis(Vector3(0.0f, 1.0f, 0.0f), e->GetRotation().y * (3.1415f / 180.0f));
		i->second->RotateAxis(Vector3(0.0f, 0.0f, 1.0f), e->GetRotation().z * (3.1415f / 180.0f));
		i->second->SetScale(e->GetScale()*0.05f);
	}
}

void WorldRenderer::DeleteEntity( Entity* e )
{
	// Delete Old Graphics
	auto i = zEntities.find(e);
	if ( i != zEntities.end() )
	{
		e->RemoveObserver(this);
		zGraphics->DeleteMesh(i->second);
		zEntities.erase(i);
	}
}