#pragma once


#include "iPhysicsEngine.h"
#include "Terrain.h"
#include "Mesh.h"
#include "AnimatedMesh.h"
#include "MaloW.h"
#include "MaloWFileDebug.h"

class PhysicsEngine : public iPhysicsEngine
{
private:
	void DoCollisionRayVsMesh(Vector3 rayOrigin, Vector3 rayDirection, 
		Mesh* mesh, CollisionData& cd);
	void DoCollisionRayVsTriangles(Vector3 rayOrigin, Vector3 rayDirection, 
		Vertex* vertices, int nrOfVertices, int* indices, int nrOfIndices, D3DXMATRIX worldMat, CollisionData& cd);
	bool DoCollisionRayVsTriangle(Vector3 rayOrigin, Vector3 rayDirection, 
		Vector3 v0, Vector3 v1, Vector3 v2, CollisionData& tempCD);

	void DoCollisionMeshVsMesh(Mesh* m1, Mesh* m2, CollisionData& cd);
	void DoCollisionTrianglesVsTriangles(Vector3 m1Pos, Vertex* vert1, int nrOfVerts1, int* inds1, int nrOfInds1, D3DXMATRIX worldMat1,
		Vertex* vert2, int nrOfVerts2, int* inds2, int nrOfInds2, D3DXMATRIX worldMat2, CollisionData& cd);
	bool DoCollisionTriangleVsTriangle(Vector3 v00, Vector3 v01, Vector3 v02, Vector3 v10, Vector3 v11, Vector3 v12,
		CollisionData& tempCD);
	bool DoCollisionSphereVsRay(BoundingSphere bs, D3DXMATRIX world, float scale, Vector3 rayOrigin, Vector3 rayDirection);
	bool DoCollisionSphereVsSphere(BoundingSphere bs1, D3DXMATRIX world1, float scale1, BoundingSphere bs2, D3DXMATRIX world2, float scale2);
	
public:
	PhysicsEngine();
	virtual ~PhysicsEngine();

	bool FrustrumVsSphere(D3DXPLANE planes[], BoundingSphere bs, D3DXMATRIX world, float scale);

	virtual CollisionData GetCollisionRayMesh(Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh);
	virtual CollisionData GetCollisionRayTerrain(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr);
	virtual CollisionData GetCollisionMeshMesh(iMesh* mesh1, iMesh* mesh2);
	virtual CollisionData GetCollisionMeshTerrain(iMesh* mesh, iTerrain* terr);

	// Overloaded
	virtual CollisionData GetCollision(Vector3 rayOrigin, Vector3 rayDirection, iMesh* mesh);
	virtual CollisionData GetCollision(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr);
	virtual CollisionData GetCollision(iMesh* mesh1, iMesh* mesh2);
	virtual CollisionData GetCollision(iMesh* mesh, iTerrain* terr);
};