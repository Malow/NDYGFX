#include "WorldRenderer.h"
#include "EntityList.h"
#include "Entity.h"
#include "WaterQuad.h"


WorldRenderer::WorldRenderer(World* world, GraphicsEngine* graphics) : 
	zWorld(world),
	zGraphics(graphics),
	zShowAIMap(false),
	zShowWaterBoxes(false)
{
	// Observer World
	zWorld->AddObserver(this);

	// Sun Properties
	zGraphics->SetSunLightProperties(
		zWorld->GetSunDir(),
		zWorld->GetSunColor(),
		zWorld->GetSunIntensity() );

	// Queue Sector Loading
	auto loadedSectors = zWorld->GetLoadedSectors();
	if ( !loadedSectors.empty() )
	{
		for( auto i = loadedSectors.cbegin(); i != loadedSectors.cend(); ++i )
		{
			UPDATEENUM& u = zUpdatesRequired[*i];
			u = (UPDATEENUM)(u | UPDATE_ALL);
		}
	}

	// Render Waters
	for( auto i = zWorld->GetWaterQuads().cbegin(); i != zWorld->GetWaterQuads().cend(); ++i )
	{
		(*i)->AddObserver(this);

		zWaterQuads[*i] = zGraphics->CreateWaterPlane(Vector3(0.0f, 0.0f, 0.0f), "Media/WaterTexture.png");
		
		auto i2 = zWaterQuads.find(*i);
		if ( i2 != zWaterQuads.end() )
		{
			for( unsigned int x=0; x<4; ++x )
			{
				i2->second->SetVertexPosition((*i)->GetPosition(x), x);
			}
		}

		UpdateWaterBoxes(*i);
	}
}

WorldRenderer::~WorldRenderer()
{
	// Stop Observing
	if ( zWorld ) 
	{
		zWorld->RemoveObserver(this);
		zWorld = 0;
	}

	// Clean Terrain
	for( unsigned int x=0; x<zTerrain.size(); ++x )
	{
		if ( zTerrain[x] )
		{
			zGraphics->DeleteTerrain(zTerrain[x]);
		}
	}
	zTerrain.clear();

	// Clean Entities
	for( auto i = zEntities.cbegin(); i != zEntities.cend(); ++i )
	{
		i->first->RemoveObserver(this);
		zGraphics->DeleteMesh(i->second);
	}
	zEntities.clear();

	// Clean Water Boxes
	for( auto i = zWaterBoxes.cbegin(); i != zWaterBoxes.cend(); ++i )
	{
		for( unsigned int x=0; x<4; ++x )
		{
			zGraphics->DeleteMesh( i->second.zCubes[x] );
		}
	}
	zWaterBoxes.clear();

	// Clean Water Quads
	for( auto i = zWaterQuads.cbegin(); i != zWaterQuads.cend(); ++i )
	{
		i->first->RemoveObserver(this);
		zGraphics->DeleteMesh(i->second);
	}
	zWaterQuads.clear();
}

void WorldRenderer::OnEvent( Event* e )
{
	if ( WorldDeletedEvent* WDE = dynamic_cast<WorldDeletedEvent*>(e) )
	{
		zWorld->RemoveObserver(this);
		zWorld = 0;
	}
	else if ( WorldLoadedEvent* WLE = dynamic_cast<WorldLoadedEvent*>(e) )
	{
		zGraphics->SetSunLightProperties(
			WLE->world->GetSunDir(),
			WLE->world->GetSunColor(),
			WLE->world->GetSunIntensity() );
	}
	else if ( WaterQuadCreatedEvent* WQCE = dynamic_cast<WaterQuadCreatedEvent*>(e) )
	{
		zWaterQuads[WQCE->zQuad] = zGraphics->CreateWaterPlane(Vector3(0.0f, 0.0f, 0.0f), "Media/WaterTexture.png");
		WQCE->zQuad->AddObserver(this);
		UpdateWaterBoxes(WQCE->zQuad);
	}
	else if ( WaterQuadLoadedEvent* WQLE = dynamic_cast<WaterQuadLoadedEvent*>(e) )
	{
		zWaterQuads[WQLE->zQuad] = zGraphics->CreateWaterPlane(Vector3(0.0f, 0.0f, 0.0f), "Media/WaterTexture.png");
		WQLE->zQuad->AddObserver(this);

		for( unsigned int x=0; x<4; ++x )
		{
			zWaterQuads[WQLE->zQuad]->SetVertexPosition(WQLE->zQuad->GetPosition(x), x);
		}
		
		UpdateWaterBoxes(WQLE->zQuad);
	}
	else if ( WaterQuadEditedEvent* WQEE = dynamic_cast<WaterQuadEditedEvent*>(e) )
	{
		auto i = zWaterQuads.find(WQEE->zQuad);
		if ( i != zWaterQuads.end() )
		{
			for( unsigned int x=0; x<4; ++x )
			{
				i->second->SetVertexPosition(WQEE->zQuad->GetPosition(x), x);
			}
		}
		UpdateWaterBoxes(WQEE->zQuad);
	}
	else if ( WaterQuadDeletedEvent* WQDE = dynamic_cast<WaterQuadDeletedEvent*>(e) )
	{
		// Find Water Plane
		auto i = zWaterQuads.find(WQDE->zQuad);
		zGraphics->DeleteWaterPlane(i->second);
		zWaterQuads.erase(i);

		// Delete Boxes
		auto boxI = zWaterBoxes.find(WQDE->zQuad);
		if ( boxI != zWaterBoxes.end() ) 
		{
			zGraphics->DeleteMesh(boxI->second.zCubes[0]);
			zGraphics->DeleteMesh(boxI->second.zCubes[1]);
			zGraphics->DeleteMesh(boxI->second.zCubes[2]);
			zGraphics->DeleteMesh(boxI->second.zCubes[3]);
			zWaterBoxes.erase(boxI);
		}
	}
	else if ( SectorUnloadedEvent* SUE = dynamic_cast<SectorUnloadedEvent*>(e) )
	{
		unsigned int tIndex = SUE->sectorY * SUE->world->GetNumSectorsWidth() + SUE->sectorX;
		zGraphics->DeleteTerrain(zTerrain[tIndex]);

		// Remove AI Grid
		auto grid = zAIGrids.find(zTerrain[tIndex]);
		if ( grid != zAIGrids.end() )
			zAIGrids.erase(grid);

		zTerrain[tIndex] = 0;
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
		UPDATEENUM& u = zUpdatesRequired[Vector2UINT(SLE->x, SLE->y)];
		u = (UPDATEENUM)(u | UPDATE_ALL);
	}
	else if ( SectorHeightMapChanged* SHMC = dynamic_cast<SectorHeightMapChanged*>(e) )
	{
		UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SHMC->sectorx, SHMC->sectory) ];
		u = (UPDATEENUM)(u | UPDATE_HEIGHTMAP);
	}
	else if ( SectorNormalChanged* SNC = dynamic_cast<SectorNormalChanged*>(e) )
	{
		UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SNC->sectorx, SNC->sectory) ];
		u = (UPDATEENUM)(u | UPDATE_HEIGHTMAP);
	}
	else if ( SectorBlendMapChanged* SHMC = dynamic_cast<SectorBlendMapChanged*>(e) )
	{
		UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SHMC->sectorx, SHMC->sectory) ];
		u = (UPDATEENUM)(u | UPDATE_BLENDMAP);
	}
	else if ( SectorBlendTexturesChanged* SBTC = dynamic_cast<SectorBlendTexturesChanged*>(e) )
	{
		UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SBTC->sectorX, SBTC->sectorY) ];
		u = (UPDATEENUM)(u | UPDATE_TEXTURES);
	}
	else if ( SectorAIGridChanged* SBTC = dynamic_cast<SectorAIGridChanged*>(e) )
	{
		UPDATEENUM& u = zUpdatesRequired[ Vector2UINT(SBTC->sectorX, SBTC->sectorY) ];
		u = (UPDATEENUM)(u | UPDATE_AIGRID);
	}
	else if ( EntityChangedTypeEvent* ECTE = dynamic_cast<EntityChangedTypeEvent*>(e) )
	{
		zEntsToUpdate.insert(ECTE->entity);
	}
	else if ( EntityLoadedEvent* ELE = dynamic_cast<EntityLoadedEvent*>(e) )
	{
		zEntsToUpdate.insert(ELE->entity);
	}
	else if ( EntityUpdatedEvent* EUE = dynamic_cast<EntityUpdatedEvent*>(e) )
	{
		SetEntityTransformation(EUE->entity);
	}
	else if ( EntityRemovedEvent* ERE = dynamic_cast<EntityRemovedEvent*>(e) )
	{
		DeleteEntity(ERE->entity);
		ERE->entity->RemoveObserver(this);
	}
}

float WorldRenderer::GetYPosFromHeightMap( float x, float y )
{
	if(zWorld == NULL)
		return std::numeric_limits<float>::infinity();

	unsigned int tIndex = (unsigned int)(y / (float)SECTOR_WORLD_SIZE) * zWorld->GetNumSectorsWidth() + (unsigned int)(x/(float)SECTOR_WORLD_SIZE);
	
	if ( tIndex >= zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight() )
		return std::numeric_limits<float>::infinity();

	if ( zTerrain[tIndex])
	{
		return zTerrain[tIndex]->GetYPositionAt(fmod(x, (float)SECTOR_WORLD_SIZE), fmod(y, (float)SECTOR_WORLD_SIZE));
	}
	
	return std::numeric_limits<float>::infinity();
}

WaterCollisionData WorldRenderer::GetCollisionWithWaterBoxes()
{
	WaterCollisionData result;

	// Default Settings
	result.quad = 0; 

	float curDistance = std::numeric_limits<float>::max();

	iCamera* cam = zGraphics->GetCamera();
	Vector3 camPos = cam->GetPosition();
	Vector3 pickDir = cam->Get3DPickingRay();

	// Test Terrain To Pick Stuff Above Terrain Only
	CollisionData terrainColl = this->Get3DRayCollisionDataWithGround();
	if ( terrainColl.collision )
	{
		curDistance = terrainColl.distance;
	}

	// Water Boxes
	for( auto i = zWaterBoxes.begin(); i != zWaterBoxes.end(); ++i )
	{
		for( unsigned int x=0; x<4; ++x )
		{
			CollisionData cd = zGraphics->GetPhysicsEngine()->GetCollisionRayMesh(
				camPos, 
				pickDir, 
				i->second.zCubes[x]);

			if(cd.collision)
			{
				float thisDistance = (Vector3(cd.posx, cd.posy, cd.posz) - camPos).GetLength();

				if(thisDistance < curDistance)
				{
					result.quad = i->first;
					result.cornerIndex = x;
					result.position = Vector3(cd.posx, cd.posy, cd.posz);
					curDistance = thisDistance;
				}
			}
		}

		// Test Collision With Water Quad
		CollisionData cd = zGraphics->GetPhysicsEngine()->GetCollisionRayMesh(
			camPos, 
			pickDir,
			zWaterQuads[i->first]);

		if(cd.collision)
		{
			float thisDistance = (Vector3(cd.posx, cd.posy, cd.posz) - camPos).GetLength();

			if(thisDistance < curDistance)
			{
				result.quad = i->first;
				result.cornerIndex = 4;
				result.position = Vector3(cd.posx, cd.posy, cd.posz);
				curDistance = thisDistance;
			}
		}
	}

	return result;
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

		if ( zTerrain[sectorIndex] )
		{
			CollisionData cd = zGraphics->GetPhysicsEngine()->GetSpecialCollisionRayTerrain(
				zGraphics->GetCamera()->GetPosition(), 
				zGraphics->GetCamera()->Get3DPickingRay(), 
				zTerrain[sectorIndex],
				(float)SECTOR_WORLD_SIZE / (float)(SECTOR_HEIGHT_SIZE-1));
			
			if ( cd.collision && cd.distance < returnData.distance ) returnData = cd;
		}
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
	zWorld->GetEntitiesInCircle(Vector2(cam->GetPosition().x, cam->GetPosition().z), zGraphics->GetEngineParameters().FarClip, closeEntities);

	float curDistance = std::numeric_limits<float>::max();
	returnPointer = 0;

	for( auto i = closeEntities.cbegin(); i != closeEntities.cend(); ++i )
	{
		if ( zEntities[*i] )
		{
			CollisionData cd = zGraphics->GetPhysicsEngine()->GetCollisionRayMesh(
				cam->GetPosition(), 
				cam->Get3DPickingRay(), 
				zEntities[*i]);

			if(cd.collision)
			{
				float thisDistance = (Vector3(cd.posx,cd.posy,cd.posz) - camPos).GetLength();

				if(thisDistance < curDistance)
				{
					returnPointer = *i;
					curDistance = thisDistance;
				}
			}
		}
	}

	return returnPointer;
}

void WorldRenderer::Update()
{
	// Update Terrain Routine
	UpdateTerrain();

	// Cam Pos
	Vector3 camPos = zGraphics->GetCamera()->GetPosition();

	// Check Distance
	if ( (zLastEntUpdatePos - camPos).GetLength() > 10.0f )
	{
		// Update Existing LOD
		for( auto i = zEntities.cbegin(); i != zEntities.cend(); ++i )
		{
			zEntsToUpdate.insert(i->first);
		}

		// Scan New Entities
		if ( zWorld ) 
		{
			zWorld->GetEntitiesInCircle(camPos.GetXZ(), zGraphics->GetEngineParameters().FarClip, zEntsToUpdate);
		}

		// Update Position
		zLastEntUpdatePos = camPos;
	}
	
	// Update a between 10% and a minimum of 25
	unsigned int x = zEntsToUpdate.size() / 10;
	if ( x < 25 ) x = 25;
	if ( x > 100 ) x = 100;
	auto i = zEntsToUpdate.begin();
	while( i != zEntsToUpdate.end() )
	{
		SetEntityGraphics(*i);
		i = zEntsToUpdate.erase(i);
		if ( !x || !--x ) break;
	}
}

void WorldRenderer::ToggleAIGrid(bool state)
{
	zShowAIMap = state;

	if ( !zTerrain.empty() )
	{
		for( unsigned int i=0; i<zWorld->GetNumSectorsWidth()*zWorld->GetNumSectorsHeight(); ++i )
		{
			if ( zTerrain[i] )
			{
				zTerrain[i]->UseAIMap(state);
			}
		}
	}
}

void WorldRenderer::ToggleWaterBoxes(bool flag)
{
	if ( zShowWaterBoxes != flag )
	{
		zShowWaterBoxes = flag;

		if ( zShowWaterBoxes )
		{
			// Create Water Boxes
			for( auto i = zWaterQuads.begin(); i != zWaterQuads.end(); ++i )
			{
				UpdateWaterBoxes(i->first);
			}
		}
		else
		{
			// Delete Water Boxes
			for( auto i = zWaterBoxes.begin(); i != zWaterBoxes.end(); ++i )
			{
				for( unsigned int x=0; x<4; ++x )
				{
					zGraphics->DeleteMesh(zWaterBoxes[i->first].zCubes[x]);
				}
			}

			zWaterBoxes.clear();
		}
	}
}

void WorldRenderer::CreateEntity( Entity* e )
{
	e->AddObserver(this);
	SetEntityGraphics(e);
}

void WorldRenderer::SetEntityGraphics( Entity* e )
{
	// Camera Distance
	Vector3 camPos = zGraphics->GetCamera()->GetPosition();
	float distanceToCam = (camPos - e->GetPosition()).GetLength();

	// Too Far Away
	if ( distanceToCam >= zGraphics->GetEngineParameters().FarClip )
	{
		auto i = zEntities.find(e);
		if ( i != zEntities.cend() )
		{
			i->first->RemoveObserver(this);
			if ( i->second ) zGraphics->DeleteMesh(i->second);
			zEntities.erase(i);
		}
		return;
	}

	// New Graphics
	const std::string& model = GetEntModel(e->GetType(), distanceToCam);

	// Find Old Graphics
	auto i = zEntities.find(e);

	if ( !model.empty() && i == zEntities.end() )
	{
		e->AddObserver(this);
		zEntities[e] = 0;
		i = zEntities.find(e);
	}

	std::string file;
	if ( i != zEntities.end() && i->second ) file = i->second->GetFilePath();

	if ( i != zEntities.end() && (!i->second || strcmp(model.c_str(), i->second->GetFilePath())) )
	{
		// Delete Old Graphics
		if ( i->second ) zGraphics->DeleteMesh(i->second);

		// Create New
		if ( !model.empty() )
		{
			float billboardDistance = GetEntBillboardDistance(e->GetType());

			if ( billboardDistance > 0.0f )
			{
				i->second = zGraphics->CreateMesh(
					model.c_str(), 
					e->GetPosition(), 
					GetEntBillboard(e->GetType()).c_str(),
					billboardDistance);
			}
			else
			{
				i->second = zGraphics->CreateMesh(model.c_str(), e->GetPosition());
			}

			// Randomize Animation Frame
			if ( iAnimatedMesh* iAnimMesh = dynamic_cast<iAnimatedMesh*>(i->second) )
			{
				float randTime = (float)rand() / (float)RAND_MAX;
				iAnimMesh->SetAnimationTime( randTime * (float)iAnimMesh->GetAnimationLength() * 0.001f );
			}

			SetEntityTransformation(e);
		}
		else
		{
			zEntities.erase(i);
		}
	}
}

void WorldRenderer::SetEntityTransformation( Entity* e )
{
	auto i = zEntities.find(e);
	if ( i != zEntities.end() && i->second != 0 )
	{
		i->second->SetPosition(e->GetPosition());
		i->second->SetQuaternion(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		i->second->RotateAxis(Vector3(1.0f, 0.0f, 0.0f), e->GetRotation().x * (3.1415f / 180.0f));
		i->second->RotateAxis(Vector3(0.0f, 1.0f, 0.0f), e->GetRotation().y * (3.1415f / 180.0f));
		i->second->RotateAxis(Vector3(0.0f, 0.0f, 1.0f), e->GetRotation().z * (3.1415f / 180.0f));
		i->second->SetScale(e->GetScale() * 0.05f);
	}
}

void WorldRenderer::DeleteEntity( Entity* e )
{
	// Remove From Update
	zEntsToUpdate.erase(e);

	// Delete Old Graphics
	auto i = zEntities.find(e);
	if ( i != zEntities.end() )
	{
		e->RemoveObserver(this);
		zGraphics->DeleteMesh(i->second);
		zEntities.erase(i);
	}
}

void WorldRenderer::UpdateWaterBoxes( WaterQuad* quad )
{
	// Create Boxes
	if ( zShowWaterBoxes )
	{
		auto i = zWaterBoxes.find(quad);

		if ( i != zWaterBoxes.end() )
		{
			// Calculate Positions
			for( unsigned int x=0; x<4; ++x )
			{
				Vector3 position;
				position = quad->GetPosition(x);

				// Terrain Height Minimum
				try
				{
					float terrainHeight = zWorld->CalcHeightAtWorldPos(position.GetXZ());
					if ( position.y < terrainHeight ) position.y = terrainHeight;
				}
				catch(...)
				{
				}

				i->second.zCubes[x]->SetPosition(position);
			}
		}
		else
		{
			// Calculate Positions
			Vector3 positions[4];
			for( unsigned int x=0; x<4; ++x )
			{
				positions[x] = quad->GetPosition(x);

				// Terrain Height Minimum
				try
				{
					float terrainHeight = zWorld->CalcHeightAtWorldPos(positions[x].GetXZ());
					if ( positions[x].y < terrainHeight ) positions[x].y = terrainHeight;
				}
				catch(...)
				{
				}
			}
			
			zWaterBoxes[quad].zCubes[0] = zGraphics->CreateMesh("Media/Models/Cube_1.obj", positions[0]);
			zWaterBoxes[quad].zCubes[1] = zGraphics->CreateMesh("Media/Models/Cube_2.obj", positions[1]);
			zWaterBoxes[quad].zCubes[2] = zGraphics->CreateMesh("Media/Models/Cube_3.obj", positions[2]);
			zWaterBoxes[quad].zCubes[3] = zGraphics->CreateMesh("Media/Models/Cube_4.obj", positions[3]);

			for( unsigned int x=0; x<4; ++x )
			{
				zWaterBoxes[quad].zCubes[x]->SetScale(1.0f/20.0f);
			}
		}
	}
}

void WorldRenderer::UpdateTerrain()
{
	// Initialize Vector
	if ( zTerrain.empty() )
	{
		if ( zWorld )
		{
			zTerrain.resize(zWorld->GetNumSectorsWidth() * zWorld->GetNumSectorsHeight());
		}
	}

	// Camera Position
	Vector2 camPos = zGraphics->GetCamera()->GetPosition().GetXZ();

	// Find Closest Terrain
	auto closestIterator = zUpdatesRequired.begin();

	// Search And Compare
	for( auto i = zUpdatesRequired.begin(); i != zUpdatesRequired.end(); ++i )
	{	
		// This Position
		Vector2 newPos;
		newPos.x = (float)i->first.x * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;
		newPos.y = (float)i->first.y * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;

		// Current Position
		Vector2 curPos;
		curPos.x = (float)closestIterator->first.x * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;
		curPos.y = (float)closestIterator->first.y * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;

		// Compare
		if ( (camPos-newPos).GetLength() < (camPos-curPos).GetLength() )
		{
			closestIterator = i;
		}
	}

	// Apply Effects To Closest
	if ( closestIterator != zUpdatesRequired.end() )
	{
		// First Update
		auto i = closestIterator;
		
		// Terrain Index
		unsigned int tIndex = i->first.y * zWorld->GetNumSectorsWidth() + i->first.x;
		iTerrain* ptrTerrain = zTerrain[tIndex];

		// Delete 
		if ( ( i->second & UPDATE_DELETE ) == UPDATE_DELETE )
		{
			if ( ptrTerrain )
			{
				// Destroy Terrain
				zGraphics->DeleteTerrain(ptrTerrain);

				// Remove AI Grid
				auto grid = zAIGrids.find(ptrTerrain);
				if ( grid != zAIGrids.end() )
					zAIGrids.erase(grid);

				// Set Pointers
				zTerrain[tIndex] = 0;
				ptrTerrain = 0;
			}
		}
		else
		{
			// Terrain Object
			if ( !ptrTerrain && ( i->second & UPDATE_CREATE ) == UPDATE_CREATE )
			{
				// Create if it doesn't exist
				Vector3 pos;
				pos.x = i->first.x * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;
				pos.y = 0.0f;
				pos.z = i->first.y * FSECTOR_WORLD_SIZE + FSECTOR_WORLD_SIZE * 0.5f;

				zTerrain[tIndex] = zGraphics->CreateTerrain(pos, Vector3(FSECTOR_WORLD_SIZE, 1.0f, FSECTOR_WORLD_SIZE), SECTOR_HEIGHT_SIZE);
				zTerrain[tIndex]->SetTextureScale(10.0f);

				// Update Pointer
				ptrTerrain = zTerrain[tIndex];
			}

			if ( ptrTerrain )
			{
				// Height Map And Normals
				if ( ( i->second & UPDATE_HEIGHTMAP ) == UPDATE_HEIGHTMAP )
				{
					ptrTerrain->SetHeightMap(zWorld->GetSector(i->first)->GetHeightMap());
					ptrTerrain->SetNormals(zWorld->GetSector(i->first)->GetNormals());
				}

				// Blend Map
				if ( ( i->second & UPDATE_BLENDMAP ) == UPDATE_BLENDMAP )
				{
					float* data[2] = { zWorld->GetSector(i->first)->GetBlendMap(), zWorld->GetSector(i->first)->GetBlendMap2() };
					unsigned int sizes[2] = { SECTOR_BLEND_SIZE, SECTOR_BLEND_SIZE };

					ptrTerrain->SetBlendMaps(
						2,
						&sizes[0],
						&data[0] );
				}

				// Textures
				if ( ( i->second & UPDATE_TEXTURES ) == UPDATE_TEXTURES )
				{
					const char* terrainTextures[8];
					std::string files[8];
					for( unsigned int x=0; x<8; ++x )
					{
						files[x] = "Media/Textures/";
						files[x] += zWorld->GetSector(i->first)->GetTextureName(x);
						terrainTextures[x] = &files[x][0];
					}
					ptrTerrain->SetTextures(terrainTextures);
				}

				// AI Grid
				if ( ( i->second & UPDATE_AIGRID ) == UPDATE_AIGRID )
				{
					// Data Access
					AIGrid& sectorGrid = zWorld->GetSector(i->first)->GetAIGrid();
					std::vector<unsigned char> &graphicsGrid = zAIGrids[ptrTerrain];
					graphicsGrid.resize(SECTOR_AI_GRID_SIZE*SECTOR_AI_GRID_SIZE);

					// Bit To Byte
					for( unsigned int x=0; x<SECTOR_AI_GRID_SIZE*SECTOR_AI_GRID_SIZE; ++x )
					{
						graphicsGrid[x] = sectorGrid[x] * 255;
					}

					ptrTerrain->SetAIGrid(SECTOR_AI_GRID_SIZE, &graphicsGrid[0]);
					ptrTerrain->SetAIGridThickness();
					ptrTerrain->UseAIMap(zShowAIMap);
				}
			}
		}

		zUpdatesRequired.erase(i);
	}
}