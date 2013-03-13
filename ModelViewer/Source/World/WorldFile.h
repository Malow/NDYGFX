#pragma once

#include <string>
#include <fstream>
#include "Sector.h"
#include "Observer.h"
#include "WorldEvents.h"
#include <array>

/*
File Order:
WorldHeader
SectorHeaders
TextureNames
HeightMaps
BlendMaps
Entities
AIGrid
BlendMaps2
TextureNames2
Water
*/

// The Maximum of water quads saved
static unsigned int MAX_NUM_WATER_QUADS = 100000;

enum WORLDFILE_OPENMODE
{
	OPEN_LOAD = 1,
	OPEN_SAVE = 2,
	OPEN_EDIT = 3
};

class WorldFileHeader
{
public:
	unsigned int versionNr;
	unsigned int width;
	unsigned int height;

	// Global Ambient Light
	float ambientLight[3];

	// Sun Settings
	float sunDirection[3];
	float sunColor[3];
	float sunIntensity;

	// Camera Position
	float camPos[3];
	float camRot[3];

	WorldFileHeader() :
		versionNr(0),
		width(10),
		height(10)
	{
		ambientLight[0] = 0.4f;
		ambientLight[1] = 0.4f;
		ambientLight[2] = 0.4f;

		sunDirection[0] = 0.5f;
		sunDirection[1] = -1.0f;
		sunDirection[2] = 0.0f;

		sunColor[0] = 1.0f;
		sunColor[1] = 1.0f;
		sunColor[2] = 1.0f;

		sunIntensity = 1.0f;

		camPos[0] = 0.0f;
		camPos[1] = 0.0f;
		camPos[2] = 0.0f;

		camRot[0] = 0.0f;
		camRot[1] = 0.0f;
		camRot[2] = 0.0f;
	}
};


struct WorldFileSectorHeader
{
	bool generated;
	float ambientColor[3];
};


struct EntityStruct
{
	float pos[3];
	float rot[3];
	float scale[3];
	unsigned int type;
};

class WorldFile : public Observed
{
	WorldFileHeader zHeader;
	std::string zFileName;
	std::fstream *zFile;
	WORLDFILE_OPENMODE zMode;
	unsigned int zNumSectors;
	
	std::vector<Vector3> zWaters;

public:
	WorldFile( Observer* observer, const std::string& fileName, WORLDFILE_OPENMODE mode );
	virtual ~WorldFile();

	// Save Entities
	void WriteEntities( const std::array<EntityStruct,256>& entities, unsigned int sectorIndex );
	bool ReadEntities( unsigned int sectorIndex, std::array<EntityStruct,256>& out );

	// Open The World File, Reads the world header
	void Open();

	// Writes height map to file
	void WriteHeightMap( const float* const data, unsigned int sectorIndex );
	bool ReadHeightMap( float* data, unsigned int sectorIndex );

	// Sector header
	void WriteSectorHeader( const WorldFileSectorHeader& header, unsigned int sectorIndex );
	bool ReadSectorHeader( WorldFileSectorHeader& headerOut, unsigned int sectorIndex );
	
	// Texture names
	void WriteTextureNames( const char* data, unsigned int x );
	bool ReadTextureNames( char* data, unsigned int sectorIndex );
	void WriteTextureNames2( const char* data, unsigned int x );
	bool ReadTextureNames2( char* data, unsigned int sectorIndex );

	// Writes blend map to file
	void WriteBlendMap( const float* const data, unsigned int sectorIndex );
	bool ReadBlendMap( float* data, unsigned int sectorIndex );
	void WriteBlendMap2( const float* const data, unsigned int sectorIndex );
	bool ReadBlendMap2( float* data, unsigned int sectorIndex );

	// Normals
	void WriteNormals( const float* const data, unsigned int sectorIndex );
	bool ReadNormals( float* data, unsigned int sectorIndex );

	// AI Grid
	void WriteAIGrid( const AIGrid& grid, unsigned int sectorIndex );
	bool ReadAIGrid( AIGrid& grid, unsigned int sectorIndex );

	// Water
	bool ReadWater( std::vector<Vector3>& waters );
	void WriteWater( const std::vector<Vector3>& waters );

	// Read the world header
	void ReadHeader();

	// World Header
	const WorldFileHeader& GetWorldHeader() const { return zHeader; }
	void SetStartCamera( const Vector3& pos, const Vector3& rot );
	void SetSunProperties( const Vector3& dir, const Vector3& color, float intensity );
	void SetWorldAmbient( const Vector3& ambient );
	Vector3 GetStartCamPos();
	Vector3 GetStartCamRot();
	Vector3 GetWorldAmbient();
	Vector3 GetSunDirection();
	Vector3 GetSunColor();
	float GetSunIntensity();

	inline const std::string& GetFileName() const { return zFileName; }

private:
	unsigned int GetBlendNamesBegin() const;
	unsigned int GetBlendsBegin() const;
	unsigned int GetHeightsBegin() const;
	unsigned int GetSectorHeadersBegin() const;
	unsigned int GetSectorTexturesBegin() const;
	unsigned int GetEntitiesBegin() const;
	unsigned int GetAIGridBegin() const;
	unsigned int GetBlendsBegin2() const;
	unsigned int GetSectorTexturesBegin2() const;
	unsigned int GetNormalsBegin() const;
	unsigned int GetWaterBegin() const;
	unsigned int GetEnding() const;
};