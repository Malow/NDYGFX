#pragma once


#include "iPhysicsEngine.h"
#include "Terrain.h"
#include "Mesh.h"
#include "FBXMesh.h"
#include "AnimatedMesh.h"
#include "MaloW.h"
#include "MaloWFileDebug.h"
#include "WaterPlane.h"

class PhysicsEngine : public iPhysicsEngine
{
private:
	Vertex* tempVerts;
	int tempVertsSize;

	void DoCollisionRayVsFBXMesh(Vector3 rayOrigin, Vector3 rayDirection, FBXMesh* mesh, CollisionData& cd);

	void DoCollisionRayVsMesh(Vector3 rayOrigin, Vector3 rayDirection, 
		Mesh* mesh, CollisionData& cd);
	void DoCollisionRayVsTriangles(Vector3 rayOrigin, Vector3 rayDirection, 
		Vertex* vertices, int nrOfVertices, int* indices, int nrOfIndices, D3DXMATRIX worldMat, CollisionData& cd);
	void DoCollisionRayVsTrianglesNM(Vector3 rayOrigin, Vector3 rayDirection, 
		VertexNormalMap* vertices, int nrOfVertices, int* indices, int nrOfIndices, D3DXMATRIX worldMat, CollisionData& cd);
	bool DoCollisionRayVsTriangle(Vector3 rayOrigin, Vector3 rayDirection, 
		Vector3 v0, Vector3 v1, Vector3 v2, CollisionData& tempCD);

	void DoCollisionMeshVsMesh(Mesh* m1, Mesh* m2, CollisionData& cd);
	void DoCollisionTrianglesVsTriangles(Vector3 m1Pos, Vertex* vert1, int nrOfVerts1, int* inds1, int nrOfInds1, D3DXMATRIX worldMat1,
		Vertex* vert2, int nrOfVerts2, int* inds2, int nrOfInds2, D3DXMATRIX worldMat2, CollisionData& cd);
	void DoCollisionTrianglesVsTrianglesNM(Vector3 m1Pos, VertexNormalMap* vert1, int nrOfVerts1, int* inds1, int nrOfInds1, D3DXMATRIX worldMat1,
		VertexNormalMap* vert2, int nrOfVerts2, int* inds2, int nrOfInds2, D3DXMATRIX worldMat2, CollisionData& cd);
	bool DoCollisionTriangleVsTriangle(Vector3 v00, Vector3 v01, Vector3 v02, Vector3 v10, Vector3 v11, Vector3 v12,
		CollisionData& tempCD);
	bool DoCollisionSphereVsRay(BoundingSphere bs, D3DXMATRIX world, float scale, Vector3 rayOrigin, Vector3 rayDirection);
	bool DoCollisionSphereVsSphere(BoundingSphere bs1, D3DXMATRIX world1, float scale1, BoundingSphere bs2, D3DXMATRIX world2, float scale2);
	CollisionData DoCollisionSphereVsRayDetailed(BoundingSphere bs, D3DXMATRIX world, float scale, Vector3 rayOrigin, Vector3 rayDirection);
	CollisionData DoCollisionSphereVsSphereDetailed(BoundingSphere bs1, D3DXMATRIX world1, float scale1, BoundingSphere bs2, D3DXMATRIX world2, float scale2);

	void DoSpecialCollisionRayVsTerrainTriangles(Vector3 rayOrigin, Vector3 rayDirection, Vertex* vertices,
		int nrOfVertices, int* indices, int nrOfIndices, D3DXMATRIX worldMat, CollisionData& cd);

public:
	PhysicsEngine();
	virtual ~PhysicsEngine();

	bool FrustrumVsSphere(D3DXPLANE planes[], BoundingSphere& bs, D3DXMATRIX& world, float& scale);

	// Functions
	virtual CollisionData GetCollisionRayMesh(Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh);
	virtual CollisionData GetCollisionRayTerrain(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iterr);
	virtual CollisionData GetCollisionMeshMesh(iMesh* mesh1, iMesh* mesh2);
	virtual CollisionData GetCollisionMeshTerrain(iMesh* mesh, iTerrain* terr);

	// Only BoundingSphere functions
	virtual CollisionData GetCollisionRayMeshBoundingOnly(Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh);;
	virtual CollisionData GetCollisionRayTerrainBoundingOnly(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iterr);
	virtual CollisionData GetCollisionMeshMeshBoundingOnly(iMesh* mesh1, iMesh* mesh2);
	virtual CollisionData GetCollisionMeshTerrainBoundingOnly(iMesh* mesh, iTerrain* terr);


	// Special
	virtual CollisionData GetSpecialCollisionRayTerrain(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iterr, 
		float distanceBetweenVerticies);


	// Overloaded functions
	virtual CollisionData GetCollision(Vector3 rayOrigin, Vector3 rayDirection, iMesh* mesh);
	virtual CollisionData GetCollision(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr);
	virtual CollisionData GetCollision(iMesh* mesh1, iMesh* mesh2);
	virtual CollisionData GetCollision(iMesh* mesh, iTerrain* terr);

	// Only BoundingSphere functions
	virtual CollisionData GetCollisionBoundingOnly(Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh);
	virtual CollisionData GetCollisionBoundingOnly(Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iterr);
	virtual CollisionData GetCollisionBoundingOnly(iMesh* mesh1, iMesh* mesh2);
	virtual CollisionData GetCollisionBoundingOnly(iMesh* mesh, iTerrain* terr);
};