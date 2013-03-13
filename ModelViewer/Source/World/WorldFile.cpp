#include "WorldFile.h"
#include <fstream>



struct HeightMap
{
	float height[SECTOR_HEIGHT_SIZE*SECTOR_HEIGHT_SIZE];
};


struct BlendMap
{
	float blend[SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE*4];
};

struct NormalsStruct
{
	float normal[SECTOR_NORMALS_SIZE*SECTOR_NORMALS_SIZE*3];
};


struct TextureNamesStruct
{
	char data[TEXTURE_NAME_LENGTH*4];
};


WorldFile::WorldFile( Observer* observer, const std::string& fileName, WORLDFILE_OPENMODE mode ) :
	Observed(observer),
	zFileName(fileName),
	zMode(mode),
	zNumSectors(0),
	zFile(0)
{
}


WorldFile::~WorldFile()
{
	if ( zFile ) delete zFile, zFile=0;
}


void WorldFile::Open()
{
	if ( zFile ) return;

	auto format = std::ios::in | std::ios::binary;
	if ( zMode == OPEN_EDIT || zMode == OPEN_SAVE ) format = format | std::ios::out | std::ios::ate;

	// Open File
	zFile = new std::fstream(zFileName, format);

	// Did it fail?
	if ( !zFile->is_open() )
	{
		// Create File
		std::ofstream s(zFileName);
		if ( !s.good() )
			throw("Failed Creating File!");
		s.close();

		// Reopen
		zFile->open(zFileName, format);
		if ( !zFile->is_open() )
			throw("Failed Opening File!");
	}

	if ( zMode == OPEN_SAVE )
	{
		// New header
		NotifyObservers(&WorldHeaderCreateEvent(this,zHeader));

		// Save number of sectors
		zNumSectors = zHeader.width * zHeader.height;

		// Expand File
		std::streampos end = zFile->tellp();
		zFile->seekp(0, std::ios::beg);
		unsigned int fileSize = (unsigned int)(end - zFile->tellp());

		if ( fileSize < GetEnding() )
		{
			std::vector<unsigned char> v;
			v.resize(10*1024*1024);
			while( fileSize < GetEnding() )
			{
				unsigned int missing = GetEnding() - fileSize;
				unsigned int thisTurn = ( missing > sizeof(v)? sizeof(v) : missing );
				zFile->write(reinterpret_cast<char*>(&v[0]), thisTurn);
				fileSize += thisTurn;
			}
		}

		// Save header
		zFile->seekp(0, std::ios::beg);
		zFile->write(reinterpret_cast<const char*>(&zHeader), sizeof(WorldFileHeader));

		// Switch Directly To Edit Move
		zMode = OPEN_EDIT;
	}
	else if ( zMode == OPEN_LOAD )
	{
		// Check Size
		std::streampos beg = zFile->tellg();
		zFile->seekp(0, std::ios::end);
		unsigned int size = (unsigned int)(zFile->tellg() - beg);

		if ( size == 0 )
			throw("Empty File!");

		if ( size < sizeof(WorldFileHeader) )
			throw("File Doesn't Have Header!");

		// Read File Header
		zFile->seekg( 0, std::ios::beg );
		zFile->read( reinterpret_cast<char*>(&zHeader), sizeof(WorldFileHeader) );

		// Save number of sectors
		zNumSectors = zHeader.width * zHeader.height;

		// Header Loaded
		NotifyObservers( &WorldHeaderLoadedEvent(this,zHeader) );
	}
	else if ( zMode == OPEN_EDIT )
	{
		// Check Size
		std::streampos end = zFile->tellp();
		zFile->seekp(0, std::ios::beg);
		unsigned int size = (unsigned int)(end - zFile->tellp());

		if ( size == 0 )
			throw("Empty File!");

		if ( size < sizeof(WorldFileHeader) )
			throw("File Doesn't Have Header!");

		// Read File Header
		zFile->seekg( 0, std::ios::beg );
		zFile->read( reinterpret_cast<char*>(&zHeader), sizeof(WorldFileHeader) );

		// Save number of sectors
		zNumSectors = zHeader.width * zHeader.height;

		if ( size < GetEnding() )
		{
			std::vector<unsigned char> v;
			v.resize(10*1024*1024);
			zFile->seekp(0, std::ios::end);
			while( size < GetEnding() )
			{
				unsigned int missing = GetEnding() - size;
				unsigned int thisTurn = ( missing > sizeof(v)? sizeof(v) : missing );
				zFile->write(reinterpret_cast<char*>(&v[0]), thisTurn);
				size += thisTurn;
			}
		}

		// Header Loaded
		NotifyObservers( &WorldHeaderLoadedEvent(this,zHeader) );
	}
}


void WorldFile::ReadHeader()
{
	if( !zFile ) Open();
}


unsigned int WorldFile::GetSectorHeadersBegin() const
{
	return sizeof(WorldFileHeader);
}


void WorldFile::WriteSectorHeader( const WorldFileSectorHeader& header, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetSectorHeadersBegin() + sectorIndex * sizeof(WorldFileSectorHeader), std::ios::beg );
		zFile->write((const char*)&header, sizeof(WorldFileSectorHeader));
	}
}


bool WorldFile::ReadSectorHeader( WorldFileSectorHeader& headerOut, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetSectorHeadersBegin() + sectorIndex * sizeof(WorldFileSectorHeader), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read((char*)&headerOut, sizeof(WorldFileSectorHeader)) ) return false;
	return true;
}

unsigned int WorldFile::GetSectorTexturesBegin() const
{
	return GetSectorHeadersBegin() + zNumSectors * sizeof(WorldFileSectorHeader);
}

void WorldFile::WriteTextureNames( const char* data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetSectorTexturesBegin() + sectorIndex * sizeof(TextureNamesStruct), std::ios::beg );
		zFile->write(data, sizeof(TextureNamesStruct));
	}
}

bool WorldFile::ReadTextureNames( char* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetSectorTexturesBegin() + sectorIndex * sizeof(TextureNamesStruct), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read(data, sizeof(TextureNamesStruct)) ) return false;
	return true;
}

unsigned int WorldFile::GetHeightsBegin() const
{
	return GetSectorTexturesBegin() + zNumSectors * sizeof(TextureNamesStruct);
}

void WorldFile::WriteHeightMap( const float* const data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetHeightsBegin() + sectorIndex * sizeof(HeightMap), std::ios::beg );
		zFile->write((const char*)data,sizeof(HeightMap));
	}
}

bool WorldFile::ReadHeightMap( float* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetHeightsBegin() + sectorIndex * sizeof(HeightMap), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read((char*)data, sizeof(HeightMap)) ) return false;
	return true;
}

unsigned int WorldFile::GetBlendsBegin() const
{
	return GetHeightsBegin() + zNumSectors * sizeof(HeightMap);
}


void WorldFile::WriteBlendMap( const float* const data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetBlendsBegin() + sectorIndex * sizeof(BlendMap), std::ios::beg );
		zFile->write((const char*)data, sizeof(BlendMap));
	}
}

bool WorldFile::ReadBlendMap( float* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetBlendsBegin() + sectorIndex * sizeof(BlendMap), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read((char*)data, sizeof(BlendMap)) ) return false;;
	return true;
}

unsigned int WorldFile::GetEntitiesBegin() const
{
	return GetBlendsBegin() + zNumSectors * sizeof(BlendMap);
}

void WorldFile::WriteEntities( const std::array<EntityStruct, 256>& entities, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetEntitiesBegin() + sectorIndex * sizeof(EntityStruct) * 256, std::ios::beg );
		zFile->write(reinterpret_cast<const char*>(entities.data()), sizeof(EntityStruct) * 256);
	}
}

bool WorldFile::ReadEntities( unsigned int sectorIndex, std::array<EntityStruct, 256>& out )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetEntitiesBegin() + sectorIndex * sizeof(EntityStruct) * 256, std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read(reinterpret_cast<char*>(out.data()), sizeof(EntityStruct) * 256) ) return false;
	return true;
}

unsigned int WorldFile::GetAIGridBegin() const
{
	return GetEntitiesBegin() + zNumSectors * sizeof(EntityStruct) * 256;
}

void WorldFile::WriteAIGrid( const AIGrid& grid, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetAIGridBegin() + sectorIndex * sizeof(AIGrid), std::ios::beg );
		zFile->write(reinterpret_cast<const char*>(&grid), sizeof(AIGrid) );
	}
}

bool WorldFile::ReadAIGrid( AIGrid& grid, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetAIGridBegin() + sectorIndex * sizeof(AIGrid), std::ios::beg );
	if ( zFile->eof() ) { zFile->clear(); return false; }
	if ( !zFile->read(reinterpret_cast<char*>(&grid), sizeof(AIGrid)) ) { zFile->clear(); return false; }
	return true;
}

unsigned int WorldFile::GetBlendsBegin2() const
{
	return GetAIGridBegin() + zNumSectors * sizeof(AIGrid);
}

void WorldFile::WriteBlendMap2( const float* const data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetBlendsBegin2() + sectorIndex * sizeof(BlendMap), std::ios::beg );
		zFile->write((const char*)data, sizeof(BlendMap));
	}
}

bool WorldFile::ReadBlendMap2( float* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetBlendsBegin2() + sectorIndex * sizeof(BlendMap), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read((char*)data, sizeof(BlendMap)) ) return false;

	/*
	// Try Data
	for( unsigned int x=0; x<SECTOR_BLEND_SIZE*SECTOR_BLEND_SIZE*4; ++x )
	{
		if ( data[x] != 0.0 ) return true;
	}
	*/

	return true;
}

unsigned int WorldFile::GetSectorTexturesBegin2() const
{
	return GetBlendsBegin2() + zNumSectors * sizeof(BlendMap);
}

void WorldFile::WriteTextureNames2( const char* data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetSectorTexturesBegin2() + sectorIndex * sizeof(TextureNamesStruct), std::ios::beg );
		zFile->write(data, sizeof(TextureNamesStruct));
	}
}
bool WorldFile::ReadTextureNames2( char* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetSectorTexturesBegin2() + sectorIndex * sizeof(TextureNamesStruct), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read(data, sizeof(TextureNamesStruct)) ) return false;
	
	// Try Data
	for( unsigned int x=0; x<sizeof(TextureNamesStruct); ++x )
	{
		if ( data[x] != 0 ) return true;
	}
	
	return false;
}

unsigned int WorldFile::GetNormalsBegin() const
{
	return GetSectorTexturesBegin2() + zNumSectors * sizeof(TextureNamesStruct);
}

void WorldFile::WriteNormals( const float* const data, unsigned int sectorIndex )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
		zFile->seekp( GetNormalsBegin() + sectorIndex * sizeof(NormalsStruct), std::ios::beg );
		zFile->write((const char*)data, sizeof(NormalsStruct));
	}
}

bool WorldFile::ReadNormals( float* data, unsigned int sectorIndex )
{
	if ( !zFile ) Open();
	if ( sectorIndex >= zNumSectors ) throw("Sector Index out of range!");
	zFile->seekg( GetNormalsBegin() + sectorIndex * sizeof(NormalsStruct), std::ios::beg );
	if ( zFile->eof() ) return false;
	if ( !zFile->read((char*)data, sizeof(NormalsStruct))) return false;
	
	// Test Data
	if ( data[0] == 0.0f && data[1] == 0.0f && data[2] == 0.0f ) return false;
	
	return true;
}

unsigned int WorldFile::GetWaterBegin() const
{
	return GetNormalsBegin() + zNumSectors * sizeof(NormalsStruct);
}

void WorldFile::WriteWater( const std::vector<Vector3>& waters )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();
		
		// Seek Water
		zFile->seekp( GetWaterBegin(), std::ios::beg );

		// Num Water Quads
		unsigned int numWaterQuads = waters.size()/4;
		if ( numWaterQuads > MAX_NUM_WATER_QUADS ) numWaterQuads = MAX_NUM_WATER_QUADS;
		zFile->write(reinterpret_cast<const char*>(&numWaterQuads), sizeof(unsigned int));

		// Water Quads
		if ( numWaterQuads ) zFile->write(reinterpret_cast<const char*>(&waters[0]), sizeof(Vector3) * 4 * numWaterQuads);
	}
}

bool WorldFile::ReadWater( std::vector<Vector3>& waters )
{
	if ( !zFile ) Open();

	// Check Water
	zFile->seekg( GetWaterBegin(), std::ios::beg );
	if ( zFile->eof() ) return false;
	
	// Check Number of Water Quads
	unsigned int numWaterQuads = 0;
	if ( !zFile->read(reinterpret_cast<char*>(&numWaterQuads), sizeof(unsigned int)) ) return false;

	// Load Water Quads
	waters.resize(numWaterQuads*4);
	if ( numWaterQuads ) if ( !zFile->read(reinterpret_cast<char*>(&waters[0]), numWaterQuads * sizeof(Vector3) * 4) ) return false;

	return true;
}

unsigned int WorldFile::GetEnding() const
{
	return GetWaterBegin() + sizeof(unsigned int) + MAX_NUM_WATER_QUADS * sizeof(Vector3) * 4;
}

void WorldFile::SetStartCamera( const Vector3& pos, const Vector3& rot )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();

		for( unsigned int x=0; x<3; ++x )
		{
			zHeader.camPos[x] = pos[x];
			zHeader.camRot[x] = rot[x];
		}
	
		zFile->seekp(0, std::ios::beg);
		zFile->write(reinterpret_cast<const char*>(&zHeader), sizeof(WorldFileHeader));
	}
}

void WorldFile::SetWorldAmbient( const Vector3& ambient )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();

		for( unsigned int x=0; x<3; ++x )
		{
			zHeader.ambientLight[x] = ambient[x];
		}

		zFile->seekp(0, std::ios::beg);
		zFile->write(reinterpret_cast<const char*>(&zHeader), sizeof(WorldFileHeader));
	}
}

Vector3 WorldFile::GetStartCamPos()
{
	ReadHeader();
	return Vector3(zHeader.camPos[0], zHeader.camPos[1], zHeader.camPos[2] );
}

Vector3 WorldFile::GetStartCamRot()
{
	ReadHeader();
	return Vector3(zHeader.camRot[0], zHeader.camRot[1], zHeader.camRot[2] );
}

Vector3 WorldFile::GetWorldAmbient()
{
	ReadHeader();
	return Vector3(zHeader.ambientLight[0], zHeader.ambientLight[1], zHeader.ambientLight[2] );
}

void WorldFile::SetSunProperties( const Vector3& dir, const Vector3& color, float intensity )
{
	if ( zMode != OPEN_LOAD )
	{
		if ( !zFile ) Open();

		for( unsigned int x=0; x<3; ++x )
		{
			zHeader.sunDirection[x] = dir[x];
			zHeader.sunColor[x] = color[x];
		}

		zHeader.sunIntensity = intensity;

		zFile->seekp(0, std::ios::beg);
		zFile->write(reinterpret_cast<const char*>(&zHeader), sizeof(WorldFileHeader));
	}
}

Vector3 WorldFile::GetSunDirection()
{
	ReadHeader();
	return Vector3(zHeader.sunDirection[0], zHeader.sunDirection[1], zHeader.sunDirection[2]);
}


Vector3 WorldFile::GetSunColor()
{
	ReadHeader();
	return Vector3(zHeader.sunColor[0], zHeader.sunColor[1], zHeader.sunColor[2]);
}

float WorldFile::GetSunIntensity()
{
	ReadHeader();
	return zHeader.sunIntensity;
}