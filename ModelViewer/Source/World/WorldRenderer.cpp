#include "WorldRenderer.h"
#include "EntityList.h"
#include "Entity.h"
#include "WaterQuad.h"


WorldRenderer::WorldRenderer(World* world, GraphicsEngine* graphics) :
	zSettings(this, "WorldRenderer.cfg"),
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

	this->zGrassDensity = zSettings.GetSetting("GrassDensity");}

WorldRenderer::~WorldRenderer()
{
	// Stop Observing
	if ( zWorld ) 
	{
		zWorld->RemoveObserver(this);
		zWorld = 0;
	}

	// Clean grass
	for(auto i = this->zGrass.begin(); i != this->zGrass.end(); ++i)
	{
		if(i->second)
		{
			this->zGraphics->DeleteBillboardCollection(i->second);
		}
	}
	this->zGrass.clear();

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
		if(i->second)
		{
			zGraphics->DeleteMesh(i->second);
		}
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
	else if ( WorldSunChanged* WSC = dynamic_cast<WorldSunChanged*>(e) )
	{
		zGraphics->SetSunLightProperties(
			WSC->world->GetSunDir(),
			WSC->world->GetSunColor(),
			WSC->world->GetSunIntensity() );
	}
	else if ( SectorUnloadedEvent* SUE = dynamic_cast<SectorUnloadedEvent*>(e) )
	{
		UPDATEENUM& u = zUpdatesRequired[Vector2UINT(SUE->sectorX, SUE->sectorY)];
		u = (UPDATEENUM)(u | UPDATE_DELETE);
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
	}
	else if ( EntityDeletedEvent* EDE = dynamic_cast<EntityDeletedEvent*>(e) )
	{
		DeleteEntity(EDE->entity);
	}
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

void WorldRenderer::SetEntityGraphics( Entity* e )
{
	// Camera Distance
	Vector3 camPos = zGraphics->GetCamera()->GetPosition();
	float distanceToCam = (camPos - e->GetPosition()).GetLength();

	// Entity in map
	auto i = zEntities.find(e);

	// Too Far Away
	if ( distanceToCam >= zGraphics->GetEngineParameters().FarClip )
	{
		if ( i != zEntities.cend() )
		{
			// Remove observer
			i->first->RemoveObserver(this);

			// Delete graphics
			if ( i->second ) 
			{
				zGraphics->DeleteMesh(i->second);
			}

			// Erase from set
			zEntities.erase(i);
		}

		return;
	}

	// New graphics file name
	const std::string& model = GetEntModel(e->GetType(), distanceToCam);

	// Empty model
	if ( model.empty() )
	{
		// Is it mapped?
		if ( i != zEntities.cend() )
		{
			e->RemoveObserver(this);

			if ( i->second ) 
			{
				zGraphics->DeleteMesh(i->second);
			}
			
			zEntities.erase(i);
		}

		return;
	}

	// Compare Current Name
	if ( i != zEntities.cend() && i->second )
	{
		// Get previous filename
		std::string file = i->second->GetFilePath();
		if ( !strcmp(model.c_str(), i->second->GetFilePath()) )
		{
			// Save Name, Return
			return;
		}
	}

	// Insert Entity
	if ( i == zEntities.cend() && !model.empty() )
	{
		e->AddObserver(this);
		zEntities[e] = 0;
		i = zEntities.find(e);
	}

	// New Mesh
	iMesh* newMesh = 0;

	// Create new
	float billboardDistance = GetEntBillboardDistance(e->GetType());	

	// Check billboard
	if ( billboardDistance > 0.0f )
	{
		newMesh = zGraphics->CreateMesh(
			model.c_str(), 
			e->GetPosition(), 
			GetEntBillboard(e->GetType()).c_str(),
			billboardDistance);
	}
	else
	{
		newMesh = zGraphics->CreateMesh(model.c_str(), e->GetPosition());
	}

	// Randomize animation frame
	if ( iAnimatedMesh* iAnimMesh = dynamic_cast<iAnimatedMesh*>(newMesh) )
	{
		float randTime = (float)rand() / (float)RAND_MAX;
		iAnimMesh->SetAnimationTime( randTime * (float)iAnimMesh->GetAnimationLength() * 0.001f );
	}

	// Delete Current
	if ( i->second ) 
	{
		zGraphics->DeleteMesh(i->second);
	}

	// Set Mesh
	i->second = newMesh;

	// Update entity transformation
	if ( newMesh ) 
	{
		SetEntityTransformation(e);
	}
}

void WorldRenderer::SetEntityTransformation( Entity* e )
{
	auto i = zEntities.find(e);
	if ( i != zEntities.cend() && i->second )
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
				//Delete previous grass if existing.
				auto grassData = this->zGrass.find(ptrTerrain);
				if(grassData != this->zGrass.end())
				{
					this->zGraphics->DeleteBillboardCollection(grassData->second);
					this->zGrass.erase(grassData);
				}

				// Remove AI Grid
				auto grid = zAIGrids.find(ptrTerrain);
				if ( grid != zAIGrids.end() )
					zAIGrids.erase(grid);

				// Destroy Terrain
				zGraphics->DeleteTerrain(ptrTerrain);

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

					this->GenerateGrass(ptrTerrain);
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

					this->GenerateGrass(ptrTerrain);
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

void WorldRenderer::GenerateGrass(iTerrain* ptrTerrain)
{
	//Delete previous grass if existing.
	auto grassData = this->zGrass.find(ptrTerrain);
	if(grassData != this->zGrass.end())
	{
		//Remove from graphics engine.
		this->zGraphics->DeleteBillboardCollection(grassData->second);
		//Remove from map.
		this->zGrass.erase(grassData);
	}

	float width = FSECTOR_WORLD_SIZE;
	float depth = FSECTOR_WORLD_SIZE; 
	unsigned int sqrtGrassDensity = (unsigned int)sqrt((long)this->zGrassDensity);
	float xDiff = width / sqrtGrassDensity;
	float zDiff = depth / sqrtGrassDensity;
	Vector2 grassPos = Vector2(0.0f);
	Vector2 terrainPosXZ = Vector2(ptrTerrain->GetPosition().x - width * 0.5f, ptrTerrain->GetPosition().z - depth * 0.5f);
	float blendValueGrassLight = 0.0f;
	float blendValueGrassMedium = 0.0f;
	float blendValueGrassDark = 0.0f;
	//1 / nrOfGrassTextures - epsilon for the special case when blend value of all 3 grass textures is equal(1/3).
	float blendThreshHold = 0.32f;
	const static float RGB10 = 10.0f / 255.0f;
	const static float RGB20 = 20.0f / 255.0f;
	const static float RGB25 = 25.0f / 255.0f;
	const static float RGB30 = 30.0f / 255.0f;
	const static float RGB40 = 40.0f / 255.0f;
	const static float RGB50 = 50.0f / 255.0f;
	const static float RGB75 = 75.0f / 255.0f;
	const static float RGB100 = 100.0f / 255.0f;
	const static float RGB125 = 125.0f / 255.0f;
	float minGrassWidth = 0.5f; //global variabel?
	float maxGrassWidth = 1.0f; //global variabel?
	float minGrassHeight = 0.25f; //global variabel?
	float maxGrassHeight = 0.5f; //global variabel?
	Vector3* positions = new Vector3[this->zGrassDensity];
	Vector2* sizes = new Vector2[this->zGrassDensity];
	Vector3* colors = new Vector3[this->zGrassDensity];
	srand(ptrTerrain->GetPosition().x + ptrTerrain->GetPosition().z); //Use same, unique seed for terrain every time.
	float rndMaxInv = 1.0f / RAND_MAX;
	float grassWidth = 0.0f;
	float grassHeight = 0.0f;
	float rndGrassColorOffset = 0.0f;
	//Vector3 rndGrassColorOffsetVec = Vector3(0.0f, 0.0f, 0.0f);
	float terrainY = 0.0f;
	Vector2 offsetVector = Vector2(xDiff, zDiff) * 0.5f;
	/*float diff = 0.0f; 
	float diffPow = 0.0f;
	float maxDiff = 0.0f;
	float maxDiffPow = 0.0f;
	float tmp = 0.0f;
	float scale = 0.0f;
	float scaleSum = 0.0f;*/

	unsigned int index = 0;
	float totBlendValue = 0.0f;
	//Vector3 colorGrassLight = Vector3(0.0f, RGB75, 0.0f);
	//Vector3 colorGrassMedium = Vector3(0.0f, RGB50, 0.0f);
	//Vector3 colorGrassDark = Vector3(0.0f, RGB25, 0.0f);
	Vector3 rndGrassColorOffsetVecGrassLight = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 rndGrassColorOffsetVecGrassMedium = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 rndGrassColorOffsetVecGrassDark = Vector3(0.0f, 0.0f, 0.0f);

	/*Vector3 colorGrassLight = Vector3(RGB75, RGB125, RGB75);
	Vector3 colorGrassMedium = Vector3(RGB50, RGB100, RGB50);
	Vector3 colorGrassDark = Vector3(RGB25, RGB75, RGB25);*/

	for(unsigned int x = 0; x < sqrtGrassDensity; ++x)
	{
		for(unsigned int z = 0; z < sqrtGrassDensity; ++z)
		{
			grassPos = terrainPosXZ + Vector2((float)x * xDiff, (float)z * zDiff) + offsetVector;
			try
			{
				terrainY = zWorld->CalcHeightAtWorldPos(grassPos);
			}
			catch (...)
			{
				continue;
			}

			//Always set variables using random to ensure same pattern.
			//Randomize size between min and max grass width and height.
			grassWidth = fmod(rand() * rndMaxInv, maxGrassWidth - minGrassWidth) + minGrassWidth;
			grassHeight = fmod(rand() * rndMaxInv, maxGrassHeight - minGrassHeight) + minGrassHeight;

			float RGB_MIN_MAX = RGB75;

			//Randomize dark grass RGB = g[-RGB_MIN_MAX, 0]
			rndGrassColorOffset = fmod(rand() * rndMaxInv, RGB_MIN_MAX) - RGB_MIN_MAX; //RGB_MIN_MAX = min
			rndGrassColorOffsetVecGrassDark.y = rndGrassColorOffset;
			//Randomize medium grass RGB = g[-RGB_MIN_MAX / 2, RGB_MIN_MAX / 2]
			rndGrassColorOffset = fmod(rand() * rndMaxInv, RGB_MIN_MAX * 0.5f + RGB_MIN_MAX * 0.5f) - RGB_MIN_MAX * 0.5f;
			rndGrassColorOffsetVecGrassMedium.y = rndGrassColorOffset;
			//Randomize light grass RGB = g[0, RGB_MIN_MAX]
			rndGrassColorOffset = fmod(rand() * rndMaxInv, RGB_MIN_MAX); //RGB_MIN_MAX = max
			rndGrassColorOffsetVecGrassLight.y = rndGrassColorOffset;

			//blendValueGrassLight + blendValueGrassMedium + blendValueGrassDark -> range[0,1]
			blendValueGrassLight = this->zWorld->GetAmountOfTexture(grassPos, "07_v01-MossLight.png");
			blendValueGrassMedium = this->zWorld->GetAmountOfTexture(grassPos, "01_v02-Moss.png");
			blendValueGrassDark = this->zWorld->GetAmountOfTexture(grassPos, "06_v01-MossDark.png");

			totBlendValue = blendValueGrassLight + blendValueGrassMedium + blendValueGrassDark;
			if(totBlendValue > blendThreshHold) //Not equal to to avoid division by zero
			{
				if(totBlendValue < blendThreshHold + 0.1f)
				{
					float derp = 1.0f;
				}
				//totBlendValue range[blendThreshHold, 1], we want [0,1]
				float tmp = totBlendValue - blendThreshHold; //range[0, 1 - blendThreshHold];
				tmp /= 1.0f - blendThreshHold;
				totBlendValue = tmp;
				//Set size
				grassHeight *= totBlendValue; //modify grassheight depending on blend values
				sizes[index] = Vector2(grassWidth, grassHeight);
				//Set position
				positions[index] = Vector3(grassPos.x, terrainY + grassHeight * 0.5f, grassPos.y);
				//Set color
				//Col = Lc * Lb +  Mc * Mb + Dc * Db + Rc
				/*colors[index] =		colorGrassLight * blendValueGrassLight 
								+	colorGrassMedium * blendValueGrassMedium
								+	colorGrassDark * blendValueGrassDark
								+	rndGrassColorOffsetVec;
				*/
				colors[index] =		rndGrassColorOffsetVecGrassLight * blendValueGrassLight
								+	rndGrassColorOffsetVecGrassMedium * blendValueGrassMedium
								+	rndGrassColorOffsetVecGrassDark * blendValueGrassDark
								+	Vector3(0.5f, 0.5f, 0.5f); //Color is a multiplier.
				
				//Increase index(number of grass objects)
				index++;
			}

			

			//if(blendValueGrass >= blendThreshHold)
			//{
			//	nrOfPassedConditions++;
			//	//Change height depending on blend value.
			//	//Convert blend value from range[blendThreshHold, 1] to range[0, 1].
			//	/*diff = blendValueGrass - blendThreshHold; 
			//	diffPow = diff * diff;
			//	maxDiff = 1.0f - blendThreshHold;
			//	maxDiffPow = maxDiff * maxDiff;
			//	tmp = pow(maxDiff / maxDiffPow, 2.0f);
			//	scale = diffPow * tmp;
			//	scaleSum += scale;
			//	*/
			//	colorSum += Vector3(RGB50, RGB100, RGB50) + rndGrassColorOffsetVec;
			//	totBlendValue += blendValueGrass;
			//}
			//if(blendValueGrassLight >= blendThreshHold)
			//{
			//	nrOfPassedConditions++;
			//	//Change height depending on blend value.
			//	//Convert blend value from range[blendThreshHold, 1] to range[0, 1].
			//	/*diff = blendValueGrassLight - blendThreshHold; 
			//	diffPow = diff * diff;
			//	maxDiff = 1.0f - blendThreshHold;
			//	maxDiffPow = maxDiff * maxDiff;
			//	tmp = pow(maxDiff / maxDiffPow, 2.0f);
			//	scale = diffPow * tmp;
			//	scaleSum += scale;
			//	*/
			//	colorSum += Vector3(RGB75, RGB125, RGB75) + rndGrassColorOffsetVec;
			//	totBlendValue += blendValueGrassLight;
			//}
			//if(blendValueGrassDark >= blendThreshHold)
			//{
			//	nrOfPassedConditions++;
			//	//Change height depending on blend value.
			//	//Convert blend value from range[blendThreshHold, 1] to range[0, 1].
			//	/*diff = blendValueGrassDark - blendThreshHold; 
			//	diffPow = diff * diff;
			//	maxDiff = 1.0f - blendThreshHold;
			//	maxDiffPow = maxDiff * maxDiff;
			//	tmp = pow(maxDiff / maxDiffPow, 2.0f);
			//	scale = diffPow * tmp;
			//	scaleSum += scale;
			//	*/
			//	colorSum += Vector3(RGB25, RGB75, RGB25) + rndGrassColorOffsetVec;
			//	totBlendValue += blendValueGrassDark;
			//}

			//if(nrOfPassedConditions > 0)
			//{
			//	//tillman todo: if x of tex 0 && x of tex 1 => 0 scalesum
			//	//Set size
			//	//scaleSum /= (float)nrOfPassedConditions;
			//	//grassHeight *= scaleSum;
			//	grassHeight *= totBlendValue;
			//	sizes[index] = Vector2(grassWidth, grassHeight);
			//	//Set position
			//	positions[index] = Vector3(grassPos.x, terrainY + grassHeight * 0.5f, grassPos.y);
			//	//Set color
			//	colorSum /= (float)nrOfPassedConditions;
			//	colors[index] = colorSum;
			//	index++;
			//}
		}
	}

	
	//Add grass
	//No offset vector needed since grass positions is in world space.
	if(index > 0)
	{
		this->zGrass[ptrTerrain] = this->zGraphics->CreateBillboardCollection(index, positions, sizes, colors, Vector3(0.0f, 0.0f, 0.0f), "Media/Grass.png"); 
		this->zGrass[ptrTerrain]->SetRenderShadowFlag(false); //Don't render shadows.
	}
	
	//Delete data 
	delete [] positions;
	delete [] sizes;
	delete [] colors;
}