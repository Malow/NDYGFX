#pragma once

#include "WorldRendererSettings.h"
#include "World.h"
#include <GraphicsEngine.h>
#include <map>

enum UPDATEENUM
{
	UPDATE_NOTHING=0,
	UPDATE_CREATE=1,
	UPDATE_HEIGHTMAP=2,
	UPDATE_BLENDMAP=4,
	UPDATE_TEXTURES=8,
	UPDATE_AIGRID=16,
	UPDATE_ALL=31,
	UPDATE_DELETE=32
};

struct WaterCollisionData
{
	// Specifies the quad hit
	WaterQuad* quad;
	
	// Specifies the corner hit
	unsigned int cornerIndex;

	// Position of collision
	Vector3 position;
};

class WorldRenderer : Observer
{
	// Settings
	WorldRendererSettings zSettings;

	// Components
	World* zWorld;
	GraphicsEngine* zGraphics;

	// Terrain
	std::vector< iTerrain* > zTerrain;
	std::map< Vector2UINT, UPDATEENUM > zUpdatesRequired;

	// AI Grid
	std::map< iTerrain*, std::vector<unsigned char> > zAIGrids;
	bool zShowAIMap;

	// Entities
	std::map< Entity*, iMesh* > zEntities;
	std::set< Entity* > zEntsToUpdate;
	Vector3 zLastEntUpdatePos;

	// Water
	std::map< WaterQuad*, iWaterPlane* > zWaterQuads;

	struct WaterCubes
	{
		iMesh* zCubes[4];
	};

	std::map< WaterQuad*, WaterCubes > zWaterBoxes;
	bool zShowWaterBoxes;

	// Billboard collections
	unsigned int zGrassDensity;		// Number of grass objects per terrain.
	float zGrassNearDistance;		// Distance between camera and grass
	float zGrassFarDistance;		// Distance between grass and far clip
	float zGrassUpdateDistance;		// Distance you have to walk to update grass
	float zGrassWidthMin;			// Minimum grass width
	float zGrassWidthMax;			// Maximum grass width
	float zGrassHeightMin;			// Minimum grass height
	float zGrassHeightMax;			// Maximum grass height
	float zGrassNeightbourDistance;	// Minimal distance between grass
	Vector2 zLastGrassUpdatePos;	// Last camera position when grass was generated
	std::map<Vector2UINT, iBillboardCollection*> zGrass;
	
public:
	WorldRenderer(World* world, GraphicsEngine* graphics);
	virtual ~WorldRenderer();

	WaterCollisionData GetCollisionWithWaterBoxes();
	CollisionData Get3DRayCollisionDataWithGround();
	Entity* Get3DRayCollisionWithMesh();
	float GetYPosFromHeightMap(float x, float y);

	// Returns true when there is more to be processed
	bool Update();

	// Show or don't show AI grid
	void ToggleAIGrid(bool state);
	
	// Show or don't show water corner boxes
	void ToggleWaterBoxes(bool flag);

protected:
	virtual void OnEvent( Event* e );
	void UpdateWaterBoxes( WaterQuad* water );
	void UpdateTerrain();

	void SetEntityGraphics( Entity* e );
	void SetEntityTransformation( Entity* e );
	void DeleteEntity( Entity* e );

	void GenerateGrass(const Vector2UINT& terrainCoords);
};