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
			this->BoundingSphereCollision = false;
			this->distance = 999999.0f;
			this->posx = 0.0f;
			this->posy = 0.0f;
			this->posz = 0.0f;
		}
		bool collision;
		bool BoundingSphereCollision;
		float distance;
		float posx;
		float posy;
		float posz;
	};

	class DECLDIR iPhysicsEngine
	{	
	public:
		iPhysicsEngine() {};
		virtual ~iPhysicsEngine() {};

		virtual CollisionData GetCollisionRayMesh(Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh) = 0;
		virtual CollisionData GetCollisionRayTerrain(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iterr) = 0;
		virtual CollisionData GetCollisionMeshMesh(iMesh* mesh1, iMesh* mesh2) = 0;
		virtual CollisionData GetCollisionMeshTerrain(iMesh* mesh, iTerrain* terr) = 0;

		// Overloaded
		virtual CollisionData GetCollision(Vector3 rayOrigin, Vector3 rayDirection, iMesh* mesh) = 0;
		virtual CollisionData GetCollision(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr) = 0;
		virtual CollisionData GetCollision(iMesh* mesh1, iMesh* mesh2) = 0;
		virtual CollisionData GetCollision(iMesh* mesh, iTerrain* terr) = 0;
	};
}