#pragma once

// Created by Johansson Rikard 21/11/12 for project Not Dead Yet at BTH(Blekinges Tekniska Högskola)

#include "Vector.h"
#include <vector>
#include <bitset>
#include <array>
#include "BlendValues.h"

static const unsigned int SECTOR_WORLD_SIZE = 32;
static const float FSECTOR_WORLD_SIZE = (float)SECTOR_WORLD_SIZE;

static const unsigned int SECTOR_LENGTH = 64;
static const float FSECTOR_LENGTH = (float)SECTOR_LENGTH;

static const unsigned int SECTOR_BLEND_SIZE = 64;
static const float FSECTOR_BLEND_SIZE = (float)SECTOR_BLEND_SIZE;

static const unsigned int SECTOR_HEIGHT_SIZE = 64;
static const float FSECTOR_HEIGHT_SIZE = (float)SECTOR_HEIGHT_SIZE;

static const unsigned int SECTOR_BLEND_CHANNELS = 8;
static const unsigned int TEXTURE_NAME_LENGTH = 60;

static const unsigned int SECTOR_NORMALS_SIZE = 64;
static const float FSECTOR_NORMALS_SIZE = (float)SECTOR_NORMALS_SIZE;

// AI Grid
static const unsigned int SECTOR_AI_GRID_SIZE = 63;
static const float FSECTOR_AI_GRID_SIZE = (float)SECTOR_AI_GRID_SIZE;

// Data Types
typedef std::bitset<SECTOR_AI_GRID_SIZE * SECTOR_AI_GRID_SIZE> AIGrid;
typedef BlendValuesT<SECTOR_BLEND_CHANNELS> BlendValues;

class Sector
{
private:
	float zHeightMap[SECTOR_HEIGHT_SIZE*SECTOR_HEIGHT_SIZE];
	float zBlendMap[SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE*4];
	float zBlendMap2[SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE*4];
	float zNormals[SECTOR_NORMALS_SIZE*SECTOR_NORMALS_SIZE*3];

	char zTextureNames[TEXTURE_NAME_LENGTH*4];
	char zTextureNames2[TEXTURE_NAME_LENGTH*4];

	float zAmbient[3];
	AIGrid zAiGrid;

	bool zEditedFlag;

public:
	/*Default functions.*/
	Sector();
	virtual ~Sector();

	// Sets the edited flag
	void Reset();

	// Returns the blend map as a float array where each pixel is 4 floats.
	inline float* GetBlendMap() { return &zBlendMap[0]; }

	// Returns The World Normal map
	inline float* GetNormals() { return &zNormals[0]; }
	void ResetNormals();

	// Returns the second blend map
	inline float* GetBlendMap2() { return &zBlendMap2[0]; }
	void ResetBlendMap2();

	// Returns the height map as a float array.
	inline float* GetHeightMap() { return &zHeightMap[0]; }

	// Modifies blending
	void ModifyBlendingAt( const Vector2& nodePos, const BlendValues& val );

	// Sets texture blending at point
	void SetBlendingAt( const Vector2& pos, const BlendValues& val );

	// Returns the texture blending at point
	BlendValues GetBlendingAt( const Vector2& pos ) const;

	// Ambient
	inline Vector3 GetAmbient() const { return Vector3(zAmbient[0], zAmbient[1], zAmbient[2]); }
	void SetAmbient( const Vector3& ambient );

	// AI Grid
	AIGrid& GetAIGrid() { return zAiGrid; }
	void SetBlocking( const Vector2& pos, bool flag );
	bool GetBlocking( const Vector2& pos ) const;

	// Normals
	void SetNormalAt( const Vector2& pos, const Vector3& val );

	/*
	Get the value at point
	Throws when out of bounds 
	*/
	float GetHeightAt( float x, float y ) const throw(...);

	/*
	Sets the height at a point local to the sector
	Sets the edited flag
	Throws when out of bounds
	*/
	void SetHeightAt( float x, float y, float val ) throw(...);

	// Returns The Texture Name (0<=index<4)
	const char* const GetTextureName( unsigned int index ) const;
	void SetTextureName( unsigned int index, const std::string& name );
	inline char* GetTextureNames() { return &zTextureNames[0]; }
	inline char* GetTextureNames2() { return &zTextureNames2[0]; }

	// Inline Functions
	inline int GetSectorLength() { return SECTOR_LENGTH; }
	inline void SetEdited( bool state ) { zEditedFlag = state; }
	inline bool IsEdited() const { return zEditedFlag; }	
};