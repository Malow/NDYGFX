#include "Entity.h"



Entity::Entity( unsigned int type, const Vector3& pos, const Vector3& rot, const Vector3& scale ) :
	zType(type),
	zPosition(pos),
	zRotation(rot),
	zScale(scale),
	zSelected(false)
{
}

Entity::~Entity()
{
	EntityDeletedEvent EDE;
	EDE.entity = this;
	NotifyObservers( &EDE );
}

void Entity::SetPosition( const Vector3& pos )
{
	if ( zPosition != pos )
	{
		EntityMovingEvent EME(this, pos);
		NotifyObservers(&EME);

		SetEdited(true);
		zPosition = pos;

		EntityUpdatedEvent EUE;
		EUE.entity = this;
		NotifyObservers(&EUE);
	}
}

void Entity::SetRotation( const Vector3& rot )
{
	if ( zRotation != rot )
	{
		SetEdited(true);
		zRotation = rot;
		
		EntityUpdatedEvent EUE;
		EUE.entity = this;
		NotifyObservers(&EUE);
	}
}

void Entity::SetScale( const Vector3& scale )
{
	if ( zScale != scale )
	{
		SetEdited(true);
		zScale = scale;

		EntityUpdatedEvent EUE;
		EUE.entity = this;
		NotifyObservers(&EUE);
	}
}

void Entity::SetSelected( bool selected )
{
	if ( zSelected != selected )
	{
		zSelected = selected;

		EntitySelectedEvent ESE;
		ESE.entity = this;
		NotifyObservers( &ESE );
	}
}

void Entity::SetType( unsigned int& newType )
{
	if ( newType != zType )
	{
		SetEdited(true);
		zType = newType;

		EntityChangedTypeEvent ECTE;
		ECTE.entity = this;
		NotifyObservers(&ECTE);
	}
}