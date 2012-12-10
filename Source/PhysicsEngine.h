#pragma once


#include "iPhysicsEngine.h"
#include "Terrain.h"
#include "Mesh.h"
#include "MaloW.h"
#include "MaloWFileDebug.h"

class PhysicsEngine : public iPhysicsEngine
{
private:
	void DoCollisionRayVsMesh(Vector3 rayOrigin, Vector3 rayDirection, 
		Mesh* mesh, CollisionData& cd);
	void DoCollisionRayVsTriangles(Vector3 rayOrigin, Vector3 rayDirection, 
		Vertex* vertices, int nrOfVertices, int* indices, int nrOfIndices, D3DXMATRIX worldMat, CollisionData& cd);
	bool DoCollisionTestRayVsTriangle(Vector3 rayOrigin, Vector3 rayDirection, 
		Vector3 v0, Vector3 v1, Vector3 v2, CollisionData& tempCD);
	
public:
	PhysicsEngine();
	virtual ~PhysicsEngine();

	virtual CollisionData GetCollisionRayMesh(Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh);
	virtual CollisionData GetCollisionRayTerrain(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr);

};