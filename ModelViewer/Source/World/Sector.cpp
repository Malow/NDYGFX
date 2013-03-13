#include "Sector.h"


Sector::Sector() : zEditedFlag(false)
{
}


Sector::~Sector()
{
}


void Sector::Reset()
{
	for( unsigned int x=0; x<SECTOR_HEIGHT_SIZE*SECTOR_HEIGHT_SIZE; ++x )
	{
		zHeightMap[x] = 0.0f;
	}

	for( unsigned int x=0; x<SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE; ++x )
	{
		zBlendMap[x*4] = 1.0f;
		zBlendMap[x*4+1] = 0.0f;
		zBlendMap[x*4+2] = 0.0f;
		zBlendMap[x*4+3] = 0.0f;

		zBlendMap2[x*4] = 0.0f;
		zBlendMap2[x*4+1] = 0.0f;
		zBlendMap2[x*4+2] = 0.0f;
		zBlendMap2[x*4+3] = 0.0f;
	}

	SetTextureName(0, "TerrainTexture.png");
	SetTextureName(1, "Green.png");
	SetTextureName(2, "Blue.png");
	SetTextureName(3, "Red.png");

	SetTextureName(4, "TerrainTexture.png");
	SetTextureName(5, "Green.png");
	SetTextureName(6, "Blue.png");
	SetTextureName(7, "Red.png");

	zAmbient[0] = 0.0f;
	zAmbient[1] = 0.0f;
	zAmbient[2] = 0.0f;

	ResetNormals();

	SetEdited(false);
}

float Sector::GetHeightAt( float x, float y ) const throw(...)
{
	if ( x < 0.0f || x >= 1.0f || y < 0.0f || y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(x * SECTOR_HEIGHT_SIZE) / SECTOR_HEIGHT_SIZE;
	float snapY = floor(y * SECTOR_HEIGHT_SIZE) / SECTOR_HEIGHT_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * SECTOR_HEIGHT_SIZE);
	unsigned int scaledY = (unsigned int)(snapY * SECTOR_HEIGHT_SIZE);

	// Set Values
	return zHeightMap[ scaledY * SECTOR_HEIGHT_SIZE + scaledX ];
}

void Sector::SetHeightAt( float x, float y, float val ) throw(...)
{
	if ( x < 0.0f || x >= 1.0f || y < 0.0f || y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(x * SECTOR_HEIGHT_SIZE) / SECTOR_HEIGHT_SIZE;
	float snapY = floor(y * SECTOR_HEIGHT_SIZE) / SECTOR_HEIGHT_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * SECTOR_HEIGHT_SIZE);
	unsigned int scaledY = (unsigned int)(snapY * SECTOR_HEIGHT_SIZE);

	// Set Values
	zHeightMap[ scaledY * SECTOR_HEIGHT_SIZE + scaledX ] = val;

	SetEdited(true);
}

void Sector::ModifyBlendingAt( const Vector2& pos, const BlendValues& val )
{
	if ( pos.x < 0.0f || pos.x >= 1.0f || pos.y < 0.0f || pos.y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(pos.x * (float)SECTOR_BLEND_SIZE) / (float)SECTOR_BLEND_SIZE;
	float snapY = floor(pos.y * (float)SECTOR_BLEND_SIZE) / (float)SECTOR_BLEND_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * (float)(SECTOR_BLEND_SIZE));
	unsigned int scaledY = (unsigned int)(snapY * (float)(SECTOR_BLEND_SIZE));

	// Get Old Values
	BlendValues curValues;
	for( unsigned int i=0; i<4; ++i )
	{
		curValues[i] = zBlendMap[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ];
		curValues[i+4] = zBlendMap2[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ];
	}

	for( unsigned int x=0; x<SECTOR_BLEND_CHANNELS; ++x )
	{
		curValues[x] += val[x];
	}

	curValues.Saturate();

	// Set Values
	for( unsigned int i=0; i<4; ++i )
	{
		zBlendMap[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ] = curValues[i];
		zBlendMap2[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ] = curValues[i+4];
	}

	// Changed
	SetEdited(true);
}

void Sector::SetBlendingAt( const Vector2& pos, const BlendValues& val )
{
	if ( pos.x < 0.0f || pos.x >= 1.0f || pos.y < 0.0f || pos.y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(pos.x * (float)SECTOR_BLEND_SIZE) / (float)SECTOR_BLEND_SIZE;
	float snapY = floor(pos.y * (float)SECTOR_BLEND_SIZE) / (float)SECTOR_BLEND_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * (float)(SECTOR_BLEND_SIZE));
	unsigned int scaledY = (unsigned int)(snapY * (float)(SECTOR_BLEND_SIZE));

	BlendValues curValues = val;
	curValues.Normalize();

	// Set Values
	for( unsigned int i=0; i<4; ++i )
	{
		zBlendMap[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ] = curValues[i];
		zBlendMap2[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ] = curValues[i+4];
	}

	SetEdited(true);
}

BlendValues Sector::GetBlendingAt( const Vector2& pos ) const
{
	if ( pos.x < 0.0f || pos.x >= 1.0f || pos.y < 0.0f || pos.y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(pos.x * (float)SECTOR_BLEND_SIZE) / (float)SECTOR_BLEND_SIZE;
	float snapY = floor(pos.y * (float)SECTOR_BLEND_SIZE) / (float)SECTOR_BLEND_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * (float)(SECTOR_BLEND_SIZE));
	unsigned int scaledY = (unsigned int)(snapY * (float)(SECTOR_BLEND_SIZE));

	BlendValues val;

	for( unsigned int i=0; i<4; ++i )
	{
		val[i] = zBlendMap[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ];
		val[i+4] = zBlendMap2[ (scaledY * (SECTOR_BLEND_SIZE) + scaledX) * 4 + i ];
	}

	return val;
}


const char* const Sector::GetTextureName( unsigned int index ) const
{
	if ( index > 7 )
	{
		throw("Index Out Of Range");
	}
	else if ( index > 3 )
	{
		return &zTextureNames2[(index-4)*TEXTURE_NAME_LENGTH];
	}
	else
	{
		return &zTextureNames[index*TEXTURE_NAME_LENGTH];
	}
}


void Sector::SetTextureName( unsigned int index, const std::string& name )
{
	if ( name.length() >= TEXTURE_NAME_LENGTH ) throw("Texture Name Too Long!");

	if ( index > 7 )
	{
		throw("Index Out Of Range!");
	}
	else if ( index > 3 )
	{
		memset( &zTextureNames2[(index-4)*TEXTURE_NAME_LENGTH], 0, TEXTURE_NAME_LENGTH );
		memcpy( &zTextureNames2[(index-4)*TEXTURE_NAME_LENGTH], &name[0], name.length() );
	}
	else
	{
		memset( &zTextureNames[index*TEXTURE_NAME_LENGTH], 0, TEXTURE_NAME_LENGTH );
		memcpy( &zTextureNames[index*TEXTURE_NAME_LENGTH], &name[0], name.length() );
	}

	SetEdited(true);
}

void Sector::SetAmbient( const Vector3& ambient )
{
	for( unsigned int i=0; i != 3; ++i )
	{
		if ( zAmbient[i] != ambient[i] )
		{
			zAmbient[i] = ambient[i];
			SetEdited(true);
		}
	}
}

void Sector::SetBlocking( const Vector2& pos, bool flag )
{
	if ( pos.x < 0.0f || pos.x >= 1.0f || pos.y < 0.0f || pos.y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(pos.x * (float)SECTOR_AI_GRID_SIZE) / (float)SECTOR_AI_GRID_SIZE;
	float snapY = floor(pos.y * (float)SECTOR_AI_GRID_SIZE) / (float)SECTOR_AI_GRID_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * (float)(SECTOR_AI_GRID_SIZE));
	unsigned int scaledY = (unsigned int)(snapY * (float)(SECTOR_AI_GRID_SIZE));

	// Set Values
	zAiGrid[ scaledY * SECTOR_AI_GRID_SIZE + scaledX ] = flag;

	// Changed
	SetEdited(true);
}

bool Sector::GetBlocking( const Vector2& pos ) const
{
	if ( pos.x < 0.0f || pos.x >= 1.0f || pos.y < 0.0f || pos.y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(pos.x * (float)SECTOR_AI_GRID_SIZE) / (float)SECTOR_AI_GRID_SIZE;
	float snapY = floor(pos.y * (float)SECTOR_AI_GRID_SIZE) / (float)SECTOR_AI_GRID_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * (float)(SECTOR_AI_GRID_SIZE));
	unsigned int scaledY = (unsigned int)(snapY * (float)(SECTOR_AI_GRID_SIZE));

	// Set Values
	return zAiGrid[ scaledY * SECTOR_AI_GRID_SIZE + scaledX ];
}

void Sector::SetNormalAt( const Vector2& pos, const Vector3& val )
{
	if ( pos.x < 0.0f || pos.x >= 1.0f || pos.y < 0.0f || pos.y >= 1.0f )
		throw("Out Of Bounds!");

	// Find pixel
	float snapX = floor(pos.x * FSECTOR_NORMALS_SIZE) / FSECTOR_NORMALS_SIZE;
	float snapY = floor(pos.y * FSECTOR_NORMALS_SIZE) / FSECTOR_NORMALS_SIZE;

	unsigned int scaledX = (unsigned int)(snapX * FSECTOR_NORMALS_SIZE);
	unsigned int scaledY = (unsigned int)(snapY * FSECTOR_NORMALS_SIZE);

	// Set Values
	for( unsigned int i=0; i<3; ++i )
	{
		zNormals[ (scaledY * SECTOR_NORMALS_SIZE + scaledX) * 3 + i ] = val[i];
	}

	SetEdited(true);
}

void Sector::ResetBlendMap2()
{
	for( unsigned int x=0; x<SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE; ++x )
	{
		zBlendMap2[x*4+0] = 0.0f;
		zBlendMap2[x*4+1] = 0.0f;
		zBlendMap2[x*4+2] = 0.0f;
		zBlendMap2[x*4+3] = 0.0f;
	}
}

void Sector::ResetNormals()
{
	for( unsigned int x=0; x<SECTOR_NORMALS_SIZE*SECTOR_NORMALS_SIZE; ++x )
	{
		zNormals[x*3+0] = 0.0f;
		zNormals[x*3+1] = 1.0f;
		zNormals[x*3+2] = 0.0f;
	}
}