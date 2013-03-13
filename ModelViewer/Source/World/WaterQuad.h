#pragma once

#include "Observer.h"
#include <Vector.h>

class WaterQuad;
class World;


class WaterQuadDeletedEvent : public Event
{
public:
	WaterQuad* zQuad;
};

class WaterQuadEditedEvent : public Event
{
public:
	WaterQuad* zQuad;
};

class WaterQuad : public Observed
{
	Vector3 zPositions[4];

	WaterQuad();
	virtual ~WaterQuad();
public:

	void SetPosition( unsigned int i, const Vector3& pos );
	const Vector3& GetPosition( unsigned int i ) const;
	Vector3 CalcCenter() const;

	friend World;
};