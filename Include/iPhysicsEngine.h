#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"
#include "iMesh.h"
#include "iTerrain.h"

extern "C"
{
	struct DECLDIR CollisionData
	{
		CollisionData()
		{
			this->collision = false;
			this->distance = 999999.0f;
			this->position = Vector3(0, 0, 0);
		}
		bool collision;
		float distance;
		Vector3 position;
	};

	class DECLDIR iPhysicsEngine
	{	
	public:
		iPhysicsEngine() {};
		virtual ~iPhysicsEngine() {};

		virtual CollisionData GetCollisionRayMesh(Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh) = 0;
		virtual CollisionData GetCollisionRayTerrain(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iterr) = 0;
	};
}