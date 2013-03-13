#pragma once

#include "Observer.h"

#pragma warning ( push )
#pragma warning ( disable : 4512 )

class World;
class Entity;
class WorldFile;
class WorldFileHeader;
class WaterQuad;

class WorldHeaderLoadedEvent : public Event
{
public:
	WorldFile* file;
	const WorldFileHeader& header;

	WorldHeaderLoadedEvent( WorldFile* file, const WorldFileHeader& header ) :
		file(file),
		header(header)
	{
	}
};

class WorldHeaderCreateEvent : public Event
{
public:
	WorldFile* file;
	WorldFileHeader& header;

	WorldHeaderCreateEvent( WorldFile* file, WorldFileHeader& header ) : 
		file(file),
		header(header)
	{
	}
};

class WorldLoadedEvent : public Event
{
public:
	World* world;
	WorldLoadedEvent( World* world ) : world(world) {}
};

class WorldDeletedEvent : public Event
{
public:
	World* world;
	WorldDeletedEvent( World* world ) : world(world) {}
};

class WorldSunChanged : public Event
{
public:
	World* world;
	WorldSunChanged( World* world ) : world(world) {}
};

class SectorLoadedEvent : public Event
{
public:
	World* world;
	unsigned int x,y;
	SectorLoadedEvent( World* world, unsigned int x, unsigned int y ) : world(world), x(x), y(y) {}
};

class SectorHeightMapChanged : public Event
{
public:
	World* world;
	unsigned int sectorx, sectory;
	float localx, localy;
	SectorHeightMapChanged( World* world, unsigned int sectorx, unsigned int sectory, float localx, float localy ) :
		world(world),
		sectorx(sectorx),
		sectory(sectory),
		localx(localx),
		localy(localy)
	{
	}
};

class SectorBlendMapChanged : public Event
{
public:
	World* world;
	unsigned int sectorx, sectory;
	float localx, localy;
	SectorBlendMapChanged( World* world, unsigned int sectorx, unsigned int sectory, const float& localx, const float& localy ) :
		world(world),
		sectorx(sectorx),
		sectory(sectory),
		localx(localx),
		localy(localy)
	{
	}
};


class SectorNormalChanged : public Event
{
public:
	World* world;
	unsigned int sectorx, sectory;
	float localx, localy;
	SectorNormalChanged( World* world, unsigned int sectorx, unsigned int sectory, const float& localx, const float& localy ) :
		world(world),
		sectorx(sectorx),
		sectory(sectory),
		localx(localx),
		localy(localy)
	{
	}
};

class SectorBlendTexturesChanged : public Event
{
public:
	World* world;
	unsigned int sectorX, sectorY;
	SectorBlendTexturesChanged( World* world, unsigned int sectorX, unsigned int sectorY ) :
		world(world),
		sectorX(sectorX),
		sectorY(sectorY)
	{
	}
};

class SectorAIGridChanged : public Event
{
public:
	World* world;
	unsigned int sectorX, sectorY;
	SectorAIGridChanged( World* world, unsigned int sectorX, unsigned int sectorY ) :
		world(world),
		sectorX(sectorX),
		sectorY(sectorY)
	{
	}
};

class SectorUnloadedEvent : public Event
{
public:
	World* world;
	unsigned int sectorX, sectorY;
	SectorUnloadedEvent( World* world, unsigned int sectorX, unsigned int sectorY ) : 
		world(world), 
		sectorX(sectorX),
		sectorY(sectorY)
	{}
};

class EntityLoadedEvent : public Event
{
public:
	World* world;
	Entity* entity;
	EntityLoadedEvent(World* world, Entity* entity) : world(world), entity(entity) {}
};

class EntityRemovedEvent : public Event
{
public:
	World* world;
	Entity* entity;
	EntityRemovedEvent( World* world, Entity* entity ) : world(world), entity(entity) {}
};

class WaterQuadCreatedEvent : public Event
{
public:
	WaterQuad* zQuad;
};

class WaterQuadLoadedEvent : public Event
{
public:
	WaterQuad* zQuad;
};

#pragma warning (pop)