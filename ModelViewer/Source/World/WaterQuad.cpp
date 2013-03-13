#include "WaterQuad.h"


WaterQuad::WaterQuad()
{

}

WaterQuad::~WaterQuad()
{
	WaterQuadDeletedEvent WQDE;
	WQDE.zQuad = this;
	NotifyObservers(&WQDE);
}

void WaterQuad::SetPosition( unsigned int i, const Vector3& pos )
{
	if ( zPositions[i] != pos )
	{
		zPositions[i] = pos;

		WaterQuadEditedEvent WQEE;
		WQEE.zQuad = this;
		NotifyObservers(&WQEE);
	}
}

const Vector3& WaterQuad::GetPosition( unsigned int i ) const
{
	return zPositions[i];
}

Vector3 WaterQuad::CalcCenter() const
{
	float xMin = std::numeric_limits<float>::max();
	float xMax = std::numeric_limits<float>::lowest();

	float yMin = std::numeric_limits<float>::max();
	float yMax = std::numeric_limits<float>::lowest();

	float zMin = std::numeric_limits<float>::max();
	float zMax = std::numeric_limits<float>::lowest();

	for( unsigned int x=0; x<4; ++x )
	{
		const Vector3& pos = GetPosition(x);

		// Min
		xMin = ( xMin < pos.x? xMin : pos.x );
		yMin = ( yMin < pos.y? yMin : pos.y );
		zMin = ( zMin < pos.z? zMin : pos.z );

		// Max
		xMax = ( xMax > pos.x? xMax : pos.x );
		yMax = ( yMax > pos.y? yMax : pos.y );
		zMax = ( zMax > pos.z? zMax : pos.z );
	}

	return Vector3(xMin+xMax, yMin+yMax, zMin+zMax) / 2.0f;
}