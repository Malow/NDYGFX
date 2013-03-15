#include "World.h"
#include "CircleAndRect.h"
#include "Entity.h"
#include "WaterQuad.h"
#include "WorldPhysics.h"
#include <sstream>
#include <math.h>
#include <windows.h>


World::World( Observer* observer, const std::string& fileName, bool readOnly) throw(...) : 
	Observed(observer),
	zSectors(0), 
	zNrOfSectorsWidth(0), 
	zNrOfSectorsHeight(0),
	zReadOnly(readOnly),
	zWaterQuadsEdited(0)
{
	if ( zReadOnly )
	{
		zFile = new WorldFile(this, fileName, OPEN_LOAD);
	}
	else
	{
		zFile = new WorldFile(this, fileName, OPEN_EDIT);
	}

	zFile->ReadHeader();
}

World::World( Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight ) :
	Observed(observer),
	zNrOfSectorsWidth(nrOfSectorWidth),
	zNrOfSectorsHeight(nrOfSectorHeight),
	zFile(0),
	zReadOnly(false)
{
	this->zSectors = new Sector**[this->zNrOfSectorsWidth];
	for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
	{
		this->zSectors[i] = new Sector*[this->zNrOfSectorsHeight];
		for( unsigned int o=0; o < this->zNrOfSectorsHeight; ++o )
		{
			this->zSectors[i][o] = NULL;
		}
	}

	zStartCamPos.x = zNrOfSectorsWidth * SECTOR_WORLD_SIZE * 0.5f;
	zStartCamPos.z = zNrOfSectorsHeight * SECTOR_WORLD_SIZE * 0.5f;
	zStartCamPos.y = 1.7f;
	
	zStartCamRot.x = 1.0f;
	zStartCamRot.y = 0.0f;
	zStartCamRot.z = 0.0f;

	zAmbient.x = 0.1f;
	zAmbient.y = 0.1f;
	zAmbient.z = 0.1f;

	zSunDir = Vector3(0.5f, -1.0f, 0.5f);
	zSunColor = Vector3(1.0f, 1.0f, 1.0f);
	zSunIntensity = 1.0f;

	NotifyObservers( &WorldLoadedEvent(this) );
}

World::~World()
{
	// Notify Observers
	NotifyObservers( &WorldDeletedEvent(this) );

	// Close File
	if ( zFile ) 
	{
		delete zFile;
		zFile = 0;
	}

	// Delete Anchors
	for( auto i = zAnchors.cbegin(); i != zAnchors.cend(); ++i )
	{
		delete *i;
	}
	zAnchors.clear();

	// Delete Entities
	std::set<Entity*> ents;
	if ( zEntTree.FlatScan(ents, 0) )
	{
		for( auto i = ents.cbegin(); i != ents.cend(); ++i )
		{
			RemoveEntity(*i);
		}
	}

	// Delete Water Quads
	for( auto i = zWaterQuads.cbegin(); i != zWaterQuads.cend(); ++i )
	{
		delete *i;
	}
	zWaterQuads.clear();

	// Delete the zSectors pointers.
	if ( this->zSectors )
	{
		for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
		{
			for(unsigned int o = 0; o < this->zNrOfSectorsHeight; o++)
			{
				if ( this->zSectors[i][o] )
				{
					delete this->zSectors[i][o];
					this->zSectors[i][o] = 0;
				}
			}
			delete [] this->zSectors[i];
			this->zSectors[i] = 0;
		}

		// Delete the zSector pointer.
		delete [] this->zSectors;
		this->zSectors = 0;
	}
}

Entity* World::CreateEntity( unsigned int entityType )
{
	Entity* temp = new Entity(entityType);
	zEntTree.Insert(temp);
	NotifyObservers( &EntityLoadedEvent(this, temp) );
	temp->SetEdited(true);
	return temp;
}

void World::ModifyHeightAt( float x, float y, float delta )
{
	SetHeightAt( x, y, GetHeightAt( Vector2(x, y) ) + delta );
}

void World::SetHeightAt( float x, float y, float val )
{
	unsigned int sectorX = (unsigned int)x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)y / SECTOR_WORLD_SIZE;

	float localX = fmod(x, (float)SECTOR_WORLD_SIZE) / SECTOR_WORLD_SIZE;
	float localY = fmod(y, (float)SECTOR_WORLD_SIZE) / SECTOR_WORLD_SIZE;

	// Snap Local Coordinates
	float snapX = floor(localX * (SECTOR_HEIGHT_SIZE-1)) / (SECTOR_HEIGHT_SIZE-1);
	float snapY = floor(localY * (SECTOR_HEIGHT_SIZE-1)) / (SECTOR_HEIGHT_SIZE-1);

	GetSector(sectorX, sectorY)->SetHeightAt(snapX, snapY, val);

	// Notify Sector Change
	NotifyObservers( &SectorHeightMapChanged(this, sectorX, sectorY, localX, localY) );

	// Overlap Left
	if ( sectorX > 0 && snapX == 0.0f )
	{
		float border = (float)(SECTOR_HEIGHT_SIZE-1)/(float)SECTOR_HEIGHT_SIZE;
		GetSector(sectorX-1, sectorY)->SetHeightAt(border, snapY, val);
		NotifyObservers( &SectorHeightMapChanged(this, sectorX-1, sectorY, border, snapY) );
	}

	// Overlap Up
	if ( sectorY > 0 && snapY == 0.0f )
	{
		float border = (float)(SECTOR_HEIGHT_SIZE-1)/(float)SECTOR_HEIGHT_SIZE;
		GetSector(sectorX, sectorY-1)->SetHeightAt(snapX, border, val);
		NotifyObservers(&SectorHeightMapChanged(this, sectorX, sectorY-1, snapX, border));
	}

	// Overlap Left Up Corner
	if ( sectorY > 0 && snapY == 0.0f && sectorX > 0 && snapX == 0.0f )
	{
		float border = (float)(SECTOR_HEIGHT_SIZE-1)/(float)SECTOR_HEIGHT_SIZE;
		GetSector(sectorX-1, sectorY-1)->SetHeightAt(border, border, val);
		NotifyObservers(&SectorHeightMapChanged(this, sectorX-1, sectorY-1, border, border));
	}
}

float World::GetHeightAt( const Vector2& worldPos )
{
	unsigned int sectorX = (unsigned int)worldPos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)worldPos.y / SECTOR_WORLD_SIZE;

	float localX = fmod(worldPos.x, (float)SECTOR_WORLD_SIZE) / SECTOR_WORLD_SIZE;
	float localY = fmod(worldPos.y, (float)SECTOR_WORLD_SIZE) / SECTOR_WORLD_SIZE;

	// Snap Local Coordinates
	float snapX = floor(localX * (SECTOR_HEIGHT_SIZE-1)) / (SECTOR_HEIGHT_SIZE-1);
	float snapY = floor(localY * (SECTOR_HEIGHT_SIZE-1)) / (SECTOR_HEIGHT_SIZE-1);

	return GetSector(sectorX, sectorY)->GetHeightAt(snapX, snapY);
}

void World::SaveFile()
{
	if ( zFile )
	{
		// Settings
		zFile->SetStartCamera(zStartCamPos, zStartCamRot);
		zFile->SetWorldAmbient(zAmbient);
		zFile->SetSunProperties(zSunDir,zSunColor,zSunIntensity);

		// Save Waters
		if ( zWaterQuadsEdited )
		{
			// Compile Water Quads
			std::vector<Vector3> quads;
			quads.resize(zWaterQuads.size()*4);

			unsigned int x = 0;
			for( auto i = zWaterQuads.begin(); i != zWaterQuads.end(); ++i )
			{
				for( auto y = 0; y < 4; ++y )
				{
					quads[x++] = (*i)->GetPosition(y);
				}
			}

			zFile->WriteWater(quads);
			zWaterQuadsEdited = false;
		}

		// Per Sector
		if ( zSectors )
		{
			for(unsigned int x=0; x<GetNumSectorsWidth(); ++x)
			{
				for(unsigned int y=0; y<GetNumSectorsHeight(); ++y)
				{
					if ( this->zSectors[x][y] )
					{
						// Optimization because it was used a lot
						unsigned int sectorIndex = y * zNrOfSectorsWidth + x;

						if ( this->zSectors[x][y]->IsEdited() )
						{
							// Write Sector Data
							zFile->WriteHeightMap(zSectors[x][y]->GetHeightMap(), sectorIndex);
							zFile->WriteBlendMap(zSectors[x][y]->GetBlendMap(), sectorIndex);
							zFile->WriteTextureNames(zSectors[x][y]->GetTextureNames(), sectorIndex);
							zFile->WriteAIGrid(zSectors[x][y]->GetAIGrid(), sectorIndex);
							zFile->WriteBlendMap2(zSectors[x][y]->GetBlendMap2(), sectorIndex);
							zFile->WriteTextureNames2(zSectors[x][y]->GetTextureNames2(), sectorIndex);
							zFile->WriteNormals(zSectors[x][y]->GetNormals(), sectorIndex);

							// Write Sector Header
							WorldFileSectorHeader header;
							header.generated = true;
							for( unsigned int i = 0; i < 3; ++i )
							{
								header.ambientColor[i] = zSectors[x][y]->GetAmbient()[i];
							}
							
							zFile->WriteSectorHeader(header, sectorIndex);
							zSectors[x][y]->SetEdited(false);
						}

						// Save Entities
						Rect sectorRect(Vector2(x * FSECTOR_WORLD_SIZE, y * FSECTOR_WORLD_SIZE), Vector2(FSECTOR_WORLD_SIZE, FSECTOR_WORLD_SIZE));
						std::set< Entity* > entitiesInArea;
						bool save = false;

						// Find Regional Entities
						if ( GetEntitiesInRect(sectorRect, entitiesInArea) != zLoadedEntityCount[zSectors[x][y]] )
						{
							save = true;
						}

						// Go Through List
						std::array<EntityStruct, 256> eArray;
						for( unsigned int i=0; i<256; ++i )
						{
							eArray[i].type = 0;
						}

						unsigned int cur = 0;
						for( auto e = entitiesInArea.begin(); e != entitiesInArea.end(); ++e )
						{
							if( (*e)->IsEdited() )
							{
								(*e)->SetEdited(false);
								save = true;
							}

							// Position
							Vector3 pos = (*e)->GetPosition();
							eArray[cur].pos[0] = pos.x - sectorRect.topLeft.x;
							eArray[cur].pos[1] = pos.y;
							eArray[cur].pos[2] = pos.z - sectorRect.topLeft.y;

							// Rotation
							Vector3 rot = (*e)->GetRotation();
							eArray[cur].rot[0] = rot.x;
							eArray[cur].rot[1] = rot.y;
							eArray[cur].rot[2] = rot.z;

							// Scale
							Vector3 scale = (*e)->GetScale();
							eArray[cur].scale[0] = scale.x;
							eArray[cur].scale[1] = scale.y;
							eArray[cur].scale[2] = scale.z;

							// Type
							eArray[cur].type = (*e)->GetType();

							cur++;
						}

						if ( save )
						{
							zFile->WriteEntities(eArray, y * GetNumSectorsWidth() + x);
							zLoadedEntityCount[zSectors[x][y]] = cur;
						}
					}
				}
			}
		}
	}
}

void World::SaveFileAs( const std::string& fileName )
{
	if ( fileName.empty() )
		throw("Filename is empty!");

	if ( zFile )
	{
		if ( fileName == zFile->GetFileName() )	// Saving to the currently open file
		{
			SaveFile();
			return;
		}
		else
		{
			// Handle Save As
			std::string filePath = zFile->GetFileName();
			delete zFile;

			if ( !CopyFile( filePath.c_str(), fileName.c_str(), false ) )
			{
				throw("Failed Copy!");
			}

			zFile = new WorldFile(this, fileName, OPEN_SAVE);
			SaveFile();
		}
	}
	else
	{
		DeleteFile( fileName.c_str() );
		zFile = new WorldFile(this, fileName, OPEN_SAVE);
		SaveFile();
	}
}

Sector* World::GetSectorAtWorldPos( const Vector2& pos )
{
	Vector2UINT sectorPos = WorldPosToSector(pos);
	return GetSector( sectorPos.x, sectorPos.y );
}

Vector2UINT World::WorldPosToSector( const Vector2& pos ) const
{
	return Vector2UINT((unsigned int)pos.x / SECTOR_WORLD_SIZE, (unsigned int)pos.y / SECTOR_WORLD_SIZE);
}

Sector* World::GetSector( const Vector2UINT& sectorCoords ) throw(...)
{
	return GetSector(sectorCoords.x, sectorCoords.y);
}

Sector* World::GetSector( unsigned int x, unsigned int y ) throw(...)
{
	if( x >= this->GetNumSectorsWidth() || y >= this->GetNumSectorsHeight() )
		throw("Index Out Of Bounds");

	Sector* s = this->zSectors[x][y];

	if ( !s )
	{
		s = new Sector();

		zSectors[x][y] = s;

		if ( zFile )
		{
			// Read Header
			WorldFileSectorHeader header;
			if ( zFile->ReadSectorHeader(header, y * GetNumSectorsWidth() + x) && header.generated )
			{
				// Set Header
				s->SetAmbient(Vector3(header.ambientColor[0], header.ambientColor[1], header.ambientColor[2]));

				// Height Map
				if ( !zFile->ReadHeightMap(s->GetHeightMap(), y * GetNumSectorsWidth() + x) )
				{
					SectorWarningEvent SWE;
					SWE.message = "Failed Loading Height Map";
					SWE.sectorX = x;
					SWE.sectorY = y;
					SWE.world = this;
					NotifyObservers(&SWE);
				}

				// Blend Map
				if ( !zFile->ReadBlendMap(s->GetBlendMap(), y * GetNumSectorsWidth() + x) )
				{
					SectorWarningEvent SWE;
					SWE.message = "Failed Loading Blend Map #1";
					SWE.sectorX = x;
					SWE.sectorY = y;
					SWE.world = this;
					NotifyObservers(&SWE);
				}

				// Texture Set Number 1
				if ( !zFile->ReadTextureNames(s->GetTextureNames(), y * GetNumSectorsWidth() + x) )
				{
					SectorWarningEvent SWE;
					SWE.message = "Failed Loading Texture Set #1";
					SWE.sectorX = x;
					SWE.sectorY = y;
					SWE.world = this;
					NotifyObservers(&SWE);
				}
				
				// AI Grid
				if ( !zFile->ReadAIGrid(s->GetAIGrid(), y * GetNumSectorsWidth() + x) )
				{
					SectorWarningEvent SWE;
					SWE.message = "Failed Loading AI Grid";
					SWE.sectorX = x;
					SWE.sectorY = y;
					SWE.world = this;
					NotifyObservers(&SWE);
				}

				// Second Blend Map
				if ( !zFile->ReadBlendMap2(s->GetBlendMap2(), y * GetNumSectorsWidth() + x) )
				{
					SectorWarningEvent SWE;
					SWE.message = "Failed Loading Blend Map #2";
					SWE.sectorX = x;
					SWE.sectorY = y;
					SWE.world = this;
					NotifyObservers(&SWE);

					// Use Default
					s->ResetBlendMap2();
				}

				// Texture Set
				if ( !zFile->ReadTextureNames2(s->GetTextureNames2(), y * GetNumSectorsWidth() + x) )
				{
					SectorWarningEvent SWE;
					SWE.message = "Failed Loading Texture Set #2";
					SWE.sectorX = x;
					SWE.sectorY = y;
					SWE.world = this;
					NotifyObservers(&SWE);

					// Default Values
					s->SetTextureName(4, s->GetTextureName(0));
					s->SetTextureName(5, s->GetTextureName(1));
					s->SetTextureName(6, s->GetTextureName(2));
					s->SetTextureName(7, s->GetTextureName(3));
				}

				// Normals
				if ( !zFile->ReadNormals(s->GetNormals(), y * GetNumSectorsWidth() + x) )
				{
					SectorWarningEvent SWE;
					SWE.message = "Failed Loading Normals";
					SWE.sectorX = x;
					SWE.sectorY = y;
					SWE.world = this;
					NotifyObservers(&SWE);

					// Use Default
					s->ResetNormals();
				}
			}
			else
			{
				s->Reset();
			}

			// Load Entities
			std::array<EntityStruct, 256> eArray;
			if ( zFile->ReadEntities(y * GetNumSectorsWidth() + x, eArray) )
			{
				unsigned int counter=0;
				for( auto e = eArray.cbegin(); e != eArray.cend() && e->type != 0; ++e )
				{
					Vector3 pos(e->pos[0]+(x*SECTOR_WORLD_SIZE), e->pos[1], e->pos[2]+(y*SECTOR_WORLD_SIZE));
					Vector3 rot(e->rot[0], e->rot[1], e->rot[2]);
					Vector3 scale(e->scale[0], e->scale[1], e->scale[2]);

					Entity* ent = new Entity(e->type, pos, rot, scale);
					ent->SetEdited(false);
					zEntTree.Insert(ent);
					NotifyObservers(&EntityLoadedEvent(this, ent));

					counter++;
				}
				zLoadedEntityCount[zSectors[x][y]] = counter;
			}
			else
			{
				zLoadedEntityCount[zSectors[x][y]] = 0;
			}
		}
		else
		{
			s->Reset();
		}

		zLoadedSectors.insert( Vector2UINT(x, y) );
		NotifyObservers( &SectorLoadedEvent(this,x,y) );
	}

	return s;
}

void World::OnEvent( Event* e )
{
	if ( WorldHeaderLoadedEvent* WHL = dynamic_cast<WorldHeaderLoadedEvent*>(e) )
	{
		zNrOfSectorsWidth = WHL->header.width;
		zNrOfSectorsHeight = WHL->header.height;

		this->zSectors = new Sector**[this->zNrOfSectorsWidth];
		for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
		{
			this->zSectors[i] = new Sector*[this->zNrOfSectorsHeight];
			for( unsigned int o=0; o < this->zNrOfSectorsHeight; ++o )
			{
				this->zSectors[i][o] = NULL;
			}
		}

		zStartCamPos = zFile->GetStartCamPos();
		zStartCamRot = zFile->GetStartCamRot();
		zAmbient = zFile->GetWorldAmbient();
		zSunDir = zFile->GetSunDirection();
		zSunColor = zFile->GetSunColor();
		zSunIntensity = zFile->GetSunIntensity();

		// Load Waters
		std::vector<Vector3> positions;
		if ( zFile->ReadWater(positions) )
		{
			for( unsigned int x=0; x<positions.size()/4; ++x )
			{
				WaterQuad* quad = new WaterQuad();
				
				for( unsigned int y=0; y<4; ++y )
				{
					quad->SetPosition(y, positions[x*4+y]);
				}

				// Add Observer
				quad->AddObserver(this);

				// Insert Quad
				zWaterQuads.insert(quad);

				// Notify Observers
				WaterQuadLoadedEvent WQLE;
				WQLE.zQuad = quad;
				NotifyObservers(&WQLE);
			}
			
			zWaterQuadsEdited = false;
		}

		// World Has Been Loaded
		NotifyObservers( &WorldLoadedEvent(this) );	
	}
	else if ( WaterQuadEditedEvent* WQEE = dynamic_cast<WaterQuadEditedEvent*>(e) )
	{
		zWaterQuadsEdited = true;
	}
	else if ( WorldHeaderCreateEvent* WHCE = dynamic_cast<WorldHeaderCreateEvent*>(e) )
	{
		WHCE->header.width = zNrOfSectorsWidth;
		WHCE->header.height = zNrOfSectorsHeight;
	}
}

void World::LoadAllSectors()
{
	for( unsigned int x=0; x<GetNumSectorsWidth(); ++x )
	{
		for( unsigned int y=0; y<GetNumSectorsHeight(); ++y )
		{
			GetSector( x, y );
		}
	}
}

unsigned int World::GetEntitiesInCircle( const Vector2& center, float radius, std::set<Entity*>& out, unsigned int typeFilter) const
{
	return zEntTree.CircleScan(out, Circle(center, radius), typeFilter);
}

unsigned int World::GetSectorsInCicle( const Vector2& center, float radius, std::set<Vector2UINT>& out ) const
{
	if ( radius == 0.0f )
	{
		out.insert(this->WorldPosToSector(center));
		return 1;
	}

	unsigned int counter=0;
	
	unsigned int xMin = (unsigned int)(((center.x - radius) < 0.0f? 0.0f : center.x - radius) / FSECTOR_WORLD_SIZE);
	unsigned int xMax = (unsigned int)(((center.x + radius) > (float)zNrOfSectorsWidth * FSECTOR_WORLD_SIZE? (float)zNrOfSectorsWidth * FSECTOR_WORLD_SIZE : (center.x + radius)) / FSECTOR_WORLD_SIZE);

	unsigned int yMin = (unsigned int)(((center.y - radius) < 0.0f? 0.0f : center.y - radius) / FSECTOR_WORLD_SIZE);
	unsigned int yMax = (unsigned int)(((center.y + radius) > (float)zNrOfSectorsHeight * FSECTOR_WORLD_SIZE? (float)zNrOfSectorsHeight * FSECTOR_WORLD_SIZE : (center.y + radius)) / FSECTOR_WORLD_SIZE); 

	for( unsigned int x=xMin; x<xMax; ++x )
	{
		for( unsigned int y=yMin; y<yMax; ++y )
		{
			Rect sectorRect(Vector2(x * FSECTOR_WORLD_SIZE, y * FSECTOR_WORLD_SIZE), Vector2(32,32));
			if ( DoesIntersect(sectorRect, Circle(center,radius)) )
			{
				counter++;
				out.insert( Vector2UINT(x,y) );
			}
		}
	}

	return counter;
}

unsigned int World::CountEntitiesInRect(const Rect& rect) const
{
	return zEntTree.CountInRect(rect);
}

unsigned int World::GetHeightNodesInCircle( const Vector2& center, float radius, std::set<Vector2>& out ) const
{
	unsigned int counter=0;

	// Calculate Height Node Density
	float density = FSECTOR_WORLD_SIZE / (FSECTOR_HEIGHT_SIZE-1.0f);

	for( float x = center.x - radius; x < center.x + radius; x+=density )
	{
		// Outside World
		if ( x < 0.0f || x >= GetWorldSize().x )
			continue;

		for( float y = center.y - radius; y < center.y + radius; y+=density )
		{
			// Outside World
			if ( y < 0.0f || y >= GetWorldSize().y )
				continue;

			// Snap it
			Vector2 snap;
			snap.x = floor(x / density) * density;
			snap.y = floor(y / density) * density;


			if ( Circle(Vector2(center.x, center.y), radius).IsInside(Vector2(snap.x, snap.y) ) )
			{
				out.insert(Vector2(snap.x, snap.y));
				counter++;
			}
		}
	}

	return counter;
}

bool World::IsSectorLoaded( const Vector2UINT& sectorCoords ) const
{
	if ( sectorCoords.x >= zNrOfSectorsWidth ) return false;
	if ( sectorCoords.y >= zNrOfSectorsHeight ) return false;
	return zSectors[sectorCoords.x][sectorCoords.y] != 0;
}

unsigned int World::GetNumSectorsWidth() const
{
	if ( !zNrOfSectorsWidth ) zFile->ReadHeader();
	return zNrOfSectorsWidth;
}

unsigned int World::GetNumSectorsHeight() const
{
	if ( !zNrOfSectorsHeight ) zFile->ReadHeader();
	return zNrOfSectorsHeight;
}

void World::RemoveEntity(Entity* entity)
{
	if ( entity )
	{
		NotifyObservers(&EntityRemovedEvent(this, entity));
		zEntTree.Erase(entity);
		delete entity;
	}
}

unsigned int World::GetTextureNodesInCircle( const Vector2& center, float radius, std::set<Vector2>& out ) const
{
	unsigned int counter=0;

	// Calculate AI Node Density
	float density = ( (float)SECTOR_WORLD_SIZE / (float)(SECTOR_BLEND_SIZE-1) );

	for( float x = center.x-radius-density; x <= center.x+radius+density; x+=density )
	{
		// Outside World
		if ( x < 0.0f || x >= GetWorldSize().x )
			continue;

		for( float y = center.y-radius-density; y <= center.y+radius+density; y+=density )
		{
			// Outside World
			if ( y < 0.0f || y >= GetWorldSize().y )
				continue;

			Vector2 snap;
			snap.x = floor(x / density) * density;
			snap.y = floor(y / density) * density;

			if (Circle(center,radius).IsInside(snap))
			{
				out.insert( Vector2(snap.x, snap.y) );
				counter++;
			}
		}
	}

	return counter;
}

BlendValues World::GetBlendingAt( const Vector2& worldPos )
{
	// Sector Coordinates
	unsigned int sectorX = (unsigned int)worldPos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)worldPos.y / SECTOR_WORLD_SIZE;

	// Local Coordinates
	Vector2 localPos;
	localPos.x = fmod(worldPos.x, FSECTOR_WORLD_SIZE)/FSECTOR_WORLD_SIZE;
	localPos.y = fmod(worldPos.y, FSECTOR_WORLD_SIZE)/FSECTOR_WORLD_SIZE;

	// Snap Local Coordinates
	Vector2 snapPos;
	snapPos.x = floor(localPos.x * (SECTOR_BLEND_SIZE-1)) / (SECTOR_BLEND_SIZE-1);
	snapPos.y = floor(localPos.y * (SECTOR_BLEND_SIZE-1)) / (SECTOR_BLEND_SIZE-1);

	return GetSector(sectorX, sectorY)->GetBlendingAt( snapPos );
}

void World::ModifyBlendingAt( const Vector2& worldPos, const BlendValues& val )
{
	SetBlendingAt( worldPos, GetBlendingAt(worldPos) + val );
}

void World::SetBlendingAt( const Vector2& worldPos, const BlendValues& val )
{
	unsigned int sectorX = (unsigned int)worldPos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)worldPos.y / SECTOR_WORLD_SIZE;
	Vector2 localPos;
	localPos.x = fmod(worldPos.x, FSECTOR_WORLD_SIZE)/FSECTOR_WORLD_SIZE;
	localPos.y = fmod(worldPos.y, FSECTOR_WORLD_SIZE)/FSECTOR_WORLD_SIZE;

	// Snap Local Coordinates
	Vector2 snapPos;
	snapPos.x = floor(localPos.x * (FSECTOR_BLEND_SIZE-1.0f)) / (FSECTOR_BLEND_SIZE-1.0f);
	snapPos.y = floor(localPos.y * (FSECTOR_BLEND_SIZE-1.0f)) / (FSECTOR_BLEND_SIZE-1.0f);

	GetSector(sectorX, sectorY)->SetBlendingAt(snapPos, val);

	NotifyObservers(&SectorBlendMapChanged(this, 
		sectorX,
		sectorY,
		localPos.x,
		localPos.y ));
	
	// Overlap Left
	if ( sectorX > 0 && snapPos.x == 0.0f )
	{
		float border = (float)(SECTOR_BLEND_SIZE-1)/(float)FSECTOR_BLEND_SIZE;
		GetSector(sectorX-1, sectorY)->SetBlendingAt(Vector2(border, snapPos.y), val);
		NotifyObservers( &SectorBlendMapChanged(this, sectorX-1, sectorY, border, snapPos.y) );
	}

	// Overlap Up
	if ( sectorY > 0 && snapPos.y == 0.0f )
	{
		float border = (float)(SECTOR_HEIGHT_SIZE-1)/(float)FSECTOR_HEIGHT_SIZE;
		GetSector(sectorX, sectorY-1)->SetBlendingAt(Vector2(snapPos.x, border), val);
		NotifyObservers( &SectorBlendMapChanged(this, sectorX, sectorY-1, snapPos.x, border) );
	}
	
	// Overlap Left Up Corner
	if ( sectorY > 0 && snapPos.y == 0.0f && sectorX > 0 && snapPos.x == 0.0f )
	{
		float border = (float)(SECTOR_HEIGHT_SIZE-1)/(float)SECTOR_HEIGHT_SIZE;
		GetSector(sectorX-1, sectorY-1)->SetBlendingAt(Vector2(border, border), val);
		NotifyObservers( &SectorBlendMapChanged(this, sectorX-1, sectorY-1, border, border) );
	}
}

WorldAnchor* World::CreateAnchor()
{
	WorldAnchor* newAnchor = new WorldAnchor();
	zAnchors.insert(newAnchor);
	return newAnchor;
}

void World::DeleteAnchor( WorldAnchor*& anchor )
{
	zAnchors.erase(anchor);
	if ( anchor ) delete anchor, anchor = 0;
}

void World::Update()
{
	// Anchored sectors
	std::set< Vector2UINT > anchoredSectors;
	for( auto i = zAnchors.begin(); i != zAnchors.end(); ++i )
	{
		GetSectorsInCicle( (*i)->position, (*i)->radius, anchoredSectors );
	}

	// Loaded sectors
	std::set< Vector2UINT > loadedSectors = GetLoadedSectors();

	// Unload sectors
	std::set< Vector2UINT > sectorsToUnload;
	for( auto i = loadedSectors.begin(); i != loadedSectors.end(); ++i )
	{
		// Is Sector Anchored?
		if ( anchoredSectors.find(*i) == anchoredSectors.end() )
		{
			// Has the sector been edited?
			if ( !GetSector( i->x, i->y )->IsEdited() )
			{
				// Does Sector have edited entities?
				Rect sectorRect(Vector2(i->x * FSECTOR_WORLD_SIZE, i->y * FSECTOR_WORLD_SIZE), Vector2(FSECTOR_WORLD_SIZE, FSECTOR_WORLD_SIZE));
				std::set< Entity* > entitiesInArea;
				bool unsavedEntities = false;

				if ( GetEntitiesInRect(sectorRect, entitiesInArea) != zLoadedEntityCount[zSectors[i->x][i->y]] )
				{
					unsavedEntities = true;
				}

				for( auto e = entitiesInArea.cbegin(); !unsavedEntities && e != entitiesInArea.cend(); ++e )
				{
					if ( (*e)->IsEdited() )
					{
						unsavedEntities = true;
					}
				}

				if ( !unsavedEntities )
					sectorsToUnload.insert(*i);
			}
		}
	}

	// Unload Sectors
	for( auto i = sectorsToUnload.begin(); i != sectorsToUnload.end(); ++i )
	{
		// Delete Entities
		Rect sectorRect(Vector2(i->x * FSECTOR_WORLD_SIZE, i->y * FSECTOR_WORLD_SIZE), Vector2(FSECTOR_WORLD_SIZE, FSECTOR_WORLD_SIZE));
		std::set<Entity*> entitiesInArea;
		GetEntitiesInRect(sectorRect, entitiesInArea);
		for( auto e = entitiesInArea.cbegin(); e != entitiesInArea.cend(); ++e )
		{
			RemoveEntity(*e);
		}
		zLoadedEntityCount.erase(zSectors[i->x][i->y]);

		// Delete Map
		NotifyObservers(&SectorUnloadedEvent(this,i->x,i->y));
		zLoadedSectors.erase(Vector2UINT(i->x, i->y));
		delete zSectors[i->x][i->y];
		zSectors[i->x][i->y] = 0;
	}

	// Load new sectors
	for( auto i = anchoredSectors.begin(); i != anchoredSectors.end(); ++i )
	{
		GetSector( i->x, i->y );
	}
}

void World::SetSectorTexture( unsigned int x, unsigned int y, const std::string& texture, unsigned int index )
{
	Sector* s = GetSector(x,y);
	s->SetTextureName(index, texture);
	NotifyObservers( &SectorBlendTexturesChanged(this,x,y) );
}

const char* const World::GetSectorTexture( unsigned int x, unsigned int y, unsigned int index )
{
	Sector *s = GetSector(x,y);
	return s->GetTextureName(index);
}

unsigned int World::GetEntitiesInRect( const Rect& rect, std::set<Entity*>& out, unsigned int typeFilter ) const
{
	return zEntTree.RectangleScan(out, rect, typeFilter);
}

float World::CalcHeightAtWorldPos( const Vector2& worldPos ) throw(...)
{
	// Outside world
	if ( worldPos.x >= GetWorldSize().x ||
		worldPos.y >= GetWorldSize().y ||
		worldPos.x < 0.0f ||
		worldPos.y < 0.0f ) throw("Out Of Bounds!");

	// Height Nodes Density
	float density = (float)SECTOR_WORLD_SIZE / (float)(SECTOR_HEIGHT_SIZE-1);

	// Snap To Lower
	float minX = floor(worldPos.x / density) * density;
	float minY = floor(worldPos.y / density) * density;

	// Snap To Lower
	float maxX = minX + density;
	float maxY = minY + density;

	Vector3 a(minX, GetHeightAt(Vector2(minX, minY)), minY);
	Vector3 b(maxX, GetHeightAt(Vector2(maxX, minY)), minY);
	Vector3 c(minX, GetHeightAt(Vector2(minX, maxY)), maxY);
	Vector3 d(maxX, GetHeightAt(Vector2(maxX, maxY)), maxY);

	float s = (worldPos.x-a.x) / (b.x-a.x);
	float t = (worldPos.y-a.z) / (c.z-a.z);

	float y1 = a.y + s*(b.y-a.y);
	float y2 = c.y + s*(d.y-c.y);

	return y1 + t*(y2-y1);
}

const Vector3& World::GetStartCamPos() const
{
	return zStartCamPos;
}

const Vector3& World::GetStartCamRot() const
{
	return zStartCamRot;
}

void World::SetStartCamera( const Vector3& pos, const Vector3& rot )
{
	zStartCamPos = pos;
	zStartCamRot = rot;
}

Vector3 World::GetAmbientAtWorldPos( const Vector2& worldPos )
{
	if ( worldPos.x < 0 ) return zAmbient;
	if ( worldPos.y < 0 ) return zAmbient;
	if ( worldPos.x >= GetNumSectorsWidth() * SECTOR_WORLD_SIZE ) return zAmbient;
	if ( worldPos.y >= GetNumSectorsHeight() * SECTOR_WORLD_SIZE ) return zAmbient;

	Sector* s = GetSectorAtWorldPos(worldPos);
	return zAmbient + s->GetAmbient();
}

void World::SetSunProperties( const Vector3& dir, const Vector3& color, float intensity )
{
	zSunDir = dir;
	zSunColor = color;
	zSunIntensity = intensity;

	if ( zFile )
		zFile->SetSunProperties(dir, color, intensity);

	NotifyObservers( &WorldSunChanged(this) );
}

void World::SetWorldAmbient( const Vector3& ambient )
{
	zAmbient = ambient;

	if ( zFile )
		zFile->SetWorldAmbient(ambient);
}

bool World::IsBlockingAt( const Vector2& pos )
{
	unsigned int sectorX = (unsigned int)(pos.x / FSECTOR_WORLD_SIZE);
	unsigned int sectorY = (unsigned int)(pos.y / FSECTOR_WORLD_SIZE);
	float localX = fmod(pos.x, FSECTOR_WORLD_SIZE) / FSECTOR_WORLD_SIZE;
	float localY = fmod(pos.y, FSECTOR_WORLD_SIZE) / FSECTOR_WORLD_SIZE;

	return GetSector(sectorX, sectorY)->GetBlocking( Vector2(localX, localY) );
}

void World::SetBlockingAt( const Vector2& pos, const bool& flag )
{
	unsigned int sectorX = (unsigned int)(pos.x / FSECTOR_WORLD_SIZE);
	unsigned int sectorY = (unsigned int)(pos.y / FSECTOR_WORLD_SIZE);
	float localX = fmod(pos.x, FSECTOR_WORLD_SIZE) / FSECTOR_WORLD_SIZE;
	float localY = fmod(pos.y, FSECTOR_WORLD_SIZE) / FSECTOR_WORLD_SIZE;

	GetSector(sectorX, sectorY)->SetBlocking( Vector2(localX, localY), flag );

	NotifyObservers(&SectorAIGridChanged(this, sectorX, sectorY));
}

Vector3 World::CalcNormalAt( const Vector2& worldPos ) throw(...)
{
	// Outside World
	if ( worldPos.x >= GetWorldSize().x ||
		worldPos.y >= GetWorldSize().y ||
		worldPos.x < 0 ||
		worldPos.y < 0 ) throw("Out Of Bounds!");

	// Densityx
	float density = FSECTOR_WORLD_SIZE / (FSECTOR_HEIGHT_SIZE - 1.0f);

	Vector3 vertices[4];

	vertices[0] = Vector3(worldPos.x, CalcHeightAtWorldPos(worldPos), worldPos.y);
	vertices[1] = Vector3(worldPos.x+density, CalcHeightAtWorldPos(worldPos+Vector2(density, 0.0f)), worldPos.y);
	vertices[2] = Vector3(worldPos.x+density, CalcHeightAtWorldPos(worldPos+density), worldPos.y+density);
	vertices[3] = Vector3(worldPos.x, CalcHeightAtWorldPos(worldPos+Vector2(0.0f, density)), worldPos.y+density);
	
	Vector3 normal(0.0f, 0.0f, 0.0f);

	for (int i=0; i<4; i++)
	{
		Vector3 first = vertices[(i+1)%4] - vertices[i];
		Vector3 second = vertices[(i+3)%4] - vertices[i]; 
		normal += second.GetCrossProduct(first);
	}
	normal.Normalize();

	return normal;
}

void World::SetNormalAt( const Vector2& worldPos, const Vector3& val ) throw(...)
{
	unsigned int sectorX = (unsigned int)worldPos.x / SECTOR_WORLD_SIZE;
	unsigned int sectorY = (unsigned int)worldPos.y / SECTOR_WORLD_SIZE;
	Vector2 localPos;
	localPos.x = fmod(worldPos.x, FSECTOR_WORLD_SIZE) / FSECTOR_WORLD_SIZE;
	localPos.y = fmod(worldPos.y, FSECTOR_WORLD_SIZE) / FSECTOR_WORLD_SIZE;

	// Snap Local Coordinates
	Vector2 snapPos;
	snapPos.x = floor(localPos.x * (FSECTOR_NORMALS_SIZE-1.0f)) / (FSECTOR_NORMALS_SIZE-1.0f);
	snapPos.y = floor(localPos.y * (FSECTOR_NORMALS_SIZE-1.0f)) / (FSECTOR_NORMALS_SIZE-1.0f);

	GetSector(sectorX, sectorY)->SetNormalAt(snapPos, val);

	NotifyObservers(&SectorNormalChanged(this, 
		sectorX,
		sectorY,
		localPos.x,
		localPos.y ));
	
	// Overlap Left
	if ( sectorX > 0 && snapPos.x == 0.0f )
	{
		float border = (float)(FSECTOR_NORMALS_SIZE-1)/(float)FSECTOR_NORMALS_SIZE;
		GetSector(sectorX-1, sectorY)->SetNormalAt(Vector2(border, snapPos.y), val);
		NotifyObservers( &SectorNormalChanged(this, sectorX-1, sectorY, border, snapPos.y) );
	}

	// Overlap Up
	if ( sectorY > 0 && snapPos.y == 0.0f )
	{
		float border = (float)(FSECTOR_NORMALS_SIZE-1)/(float)FSECTOR_NORMALS_SIZE;
		GetSector(sectorX, sectorY-1)->SetNormalAt(Vector2(snapPos.x, border), val);
		NotifyObservers( &SectorNormalChanged(this, sectorX, sectorY-1, snapPos.x, border) );
	}
	
	// Overlap Left Up Corner
	if ( sectorY > 0 && snapPos.y == 0.0f && sectorX > 0 && snapPos.x == 0.0f )
	{
		float border = (float)(FSECTOR_NORMALS_SIZE-1)/(float)FSECTOR_NORMALS_SIZE;
		GetSector(sectorX-1, sectorY-1)->SetNormalAt(Vector2(border, border), val);
		NotifyObservers( &SectorNormalChanged(this, sectorX-1, sectorY-1, border, border) );
	}
}

void World::GenerateSectorNormals( const Vector2UINT& sectorCoords )
{
	// Density
	float density = FSECTOR_WORLD_SIZE / (FSECTOR_NORMALS_SIZE-1.0f);

	// Sector Corner
	Vector2 start;
	start.x = (float)sectorCoords.x * FSECTOR_WORLD_SIZE;
	start.y = (float)sectorCoords.y * FSECTOR_WORLD_SIZE;

	Vector2 end;
	end.x = (float)(sectorCoords.x+1) * FSECTOR_WORLD_SIZE;
	end.y = (float)(sectorCoords.y+1) * FSECTOR_WORLD_SIZE;

	Vector2 current;
	for( current.x = start.x; current.x < end.x; current.x += density )
	{
		for( current.y = start.y; current.y < end.y; current.y += density )
		{
			try
			{
				SetNormalAt( current, CalcNormalAt(current) );
			}
			catch(...)
			{
			}
		}
	}
}

unsigned int World::GetAINodesInCircle( const Vector2& center, float radius, std::set<Vector2>& out ) const
{
	unsigned int counter=0;

	// Calculate AI Node Density
	float density = FSECTOR_WORLD_SIZE / FSECTOR_AI_GRID_SIZE;

	for( float x = center.x-radius-density; x <= center.x+radius+density; x+=density )
	{
		// Outside World
		if ( x < 0.0f || x >= GetWorldSize().x )
			continue;

		for( float y = center.y-radius-density; y <= center.y+radius+density; y+=density )
		{
			// Outside World
			if ( y < 0.0f || y >= GetWorldSize().y )
				continue;

			Vector2 snap;
			snap.x = floor(x / density) * density;
			snap.y = floor(y / density) * density;

			Rect r(snap, Vector2(density, density));

			if (DoesIntersect(r, Circle(center, radius)))
			{
				out.insert( Vector2(snap.x, snap.y) );
				counter++;
			}
		}
	}

	return counter;
}

Vector2 World::GetWorldSize() const
{
	return Vector2( GetNumSectorsWidth() * FSECTOR_WORLD_SIZE, GetNumSectorsHeight() * FSECTOR_WORLD_SIZE );
}

bool World::IsInside( const Vector2& worldPos )
{
	// Invalid Floats
	if ( worldPos.x != worldPos.x ) return false;
	if ( worldPos.y != worldPos.y ) return false;

	if ( worldPos.x < 0.0f || worldPos.y < 0.0f ) return false;
	if ( worldPos.x >= GetWorldSize().x ) return false;
	if ( worldPos.y >= GetWorldSize().y ) return false;
	return true;
}

WaterQuad* World::CreateWaterQuad()
{
	WaterQuad* WQ = new WaterQuad();

	WQ->AddObserver(this);
	zWaterQuads.insert(WQ);
	zWaterQuadsEdited = true;

	// Notify Observers
	WaterQuadCreatedEvent WQCE;
	WQCE.zQuad = WQ;
	NotifyObservers(&WQCE);

	return WQ;
}

void World::DeleteWaterQuad( WaterQuad* quad )
{
	delete quad;
	zWaterQuads.erase(quad);
	zWaterQuadsEdited = false;
}

float World::GetWaterDepthAt( const Vector2& worldPos )
{
	float curDistance = 0.0f;

	// Calc Ground Height At Position
	Vector3 groundPos( worldPos.x, GetHeightAt(worldPos), worldPos.y );

	// Ray VS Water Quads
	auto zWaterQuads_end = zWaterQuads.end();
	for( auto i = zWaterQuads.begin(); i != zWaterQuads_end; ++i )
	{
		WorldPhysics::CollisionData coll;

		if ( WorldPhysics::RayVSTriangle(groundPos, Vector3(0.0f, 1.0f, 0.0f), (*i)->GetPosition(0), (*i)->GetPosition(1), (*i)->GetPosition(2), coll) )
		{

		}
		else if ( WorldPhysics::RayVSTriangle(groundPos, Vector3(0.0f, 1.0f, 0.0f), (*i)->GetPosition(2), (*i)->GetPosition(3), (*i)->GetPosition(1), coll) )
		{

		}

		if ( coll.collision )
		{
			if ( coll.distance > curDistance )
			{
				curDistance = coll.distance;
			}
		}
	}

	return curDistance;
}

Vector2 World::GetWorldCenter() const
{
	Vector2 WorldSize = GetWorldSize();
	return Vector2( WorldSize.x / 2.0f, WorldSize.y / 2.0f );
}

const std::string& World::GetFileName() const
{
	static const std::string empty = "";
	if ( !zFile ) return empty;
	return zFile->GetFileName();
}

float World::GetAmountOfTexture( const Vector2& worldPos, const std::string& name )
{
	if ( !IsInside(worldPos) ) throw("Outside World!");

	// Get Sector
	Sector* sector = GetSectorAtWorldPos(worldPos);
	if (!sector) throw("Sector Not Loaded!");

	// Calculate Local Position
	Vector2 localPos;
	localPos.x = fmod(worldPos.x, FSECTOR_WORLD_SIZE) / FSECTOR_WORLD_SIZE;
	localPos.y = fmod(worldPos.y, FSECTOR_WORLD_SIZE) / FSECTOR_WORLD_SIZE;

	// Check Each Texture Name
	BlendValues values = sector->GetBlendingAt(localPos);

	float val = 0.0f;
	for( unsigned int x=0; x<SECTOR_BLEND_CHANNELS; ++x )
	{
		if ( !strcmp(sector->GetTextureName(x), name.c_str()) )
		{
			val += values[x];
		}
	}

	return val;
}
