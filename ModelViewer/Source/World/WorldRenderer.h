#pragma once

#include "World.h"
#include <GraphicsEngine.h>
#include <map>
#include <fstream>

enum UPDATEENUM
{
	UPDATE_NOTHING=0,
	UPDATE_HEIGHTMAP=1,
	UPDATE_BLENDMAP=2,
	UPDATE_TEXTURES=4,
	UPDATE_AIGRID=8,
	UPDATE_ALL=15
};

class WorldRenderer : Observer
{
	std::vector< iTerrain* > zTerrain;
	std::map< iTerrain*, std::vector<unsigned char> > zAIGrids;

	World* zWorld;
	GraphicsEngine* zGraphics;
	std::map< Entity*, iMesh* > zEntities;

	std::map< Vector2UINT, UPDATEENUM > zUpdatesRequired;
	bool zShowAIMap;

public:
	WorldRenderer(World* world, GraphicsEngine* graphics);
	virtual ~WorldRenderer();

	CollisionData GetCollisionDataWithGround();
	CollisionData Get3DRayCollisionDataWithGround();
	Entity* Get3DRayCollisionWithMesh();
	iMesh* GetEntityMesh(Entity* entity);
	float GetYPosFromHeightMap(float x, float y);

	void Update();
	void ToggleAIGrid( bool state );

	virtual void OnEvent( Event* e );

protected:
	void UpdateSectorTextures( const Vector2UINT& sectorCoords );
	void UpdateSectorHeightMap( const Vector2UINT& sectorCoords );
	void UpdateSector( const Vector2UINT& sectorCoords );
	void UpdateSectorBlendMap( const Vector2UINT& sectorCoords );
	void UpdateSectorAIGrid( const Vector2UINT& sectorCoords );
	void CreateTerrain( const Vector2UINT& sectorCoords );
	iTerrain* GetTerrain( const Vector2UINT& sectorCoords );

	void CreateEntity( Entity* e );
	void SetEntityGraphics( Entity* e );
	void SetEntityTransformation( Entity* e );
	void DeleteEntity( Entity* e );
};