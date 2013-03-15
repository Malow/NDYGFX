#pragma once

#include "Sector.h"
#include "Observer.h"
#include "WorldFile.h"
#include "CircleAndRect.h"
#include "EntQuadTree.h"

#include <string>
#include <map>

class Entity;
class World;
class WaterQuad;

class WorldAnchor
{
private:
	WorldAnchor() : position(0.0f, 0.0f), radius(100.0f)
	{
	}

	~WorldAnchor()
	{
	}
public:
	Vector2 position;
	float radius;

	friend class World;
};


class World : public Observed, public Observer
{
private:
	// File
	WorldFile* zFile;
	bool zReadOnly;

	// Sectors
	Sector*** zSectors;
	std::set<Vector2UINT> zLoadedSectors;
	std::map<Sector*, unsigned int> zLoadedEntityCount;

	// Water
	std::set<WaterQuad*> zWaterQuads;
	bool zWaterQuadsEdited;

	// Entities
	EntQuadTree zEntTree;
	
	// World Settings
	unsigned int zNrOfSectorsWidth;
	unsigned int zNrOfSectorsHeight;
	Vector3 zStartCamPos;
	Vector3 zStartCamRot;
	Vector3 zAmbient;
	Vector3 zSunDir;
	Vector3 zSunColor;
	float zSunIntensity;

	// Anchors
	std::set<WorldAnchor*> zAnchors;

public:
	World(Observer* observer, const std::string& fileName="", bool readOnly=true) throw(...);
	World(Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight);
	virtual ~World();

	// Save World To File
	void SaveFile();
	void SaveFileAs( const std::string& fileName );
	const std::string& GetFileName() const;

	// World Settings
	unsigned int GetNumSectorsWidth() const;
	unsigned int GetNumSectorsHeight() const;
	Vector2 GetWorldCenter() const;
	Vector2 GetWorldSize() const;
	bool IsInside( const Vector2& worldPos );

	// Water Quads
	WaterQuad* CreateWaterQuad();
	void DeleteWaterQuad( WaterQuad* quad );
	inline const std::set<WaterQuad*>& GetWaterQuads() const { return zWaterQuads; }
	float GetWaterDepthAt( const Vector2& worldPos );

	// Sun Settings
	void SetSunProperties( const Vector3& dir, const Vector3& color, float intensity );
	const Vector3& GetSunDir() const { return zSunDir; }
	const Vector3& GetSunColor() const { return zSunColor; }
	const float& GetSunIntensity() const { return zSunIntensity; }

	// World Start Camera
	void SetStartCamera( const Vector3& pos, const Vector3& rot );
	const Vector3& GetStartCamPos() const;
	const Vector3& GetStartCamRot() const;

	// Ambient Settings
	inline const Vector3& GetWorldAmbient() const { return zAmbient; }
	void SetWorldAmbient( const Vector3& vector );

	// Load All Sectors
	void LoadAllSectors();

	// Get Ambient At World Position
	Vector3 GetAmbientAtWorldPos( const Vector2& worldPos );
	
	// Modify Height Functions
	void ModifyHeightAt( float x, float y, float value ) throw(...);
	float GetHeightAt( const Vector2 &worldPos ) throw(...);
	void SetHeightAt( float x, float y, float value ) throw(...);
	float CalcHeightAtWorldPos( const Vector2& worldPos ) throw(...);

	// Normals
	Vector3 CalcNormalAt( const Vector2& worldPos ) throw(...);
	void SetNormalAt( const Vector2& worldPos, const Vector3& val ) throw(...);
	void GenerateSectorNormals( const Vector2UINT& sectorCoords );

	// Modify Blend Functions
	float GetAmountOfTexture(const Vector2& worldPos, const std::string& name );
	void ModifyBlendingAt( const Vector2& worldPos, const BlendValues& val );
	BlendValues GetBlendingAt( const Vector2& worldPos );
	void SetBlendingAt( const Vector2& worldPos, const BlendValues& val );

	// Entity Functions
	Entity* CreateEntity(unsigned int entityType);
	void RemoveEntity(Entity* entity);

	// Sector Functions
	Sector* GetSector( const Vector2UINT& sectorCoords ) throw(...);
	Sector* GetSector( unsigned int x, unsigned int y ) throw(...);
	Sector* GetSectorAtWorldPos( const Vector2& worldPos );
	Vector2UINT WorldPosToSector( const Vector2& worldPos ) const;
	bool IsSectorLoaded( const Vector2UINT& sectorCoords ) const;
	void SetSectorTexture( unsigned int x, unsigned int y, const std::string& texture, unsigned int index );
	const char* const GetSectorTexture( unsigned int x, unsigned int y, unsigned int index );

	// Anchors
	WorldAnchor* CreateAnchor();
	void DeleteAnchor( WorldAnchor*& anchor );

	// Logic
	void Update();

	// AI Grid
	void SetBlockingAt( const Vector2& pos, const bool& flag );
	bool IsBlockingAt( const Vector2& pos );

	// Data Access
	unsigned int GetAINodesInCircle(const Vector2& center, float radius, std::set<Vector2>& out) const;
	unsigned int GetEntitiesInRect(const Rect& rect, std::set<Entity*>& out, unsigned int typeFilter=0) const;
	unsigned int GetEntitiesInCircle(const Vector2& center, float radius, std::set<Entity*>& out, unsigned int typeFilter=0) const;
	unsigned int GetSectorsInCicle(const Vector2& center, float radius, std::set<Vector2UINT>& out) const;
	unsigned int CountEntitiesInRect(const Rect& rect) const;
	unsigned int GetHeightNodesInCircle(const Vector2& center, float radius, std::set<Vector2>& out) const;
	unsigned int GetTextureNodesInCircle(const Vector2& center, float radius, std::set<Vector2>& out) const;
	const std::set< Vector2UINT >& GetLoadedSectors() const { return zLoadedSectors; }

protected:
	// Engine Events
	virtual void OnEvent( Event* e );
};