#pragma once

#include <Vector.h>
#include "Observer.h"

class Entity;
class World;

class EntityDeletedEvent : public Event
{
public:
	Entity* entity;
};

class EntityChangedTypeEvent : public Event
{
public:
	Entity* entity;
};

class EntityUpdatedEvent : public Event
{
public:
	Entity* entity;
};

class EntitySelectedEvent : public Event
{
public:
	Entity* entity;
};

class Entity : public Observed
{
private:
	Vector3 zPosition;
	Vector3 zRotation;
	Vector3 zScale;
	unsigned int zType;

	bool zSelected;
	bool zEdited;

	Entity(unsigned int type, const Vector3& pos=Vector3(0.0f,0.0f,0.0f), const Vector3& rot=Vector3(0.0f,0.0f,0.0f), const Vector3& scale=Vector3(1.0f,1.0f,1.0f));
	virtual ~Entity();

public:
	void SetPosition(const Vector3& pos);
	inline Vector3 GetPosition() const { return zPosition; }

	void SetRotation(const Vector3& rot);
	inline Vector3 GetRotation() const { return zRotation; }

	void SetScale(const Vector3& scale);
	inline Vector3 GetScale() const { return zScale; }

	void SetSelected(bool selected);
	inline bool GetSelected() const { return zSelected; }

	inline void SetEdited(bool flag) { zEdited=flag; }
	inline bool IsEdited() const { return zEdited; }

	void SetType( unsigned int& newType );
	inline unsigned int GetType() const { return zType; }

	friend class World;
};