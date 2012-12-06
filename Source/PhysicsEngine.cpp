#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine()
{

}

PhysicsEngine::~PhysicsEngine()
{

}

CollisionData PhysicsEngine::GetCollisionRayMesh( Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh )
{
	CollisionData cd;
	
	if(Mesh* mesh = dynamic_cast<Mesh*>(imesh))
	{
		this->DoCollisionRayVsMesh(rayOrigin, rayDirection, mesh, cd);
	}
	else
	{
		MaloW::Debug("Failed to cast iMesh to Mesh in PhysicsEngine.cpp");
	}

	return cd;
}

CollisionData PhysicsEngine::GetCollisionRayTerrain( Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr )
{
	CollisionData cd;

	if(Terrain* terrain = dynamic_cast<Terrain*>(iTerr))
	{
		this->DoCollisionRayVsTriangles(rayOrigin, rayDirection, 
			terrain->GetVerticesPointer(), terrain->GetNrOfVertices(), 
			terrain->GetIndicesPointer(), terrain->GetNrOfIndices(), terrain->GetWorldMatrix(), cd);
	}
	else
	{
		MaloW::Debug("Failed to cast iTerrain to Terrain in PhysicsEngine.cpp");
	}

	return cd;
}

void PhysicsEngine::DoCollisionRayVsMesh(Vector3 rayOrigin, Vector3 rayDirection, 
		Mesh* mesh, CollisionData& cd)
{
	MaloW::Array<MeshStrip*>* strips = mesh->GetStrips();
	for(int i = 0; i < strips->size(); i++)
	{
		this->DoCollisionRayVsTriangles(rayOrigin, rayDirection, 
			strips->get(i)->getVerts(), strips->get(i)->getNrOfVerts(), 
			strips->get(i)->getIndicies(), strips->get(i)->getNrOfIndicies(), mesh->GetWorldMatrix(), cd);
	}
}

void PhysicsEngine::DoCollisionRayVsTriangles(Vector3 rayOrigin, Vector3 rayDirection, 
		Vertex* vertices, int nrOfVertices, int* indices, int nrOfIndices, D3DXMATRIX worldMat, CollisionData& cd)
{
	for(int i = 0; i < nrOfIndices; i += 3)
	{
		CollisionData tempCD;

		Vertex vert0 = vertices[indices[i]];
		Vertex vert1 = vertices[indices[i + 1]];
		Vertex vert2 = vertices[indices[i + 2]];

		// D3DX STUFF
		
		D3DXVECTOR4 pos0;
		D3DXVec3Transform(&pos0, &vert0.pos, &worldMat);
		D3DXVECTOR4 pos1;
		D3DXVec3Transform(&pos1, &vert1.pos, &worldMat);
		D3DXVECTOR4 pos2;
		D3DXVec3Transform(&pos2, &vert2.pos, &worldMat);

		Vector3 v0 = Vector3(pos0.x, pos0.y, pos0.z);
		Vector3 v1 = Vector3(pos1.x, pos1.y, pos1.z);
		Vector3 v2 = Vector3(pos2.x, pos2.y, pos2.z);

		// END OF D3DX STUFF

		if(this->DoCollisionTestRayVsTriangle(rayOrigin, rayDirection, v0, v1, v2, tempCD))
		{
			if(tempCD.distance < cd.distance)
			{
				cd.distance = tempCD.distance;
				cd.position = tempCD.position;
				cd.collision = true;
			}
		}
	}
}

bool PhysicsEngine::DoCollisionTestRayVsTriangle(Vector3 rayOrigin, Vector3 rayDirection, 
		Vector3 v0, Vector3 v1, Vector3 v2, CollisionData& tempCD)
{
	float eps = 0.000001f;
	Vector3 e1 = v1 - v0;
	Vector3 e2 = v2 - v0;
	Vector3 q = e2.GetCrossProduct(rayDirection);
	float a = e1.GetDotProduct(q);

	if(a > -eps && a < eps)
		return false;
	
	float f = 1 / a;
	Vector3 s = rayOrigin - v0;
	float u = f * (s.GetDotProduct(q));

	if(u < 0.0f)
		return false;

	Vector3 r = e1.GetCrossProduct(s);
	float v = f * (rayDirection.GetDotProduct(r));

	if(v < 0.0f || u + v > 1.0f)
		return false;

	float t = f * (e2.GetDotProduct(q));

	Vector3 collPos;
	float w = 1 - (u + v);

	collPos.x = (u * v0.x + v * v1.x + w * v2.x);
	collPos.y = (u * v0.y + v * v1.y + w * v2.y);
	collPos.z = (u * v0.z + v * v1.z + w * v2.z);
			
	tempCD.collision = true;
	tempCD.position = collPos;
	tempCD.distance = (rayOrigin - tempCD.position).GetLength();

	return true;
}

