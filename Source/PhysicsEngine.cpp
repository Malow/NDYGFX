#include "PhysicsEngine.h"

//////////////////// Overloaded ////////////////////
CollisionData PhysicsEngine::GetCollision( Vector3 rayOrigin, Vector3 rayDirection, iMesh* mesh )
{
	return this->GetCollisionRayMesh(rayOrigin, rayDirection, mesh);
}

CollisionData PhysicsEngine::GetCollision( Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr )
{
	return this->GetCollisionRayTerrain(rayOrigin, rayDirection, iTerr);
}

CollisionData PhysicsEngine::GetCollision( iMesh* mesh1, iMesh* mesh2 )
{
	return this->GetCollisionMeshMesh(mesh1, mesh2);
}

CollisionData PhysicsEngine::GetCollision( iMesh* mesh, iTerrain* terr )
{
	return this->GetCollisionMeshTerrain(mesh, terr);
}

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
		MaloW::Debug("Failed to cast iMesh to Mesh in PhysicsEngine.cpp");

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
		MaloW::Debug("Failed to cast iTerrain to Terrain in PhysicsEngine.cpp");

	return cd;
}

CollisionData PhysicsEngine::GetCollisionMeshMesh(iMesh* mesh1, iMesh* mesh2)
{
	CollisionData cd;

	if(Mesh* m1 = dynamic_cast<Mesh*>(mesh1))
	{
		if(Mesh* m2 = dynamic_cast<Mesh*>(mesh2))
		{

		}
		else
			MaloW::Debug("Failed to cast iMesh* mesh2 to Mesh in PhysicsEngine.cpp");
	}
	else
		MaloW::Debug("Failed to cast iMesh* mesh1 to Mesh in PhysicsEngine.cpp");

	return cd;
}

CollisionData PhysicsEngine::GetCollisionMeshTerrain( iMesh* mesh, iTerrain* terr )
{
	CollisionData cd;

	return cd;
}



//////////////////////////////////////////////////////////////////////////
/////////////////////////////    Privates      ///////////////////////////
//////////////////////////////////////////////////////////////////////////
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

void PhysicsEngine::DoCollisionMeshVsMesh( Mesh* m1, Mesh* m2, CollisionData& cd )
{
	MaloW::Array<MeshStrip*>* strips1 = m1->GetStrips();
	MaloW::Array<MeshStrip*>* strips2 = m2->GetStrips();
	for(int i = 0; i < strips1->size(); i++)
	{
		for(int u = 0; u < strips2->size(); u++)
		{
			this->DoCollisionTrianglesVsTriangles(strips1->get(i)->getVerts(), strips1->get(i)->getNrOfVerts(),
				strips1->get(i)->getIndicies(), strips1->get(i)->getNrOfIndicies(), m1->GetWorldMatrix(), 
				strips2->get(u)->getVerts(), strips2->get(u)->getNrOfVerts(), strips2->get(u)->getIndicies(),
				strips2->get(u)->getNrOfIndicies(), m2->GetWorldMatrix(), cd);
		}
	}
}

void PhysicsEngine::DoCollisionRayVsTriangles(Vector3 rayOrigin, Vector3 rayDirection, 
		Vertex* vertices, int nrOfVertices, int* indices, int nrOfIndices, D3DXMATRIX worldMat, CollisionData& cd)
{
	if(!indices)
	{
		for(int i = 0; i < nrOfVertices; i += 3)
		{
			CollisionData tempCD;

			Vertex vert0 = vertices[i];
			Vertex vert1 = vertices[i + 1];
			Vertex vert2 = vertices[i + 2];

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

			if(this->DoCollisionRayVsTriangle(rayOrigin, rayDirection, v0, v1, v2, tempCD))
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
	else
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

			if(this->DoCollisionRayVsTriangle(rayOrigin, rayDirection, v0, v1, v2, tempCD))
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
}

bool PhysicsEngine::DoCollisionRayVsTriangle(Vector3 rayOrigin, Vector3 rayDirection, 
		Vector3 v0, Vector3 v1, Vector3 v2, CollisionData& tempCD)
{
	float eps = 0.000001f;
	Vector3 e1 = v1 - v0;
	Vector3 e2 = v2 - v0;
	Vector3 q = e2.GetCrossProduct(rayDirection);
	float a = e1.GetDotProduct(q);

	if(a > -eps && a < eps)
		return false;
	
	float f = 1.0f / a;
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

	collPos = (v0 * w) + (v1 * u) + (v2 * v);
			
	tempCD.collision = true;
	tempCD.position = collPos;
	tempCD.distance = (rayOrigin - tempCD.position).GetLength();

	return true;
}

void PhysicsEngine::DoCollisionTrianglesVsTriangles(Vertex* vert1, int nrOfVerts1, int* inds1, int nrOfInds1, 
	D3DXMATRIX worldMat1, Vertex* vert2, int nrOfVerts2, int* inds2, int nrOfInds2, 
	D3DXMATRIX worldMat2, CollisionData& cd )
{
	if(!inds1)
	{
		// Mesh1 isnt using inds
		if(!inds2)
		{
			// Mesh 2 isnt using inds either
			for(int i = 0; i < nrOfVerts1; i += 3)
			{
				CollisionData tempCD;

				Vertex vert00 = vert1[i];
				Vertex vert01 = vert1[i + 1];
				Vertex vert02 = vert1[i + 2];

				// D3DX STUFF

				D3DXVECTOR4 pos00;
				D3DXVec3Transform(&pos00, &vert00.pos, &worldMat1);
				D3DXVECTOR4 pos01;
				D3DXVec3Transform(&pos01, &vert01.pos, &worldMat1);
				D3DXVECTOR4 pos02;
				D3DXVec3Transform(&pos02, &vert02.pos, &worldMat1);

				Vector3 v00 = Vector3(pos00.x, pos00.y, pos00.z);
				Vector3 v01 = Vector3(pos01.x, pos01.y, pos01.z);
				Vector3 v02 = Vector3(pos02.x, pos02.y, pos02.z);

				// END OF D3DX STUFF

				for(int u = 0; u < nrOfVerts2; u++)
				{
					Vertex vert10 = vert2[i];
					Vertex vert11 = vert2[i + 1];
					Vertex vert12 = vert2[i + 2];

					// D3DX STUFF

					D3DXVECTOR4 pos10;
					D3DXVec3Transform(&pos10, &vert00.pos, &worldMat1);
					D3DXVECTOR4 pos11;
					D3DXVec3Transform(&pos11, &vert01.pos, &worldMat1);
					D3DXVECTOR4 pos12;
					D3DXVec3Transform(&pos12, &vert02.pos, &worldMat1);

					Vector3 v10 = Vector3(pos10.x, pos10.y, pos10.z);
					Vector3 v11 = Vector3(pos11.x, pos11.y, pos11.z);
					Vector3 v12 = Vector3(pos12.x, pos12.y, pos12.z);


					if(this->DoCollisionTriangleVsTriangle(v00, v01, v02, v10, v11, v12, tempCD))
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

		}
		else
		{
			// Mesh 1 isnt using inds, mesh 2 is


		}

	}
	else
	{
		// both is using inds
		/*
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

			if(this->DoCollisionRayVsTriangle(rayOrigin, rayDirection, v0, v1, v2, tempCD))
			{
				if(tempCD.distance < cd.distance)
				{
					cd.distance = tempCD.distance;
					cd.position = tempCD.position;
					cd.collision = true;
				}
			}
		}
		*/
	}
}

bool PhysicsEngine::DoCollisionTriangleVsTriangle(Vector3 v01, Vector3 v02, Vector3 v03, 
	Vector3 v10, Vector3 v11, Vector3 v12, CollisionData& tempCD)
{

	return true;
}













