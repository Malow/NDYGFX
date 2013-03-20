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

CollisionData PhysicsEngine::GetCollisionBoundingOnly( Vector3 rayOrigin, Vector3 rayDirection, iMesh* mesh )
{
	return this->GetCollisionRayMeshBoundingOnly(rayOrigin, rayDirection, mesh);
}

CollisionData PhysicsEngine::GetCollisionBoundingOnly( Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr )
{
	return this->GetCollisionRayTerrainBoundingOnly(rayOrigin, rayDirection, iTerr);
}

CollisionData PhysicsEngine::GetCollisionBoundingOnly( iMesh* mesh1, iMesh* mesh2 )
{
	return this->GetCollisionMeshMeshBoundingOnly(mesh1, mesh2);
}

CollisionData PhysicsEngine::GetCollisionBoundingOnly( iMesh* mesh, iTerrain* terr )
{
	return this->GetCollisionMeshTerrainBoundingOnly(mesh, terr);
}

PhysicsEngine::PhysicsEngine()
{
	this->tempVerts = NULL;
}

PhysicsEngine::~PhysicsEngine()
{
	if(this->tempVerts)
	{
		delete this->tempVerts;
		this->tempVerts = NULL;
	}
}


CollisionData PhysicsEngine::GetCollisionRayMesh( Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh )
{
	CollisionData cd;
	
	if(Mesh* mesh = dynamic_cast<Mesh*>(imesh))
	{
		if(WaterPlane* wp = dynamic_cast<WaterPlane*>(mesh))
			this->DoCollisionRayVsTriangles(rayOrigin, rayDirection, 
				wp->GetVerts(), wp->GetNrOfVerts(), NULL, 0, wp->GetWorldMatrix(), cd);

		else if(FBXMesh* fbx = dynamic_cast<FBXMesh*>(mesh))
			this->DoCollisionRayVsFBXMesh(rayOrigin, rayDirection, fbx, cd);

		else
			this->DoCollisionRayVsMesh(rayOrigin, rayDirection, mesh, cd);
	}
	else
		MaloW::Debug("Failed to cast iMesh to Mesh in PhysicsEngine.cpp in RayMesh");

	return cd;
}

CollisionData PhysicsEngine::GetCollisionRayTerrain( Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr )
{
	CollisionData cd;

	if(Terrain* terrain = dynamic_cast<Terrain*>(iTerr))
	{
		float scale = max(terrain->GetScale().x, max(terrain->GetScale().y, terrain->GetScale().z));
		if(this->DoCollisionSphereVsRay(terrain->GetBoundingSphere(), terrain->GetWorldMatrix(), scale, 
			rayOrigin, rayDirection))
		{
			cd.BoundingSphereCollision = true;
			this->DoCollisionRayVsTriangles(rayOrigin, rayDirection, 
				terrain->GetVerticesPointer(), terrain->GetNrOfVertices(), 
				terrain->GetIndicesPointer(), terrain->GetNrOfIndices(), terrain->GetWorldMatrix(), cd);
		}
	}
	else
		MaloW::Debug("Failed to cast iTerrain to Terrain in PhysicsEngine.cpp in RayTerrain");

	return cd;
}

CollisionData PhysicsEngine::GetCollisionMeshMesh(iMesh* mesh1, iMesh* mesh2)
{
	CollisionData cd;

	if(Mesh* m1 = dynamic_cast<Mesh*>(mesh1))
	{
		if(Mesh* m2 = dynamic_cast<Mesh*>(mesh2))
		{
			this->DoCollisionMeshVsMesh(m1, m2, cd);
		}
		else
			MaloW::Debug("Failed to cast iMesh* mesh2 to Mesh in PhysicsEngine.cpp in MeshMesh");
	}
	else
		MaloW::Debug("Failed to cast iMesh* mesh1 to Mesh in PhysicsEngine.cpp in MeshMesh");

	return cd;
}

CollisionData PhysicsEngine::GetCollisionMeshTerrain(iMesh* mesh, iTerrain* terr)
{
	CollisionData cd;
	// NYI
	MaloW::Debug("NYI ERROR: Tried using GetCollisionMeshTerrain in PhysicsEngine.cpp, this function is not yet Implemented.");
	return cd;
}


CollisionData PhysicsEngine::GetCollisionRayMeshBoundingOnly( Vector3 rayOrigin, Vector3 rayDirection, iMesh* imesh )
{
	CollisionData cd;

	if(Mesh* mesh = dynamic_cast<Mesh*>(imesh))
	{
		MaloW::Array<MeshStrip*>* strips = mesh->GetStrips();

		if(!strips) // If strips isnt set just return.
			return cd;

		for(int i = 0; i < strips->size(); i++)
		{
			float scale = max(mesh->GetScaling().x, max(mesh->GetScaling().y, mesh->GetScaling().z));

			// Special case for Anis, only first strip has a boundingSphere and it has one covering all strips.
			if(AnimatedMesh* aniMesh = dynamic_cast<AnimatedMesh*>(mesh))
			{
				CollisionData tempCD = this->DoCollisionSphereVsRayDetailed(strips->get(0)->GetBoundingSphere(), 
					mesh->GetWorldMatrix(), scale, rayOrigin, rayDirection);
				if(tempCD.distance > 0.0f)
				{
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
						cd.BoundingSphereCollision = true;
					}
				}
			}
			else
			{
				CollisionData tempCD = this->DoCollisionSphereVsRayDetailed(strips->get(i)->GetBoundingSphere(), 
					mesh->GetWorldMatrix(), scale, rayOrigin, rayDirection);
				if(tempCD.distance > 0.0f)
				{
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
						cd.BoundingSphereCollision = true;
					}
				}
			}
		}
	}
	else
		MaloW::Debug("Failed to cast iMesh to Mesh in PhysicsEngine.cpp in RayMesh");

	return cd;
}

CollisionData PhysicsEngine::GetCollisionRayTerrainBoundingOnly( Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iTerr )
{
	CollisionData cd;

	if(Terrain* terrain = dynamic_cast<Terrain*>(iTerr))
	{
		float scale = max(terrain->GetScale().x, max(terrain->GetScale().y, terrain->GetScale().z));
		CollisionData tempCD = this->DoCollisionSphereVsRayDetailed(terrain->GetBoundingSphere(), 
			terrain->GetWorldMatrix(), scale, rayOrigin, rayDirection);
		if(tempCD.distance > 0.0f)
		{
			if(tempCD.distance < cd.distance)
			{
				cd.distance = tempCD.distance;
				cd.posx = tempCD.posx;
				cd.posy = tempCD.posy;
				cd.posz = tempCD.posz;
				cd.collision = true;
				cd.BoundingSphereCollision = true;
			}
		}
	}
	else
		MaloW::Debug("Failed to cast iTerrain to Terrain in PhysicsEngine.cpp in RayTerrain");

	return cd;
}

CollisionData PhysicsEngine::GetCollisionMeshMeshBoundingOnly(iMesh* mesh1, iMesh* mesh2)
{
	CollisionData cd;

	if(Mesh* m1 = dynamic_cast<Mesh*>(mesh1))
	{
		if(Mesh* m2 = dynamic_cast<Mesh*>(mesh2))
		{
			MaloW::Array<MeshStrip*>* strips1 = m1->GetStrips();
			MaloW::Array<MeshStrip*>* strips2 = m2->GetStrips();
			for(int i = 0; i < strips1->size(); i++)
			{
				for(int u = 0; u < strips2->size(); u++)
				{
					float scale1 = max(m1->GetScaling().x, max(m1->GetScaling().y, m1->GetScaling().z));
					float scale2 = max(m2->GetScaling().x, max(m2->GetScaling().y, m2->GetScaling().z));
					CollisionData tempCD = this->DoCollisionSphereVsSphereDetailed(strips1->get(i)->GetBoundingSphere(), 
						m1->GetWorldMatrix(), scale1, strips2->get(u)->GetBoundingSphere(), m2->GetWorldMatrix(), scale2);
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
						cd.BoundingSphereCollision = true;
					}
				}
			}
		}
		else
			MaloW::Debug("Failed to cast iMesh* mesh2 to Mesh in PhysicsEngine.cpp in MeshMesh");
	}
	else
		MaloW::Debug("Failed to cast iMesh* mesh1 to Mesh in PhysicsEngine.cpp in MeshMesh");

	return cd;
}

CollisionData PhysicsEngine::GetCollisionMeshTerrainBoundingOnly( iMesh* mesh, iTerrain* terr)
{
	CollisionData cd;
	// NYI
	MaloW::Debug("NYI ERROR: Tried using GetCollisionMeshTerrain in PhysicsEngine.cpp, this function is not yet Implemented.");
	return cd;
}



//////////////////////////////////////////////////////////////////////////
/////////////////////////////    Privates      ///////////////////////////
//////////////////////////////////////////////////////////////////////////
bool PhysicsEngine::DoCollisionSphereVsRay(BoundingSphere bs, D3DXMATRIX world, float scale, 
										   Vector3 rayOrigin, Vector3 rayDirection)
{
	D3DXVECTOR4 pos;
	D3DXVec3Transform(&pos, &bs.center, &world);

	Vector3 v = Vector3(rayOrigin.x - pos.x, rayOrigin.y - pos.y, rayOrigin.z - pos.z);
	float rad = bs.radius * scale;
	if(v.GetLength() < rad)
		return true;
	float b = rayDirection.GetDotProduct(v);
	if(b > 0.0f)
		return false;

	float c = v.GetDotProduct(v) - (pow(rad, 2));

	if(b * b - c > 0)
		return true;
	else if(b * b - c == 0)
		return true;
	else
		return false;
}

bool PhysicsEngine::DoCollisionSphereVsSphere(BoundingSphere bs1, D3DXMATRIX world1, float scale1, 
											  BoundingSphere bs2, D3DXMATRIX world2, float scale2)
{
	D3DXVECTOR4 tpos1;
	D3DXVec3Transform(&tpos1, &bs1.center, &world1);

	D3DXVECTOR4 tpos2;
	D3DXVec3Transform(&tpos2, &bs2.center, &world2);

	D3DXVECTOR3 pos1 = D3DXVECTOR3(tpos1.x, tpos1.y, tpos1.z);
	D3DXVECTOR3 pos2 = D3DXVECTOR3(tpos2.x, tpos2.y, tpos2.z);

	float distance = D3DXVec3Length(&(pos2 - pos1));
	float radiuses = bs1.radius * scale1 + bs2.radius * scale2;
	if(distance <= radiuses)
		return true;
	return false;
}

CollisionData PhysicsEngine::DoCollisionSphereVsRayDetailed(BoundingSphere bs, D3DXMATRIX world, float scale, 
										   Vector3 rayOrigin, Vector3 rayDirection)
{
	CollisionData tempCD;
	D3DXVECTOR4 pos;
	D3DXVec3Transform(&pos, &bs.center, &world);
	float rad = bs.radius * scale;

	Vector3 C = Vector3(pos.x, pos.y, pos.z);
	float r = rad;

	Vector3 collisionPoint;
	Vector3 v;
	Vector3 CA = rayOrigin - C;
	float rSquared = r*r;
	float vSquared;
	if(CA.GetDotProduct(CA) <= rSquared)
	{
		collisionPoint = rayOrigin;
	}
	else if(CA.GetDotProduct(rayDirection) <= 0)
	{
		v = CA - rayDirection * (CA.GetDotProduct(rayDirection) / rayDirection.GetDotProduct(rayDirection));
		vSquared = v.GetDotProduct(v);
		if(vSquared <= rSquared)
		{
			rayDirection.Normalize();
			collisionPoint = C + v - (rayDirection * sqrtf(rSquared - vSquared));
			tempCD.collision = true;
		}
		else
		{
			tempCD.collision = false;
		}
	}
	else
	{
		tempCD.collision = false;
	}


	if(tempCD.collision)
	{
		Vector3 colPos = collisionPoint;

		float nrOfdirs = 0.0f;
		if(rayDirection.x > 0.0f)
			nrOfdirs = (colPos.x - rayOrigin.x) / rayDirection.x;
		else if(rayDirection.y > 0.0f)
			nrOfdirs = (colPos.y - rayOrigin.y) / rayDirection.y;
		else
			nrOfdirs = (colPos.z - rayOrigin.z) / rayDirection.z;


		tempCD.BoundingSphereCollision = true;
		tempCD.distance = nrOfdirs;
		tempCD.posx = colPos.x;
		tempCD.posy = colPos.y;
		tempCD.posz = colPos.z;
	}

	return tempCD;
}

CollisionData PhysicsEngine::DoCollisionSphereVsSphereDetailed(BoundingSphere bs1, D3DXMATRIX world1, float scale1, 
											  BoundingSphere bs2, D3DXMATRIX world2, float scale2)
{
	CollisionData tempCD;

	D3DXVECTOR4 tpos1;
	D3DXVec3Transform(&tpos1, &bs1.center, &world1);

	D3DXVECTOR4 tpos2;
	D3DXVec3Transform(&tpos2, &bs2.center, &world2);

	D3DXVECTOR3 pos1 = D3DXVECTOR3(tpos1.x, tpos1.y, tpos1.z);
	D3DXVECTOR3 pos2 = D3DXVECTOR3(tpos2.x, tpos2.y, tpos2.z);

	float distance = D3DXVec3Length(&(pos2 - pos1));
	float radiuses = bs1.radius * scale1 + bs2.radius * scale2;
	if(distance <= radiuses)
	{
		tempCD.collision = true;
		tempCD.BoundingSphereCollision = true;
		tempCD.distance = D3DXVec3Length(&(pos1 - pos2)) * 0.5f;
		D3DXVECTOR3 colPos = (pos1 + pos2) * 0.5f;
		tempCD.posx = colPos.x;
		tempCD.posy = colPos.y;
		tempCD.posz = colPos.z;
	}
	else
	{
		tempCD.collision = false;
		tempCD.BoundingSphereCollision = false;
	}

	return tempCD;
}





void PhysicsEngine::DoCollisionRayVsMesh(Vector3 rayOrigin, Vector3 rayDirection, 
		Mesh* mesh, CollisionData& cd)
{
	MaloW::Array<MeshStrip*>* strips = mesh->GetStrips();
	if ( strips )
	{
		for(int i = 0; i < strips->size(); i++)
		{
			float scale = max(mesh->GetScaling().x, max(mesh->GetScaling().y, mesh->GetScaling().z));
			
			// Special case for Anis, only first strip has a boundingSphere and it has one covering all strips.
			if(AnimatedMesh* aniMesh = dynamic_cast<AnimatedMesh*>(mesh))
			{
				if(this->DoCollisionSphereVsRay(strips->get(0)->GetBoundingSphere(), mesh->GetWorldMatrix(), 
					scale, rayOrigin, rayDirection))
					cd.BoundingSphereCollision = true;
			}
			else
			{
				if(this->DoCollisionSphereVsRay(strips->get(i)->GetBoundingSphere(), mesh->GetWorldMatrix(), 
					scale, rayOrigin, rayDirection))
					cd.BoundingSphereCollision = true;
			}

			if(cd.BoundingSphereCollision)
			{
				this->DoCollisionRayVsTrianglesNM(rayOrigin, rayDirection, 
					strips->get(i)->getVerts(), strips->get(i)->getNrOfVerts(), 
					strips->get(i)->getIndicies(), strips->get(i)->getNrOfIndicies(), mesh->GetWorldMatrix(), cd);
			}
		}
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
			float scale1 = max(m1->GetScaling().x, max(m1->GetScaling().y, m1->GetScaling().z));
			float scale2 = max(m2->GetScaling().x, max(m2->GetScaling().y, m2->GetScaling().z));
			if(this->DoCollisionSphereVsSphere(strips1->get(i)->GetBoundingSphere(), m1->GetWorldMatrix(), scale1, 
				strips2->get(u)->GetBoundingSphere(), m2->GetWorldMatrix(), scale2))
			{
				cd.BoundingSphereCollision = true;
				this->DoCollisionTrianglesVsTrianglesNM(m1->GetPosition(), strips1->get(i)->getVerts(), strips1->get(i)->getNrOfVerts(),
					strips1->get(i)->getIndicies(), strips1->get(i)->getNrOfIndicies(), m1->GetWorldMatrix(), 
					strips2->get(u)->getVerts(), strips2->get(u)->getNrOfVerts(), strips2->get(u)->getIndicies(),
					strips2->get(u)->getNrOfIndicies(), m2->GetWorldMatrix(), cd);
			}
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
			D3DXVECTOR4 pos1;
			D3DXVECTOR4 pos2;
			D3DXVec3Transform(&pos0, &vert0.pos, &worldMat);
			D3DXVec3Transform(&pos1, &vert1.pos, &worldMat);
			D3DXVec3Transform(&pos2, &vert2.pos, &worldMat);

			Vector3 v0 = Vector3(pos0.x, pos0.y, pos0.z);
			Vector3 v1 = Vector3(pos1.x, pos1.y, pos1.z);
			Vector3 v2 = Vector3(pos2.x, pos2.y, pos2.z);

			// END OF D3DX STUFF

			if(this->DoCollisionRayVsTriangle(rayOrigin, rayDirection, v0, v1, v2, tempCD))
			{
				if(tempCD.distance > 0.0f)
				{
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
					}
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
				if(tempCD.distance > 0.0f)
				{
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
					}
				}
			}
		}
	}
}


void PhysicsEngine::DoCollisionRayVsTrianglesNM(Vector3 rayOrigin, Vector3 rayDirection, 
											  VertexNormalMap* vertices, int nrOfVertices, int* indices, int nrOfIndices, D3DXMATRIX worldMat, CollisionData& cd)
{
	if(!indices)
	{
		for(int i = 0; i < nrOfVertices; i += 3)
		{
			CollisionData tempCD;

			VertexNormalMap vert0 = vertices[i];
			VertexNormalMap vert1 = vertices[i + 1];
			VertexNormalMap vert2 = vertices[i + 2];

			// D3DX STUFF

			D3DXVECTOR4 pos0;
			D3DXVECTOR4 pos1;
			D3DXVECTOR4 pos2;
			D3DXVec3Transform(&pos0, &vert0.pos, &worldMat);
			D3DXVec3Transform(&pos1, &vert1.pos, &worldMat);
			D3DXVec3Transform(&pos2, &vert2.pos, &worldMat);

			Vector3 v0 = Vector3(pos0.x, pos0.y, pos0.z);
			Vector3 v1 = Vector3(pos1.x, pos1.y, pos1.z);
			Vector3 v2 = Vector3(pos2.x, pos2.y, pos2.z);

			// END OF D3DX STUFF

			if(this->DoCollisionRayVsTriangle(rayOrigin, rayDirection, v0, v1, v2, tempCD))
			{
				if(tempCD.distance > 0.0f)
				{
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
					}
				}
			}
		}
	}
	else
	{
		for(int i = 0; i < nrOfIndices; i += 3)
		{
			CollisionData tempCD;

			VertexNormalMap vert0 = vertices[indices[i]];
			VertexNormalMap vert1 = vertices[indices[i + 1]];
			VertexNormalMap vert2 = vertices[indices[i + 2]];

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
				if(tempCD.distance > 0.0f)
				{
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
					}
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

	// Unreferenced
	// float t = f * (e2.GetDotProduct(q));

	Vector3 collPos;
	float w = 1 - (u + v);

	collPos = (v0 * w) + (v1 * u) + (v2 * v);
			
	float nrOfdirs = 999999999.0f;
	if(rayDirection.x != 0.0f)
		nrOfdirs = (collPos.x - rayOrigin.x) / rayDirection.x;
	else if(rayDirection.y != 0.0f)
		nrOfdirs = (collPos.y - rayOrigin.y) / rayDirection.y;
	else if(rayDirection.z != 0.0f)
		nrOfdirs = (collPos.z - rayOrigin.z) / rayDirection.z;


	tempCD.collision = true;
	tempCD.posx = collPos.x;
	tempCD.posy = collPos.y;
	tempCD.posz = collPos.z;
	tempCD.distance = nrOfdirs;

	return true;
}

void PhysicsEngine::DoCollisionTrianglesVsTriangles(Vector3 m1Pos, Vertex* vert1, int nrOfVerts1, int* inds1, int nrOfInds1, 
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

				for(int u = 0; u < nrOfVerts2; u += 3)
				{
					Vertex vert10 = vert2[u];
					Vertex vert11 = vert2[u + 1];
					Vertex vert12 = vert2[u + 2];

					// D3DX STUFF

					D3DXVECTOR4 pos10;
					D3DXVec3Transform(&pos10, &vert10.pos, &worldMat2);
					D3DXVECTOR4 pos11;
					D3DXVec3Transform(&pos11, &vert11.pos, &worldMat2);
					D3DXVECTOR4 pos12;
					D3DXVec3Transform(&pos12, &vert12.pos, &worldMat2);

					Vector3 v10 = Vector3(pos10.x, pos10.y, pos10.z);
					Vector3 v11 = Vector3(pos11.x, pos11.y, pos11.z);
					Vector3 v12 = Vector3(pos12.x, pos12.y, pos12.z);


					if(this->DoCollisionTriangleVsTriangle(v00, v01, v02, v10, v11, v12, tempCD))
					{
						tempCD.distance = (m1Pos - Vector3(cd.posx, cd.posy, cd.posz)).GetLength();
						if(tempCD.distance < cd.distance)
						{
							cd.distance = tempCD.distance;
							cd.posx = tempCD.posx;
							cd.posy = tempCD.posy;
							cd.posz = tempCD.posz;
							cd.collision = true;
						}
					}
				}

			}

		}
		else
		{
			// Mesh 1 isnt using inds, mesh 2 is
			MaloW::Debug("Collision MeshVSMesh, Mesh1 isnt using inds, mesh2 is, NOT YET IMPLEMENTED");
		}

	}
	else
	{
		
		if(!inds2)
		{
			// Mesh1 is using inds, mesh 2 isnt
			MaloW::Debug("Collision MeshVSMesh, Mesh1 is using inds, mesh2 isnt, NOT YET IMPLEMENTED");
		}
		else
		{
			// Neither is using inds
			MaloW::Debug("Collision MeshVSMesh, Mesh1 and mesh2 is using inds, NOT YET IMPLEMENTED");
		}

		// To be implemented above:
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


void PhysicsEngine::DoCollisionTrianglesVsTrianglesNM(Vector3 m1Pos, VertexNormalMap* vert1, int nrOfVerts1, int* inds1, int nrOfInds1, 
	D3DXMATRIX worldMat1, VertexNormalMap* vert2, int nrOfVerts2, int* inds2, int nrOfInds2, 
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

				VertexNormalMap vert00 = vert1[i];
				VertexNormalMap vert01 = vert1[i + 1];
				VertexNormalMap vert02 = vert1[i + 2];

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

				for(int u = 0; u < nrOfVerts2; u += 3)
				{
					VertexNormalMap vert10 = vert2[u];
					VertexNormalMap vert11 = vert2[u + 1];
					VertexNormalMap vert12 = vert2[u + 2];

					// D3DX STUFF

					D3DXVECTOR4 pos10;
					D3DXVec3Transform(&pos10, &vert10.pos, &worldMat2);
					D3DXVECTOR4 pos11;
					D3DXVec3Transform(&pos11, &vert11.pos, &worldMat2);
					D3DXVECTOR4 pos12;
					D3DXVec3Transform(&pos12, &vert12.pos, &worldMat2);

					Vector3 v10 = Vector3(pos10.x, pos10.y, pos10.z);
					Vector3 v11 = Vector3(pos11.x, pos11.y, pos11.z);
					Vector3 v12 = Vector3(pos12.x, pos12.y, pos12.z);


					if(this->DoCollisionTriangleVsTriangle(v00, v01, v02, v10, v11, v12, tempCD))
					{
						tempCD.distance = (m1Pos - Vector3(cd.posx, cd.posy, cd.posz)).GetLength();
						if(tempCD.distance < cd.distance)
						{
							cd.distance = tempCD.distance;
							cd.posx = tempCD.posx;
							cd.posy = tempCD.posy;
							cd.posz = tempCD.posz;
							cd.collision = true;
						}
					}
				}

			}

		}
		else
		{
			// Mesh 1 isnt using inds, mesh 2 is
			MaloW::Debug("Collision MeshVSMesh, Mesh1 isnt using inds, mesh2 is, NOT YET IMPLEMENTED");
		}

	}
	else
	{
		
		if(!inds2)
		{
			// Mesh1 is using inds, mesh 2 isnt
			MaloW::Debug("Collision MeshVSMesh, Mesh1 is using inds, mesh2 isnt, NOT YET IMPLEMENTED");
		}
		else
		{
			// Neither is using inds
			MaloW::Debug("Collision MeshVSMesh, Mesh1 and mesh2 is using inds, NOT YET IMPLEMENTED");
		}

		// To be implemented above:
		/*
		for(int i = 0; i < nrOfIndices; i += 3)
		{
			CollisionData tempCD;

			VertexNormalMap vert0 = vertices[indices[i]];
			VertexNormalMap vert1 = vertices[indices[i + 1]];
			VertexNormalMap vert2 = vertices[indices[i + 2]];

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












// Stolen and re-worked a bit from:
// http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/opttritri.txt


#define USE_EPSILON_TEST
#define EPSILON 0.000001f

#define EDGE_EDGE_TEST(V0,U0,U1)                      \
	Bx=U0[i0]-U1[i0];                                   \
	By=U0[i1]-U1[i1];                                   \
	Cx=V0[i0]-U0[i0];                                   \
	Cy=V0[i1]-U0[i1];                                   \
	f=Ay*Bx-Ax*By;                                      \
	d=By*Cx-Bx*Cy;                                      \
	if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))  \
  {                                                   \
  e=Ax*Cy-Ay*Cx;                                    \
  if(f>0)                                           \
	{                                                 \
	if(e>=0 && e<=f) return 1;                      \
}                                                 \
	else                                              \
	{                                                 \
	if(e<=0 && e>=f) return 1;                      \
}                                                 \
}

#define EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2) \
{                                              \
	float Ax,Ay,Bx,By,Cx,Cy,e,d,f;               \
	Ax=V1[i0]-V0[i0];                            \
	Ay=V1[i1]-V0[i1];                            \
	/* test edge U0,U1 against V0,V1 */          \
	EDGE_EDGE_TEST(V0,U0,U1);                    \
	/* test edge U1,U2 against V0,V1 */          \
	EDGE_EDGE_TEST(V0,U1,U2);                    \
	/* test edge U2,U1 against V0,V1 */          \
	EDGE_EDGE_TEST(V0,U2,U0);                    \
}

#define POINT_IN_TRI(V0,U0,U1,U2)           \
{                                           \
	float a,b,c,d0,d1,d2;                     \
	/* is T1 completly inside T2? */          \
	/* check if V0 is inside tri(U0,U1,U2) */ \
	a=U1[i1]-U0[i1];                          \
	b=-(U1[i0]-U0[i0]);                       \
	c=-a*U0[i0]-b*U0[i1];                     \
	d0=a*V0[i0]+b*V0[i1]+c;                   \
	\
	a=U2[i1]-U1[i1];                          \
	b=-(U2[i0]-U1[i0]);                       \
	c=-a*U1[i0]-b*U1[i1];                     \
	d1=a*V0[i0]+b*V0[i1]+c;                   \
	\
	a=U0[i1]-U2[i1];                          \
	b=-(U0[i0]-U2[i0]);                       \
	c=-a*U2[i0]-b*U2[i1];                     \
	d2=a*V0[i0]+b*V0[i1]+c;                   \
	if(d0*d1>0.0)                             \
  {                                         \
  if(d0*d2>0.0) return 1;                 \
}                                         \
}

int coplanar_tri_tri(float N[3],float V0[3],float V1[3],float V2[3],
					 float U0[3],float U1[3],float U2[3])
{
	float A[3];
	short i0,i1;
	/* first project onto an axis-aligned plane, that maximizes the area */
	/* of the triangles, compute indices: i0,i1. */
	A[0]=fabs(N[0]);
	A[1]=fabs(N[1]);
	A[2]=fabs(N[2]);
	if(A[0]>A[1])
	{
		if(A[0]>A[2])
		{
			i0=1;      /* A[0] is greatest */
			i1=2;
		}
		else
		{
			i0=0;      /* A[2] is greatest */
			i1=1;
		}
	}
	else   /* A[0]<=A[1] */
	{
		if(A[2]>A[1])
		{
			i0=0;      /* A[2] is greatest */
			i1=1;
		}
		else
		{
			i0=0;      /* A[1] is greatest */
			i1=2;
		}
	}

	/* test all edges of triangle 1 against the edges of triangle 2 */
	EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2);
	EDGE_AGAINST_TRI_EDGES(V1,V2,U0,U1,U2);
	EDGE_AGAINST_TRI_EDGES(V2,V0,U0,U1,U2);

	/* finally, test if tri1 is totally contained in tri2 or vice versa */
	POINT_IN_TRI(V0,U0,U1,U2);
	POINT_IN_TRI(U0,V0,V1,V2);

	return 0;
}

bool NEWCOMPUTE_INTERVALS(float& VV0, float& VV1, float& VV2, float& D0, float& D1, float& D2, float& D0D1,
						   float& D0D2, float& A, float& B, float& C, float& X0, float& X1)
{ 
	if(D0D1>0.0f) 
	{ 
		A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1; 
	}
	else if(D0D2>0.0f)
	{ 
		A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2; 
	} 
	else if(D1*D2>0.0f || D0!=0.0f) 
	{ 
		A=VV0; B=(VV1-VV0)*D0; C=(VV2-VV0)*D0; X0=D0-D1; X1=D0-D2; 
	} 
	else if(D1!=0.0f) 
	{ 
		A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2; 
	} 
	else if(D2!=0.0f) 
	{ 
		A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1; 
	} 
	else 
	{ 
		return true;
	} 
	return false;
}

bool PhysicsEngine::DoCollisionTriangleVsTriangle(Vector3 v00, Vector3 v01, Vector3 v02, 
	Vector3 v10, Vector3 v11, Vector3 v12, CollisionData& tempCD)
{
	Vector3 e1 = v01 - v00;
	Vector3 e2 = v02 - v00;
	Vector3 n1 = e1.GetCrossProduct(e2);
	float d1 = -n1.GetDotProduct(v00);

	float du0 = n1.GetDotProduct(v10) + d1;
	float du1 = n1.GetDotProduct(v11) + d1;
	float du2 = n1.GetDotProduct(v12) + d1;

#ifdef USE_EPSILON_TEST
	if(fabs(du0) < EPSILON) du0 = 0.0f;
	if(fabs(du1) < EPSILON) du1 = 0.0f;
	if(fabs(du2) < EPSILON) du2 = 0.0f;
#endif
		
	float du0du1 = du0 * du1;
	float du0du2 = du0 * du2;

	if(du0du1 > 0.0f && du0du2 > 0.0f)
		return false;

	e1 = v11 - v10;
	e2 = v12 - v10;
	Vector3 n2 = e1.GetCrossProduct(e2);
	float d2 = -n2.GetDotProduct(v10);

	float dv0 = n2.GetDotProduct(v00) + d2;
	float dv1 = n2.GetDotProduct(v01) + d2;
	float dv2 = n2.GetDotProduct(v02) + d2;

#ifdef USE_EPSILON_TEST
	if(fabs(dv0) < EPSILON) dv0 = 0.0f;
	if(fabs(dv1) < EPSILON) dv1 = 0.0f;
	if(fabs(dv2) < EPSILON) dv2 = 0.0f;
#endif

	float dv0dv1 = dv0 * dv1;
	float dv0dv2 = dv0 * dv2;

	if(dv0dv1 > 0.0f && dv0dv2 > 0.0f)
		return false;

	Vector3 d = n1.GetCrossProduct(n2);

	float max = fabs(d.x);
	int index = 0;
	float bb = fabs(d.y);
	float cc = fabs(d.z);
	if(bb > max)
	{
		max = bb;
		index = 1;
	}
	if(cc > max)
	{
		max = cc;
		index = 2;
	}

	float vp0 = 0.0f;
	float vp1 = 0.0f;
	float vp2 = 0.0f;
	float up0 = 0.0f;
	float up1 = 0.0f;
	float up2 = 0.0f;

	if(index == 0)
	{
		vp0 = v00.x;
		vp1 = v01.x;
		vp2 = v02.x;

		up0 = v10.x;
		up1 = v11.x;
		up2 = v12.x;
	}
	else if(index == 1)
	{
		vp0 = v00.y;
		vp1 = v01.y;
		vp2 = v02.y;

		up0 = v10.y;
		up1 = v11.y;
		up2 = v12.y;
	}
	else
	{
		vp0 = v00.z;
		vp1 = v01.z;
		vp2 = v02.z;

		up0 = v10.z;
		up1 = v11.z;
		up2 = v12.z;
	}


	float a = 0.0f;
	float b = 0.0f;
	float c= 0.0f;
	float x0 = 0.0f;
	float x1 = 0.0f;

	if(NEWCOMPUTE_INTERVALS(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv2,a,b,c,x0,x1))
	{
		float N1[3];
		N1[0] = n1.x;
		N1[1] = n1.y;
		N1[2] = n1.z;

		float V00[3];
		V00[0] = v00.x;
		V00[1] = v00.y;
		V00[2] = v00.z;

		float V01[3];
		V01[0] = v01.x;
		V01[1] = v01.y;
		V01[2] = v01.z;

		float V02[3];
		V02[0] = v02.x;
		V02[1] = v02.y;
		V02[2] = v02.z;

		float V10[3];
		V10[0] = v10.x;
		V10[1] = v10.y;
		V10[2] = v10.z;

		float V11[3];
		V11[0] = v11.x;
		V11[1] = v11.y;
		V11[2] = v11.z;

		float V12[3];
		V12[0] = v12.x;
		V12[1] = v12.y;
		V12[2] = v12.z;

		return coplanar_tri_tri(N1, V00, V01, V02, V10, V11, V12) > 0;
	}

	float dsec = 0.0f;
	float e = 0.0f;
	float f = 0.0f;
	float y0 = 0.0f;
	float y1 = 0.0f;

	if(NEWCOMPUTE_INTERVALS(up0,up1,up2,du0,du1,du2,du0du1,du0du2,dsec,e,f,y0,y1))
	{
		float N1[3];
		N1[0] = n1.x;
		N1[1] = n1.y;
		N1[2] = n1.z;

		float V00[3];
		V00[0] = v00.x;
		V00[1] = v00.y;
		V00[2] = v00.z;

		float V01[3];
		V01[0] = v01.x;
		V01[1] = v01.y;
		V01[2] = v01.z;

		float V02[3];
		V02[0] = v02.x;
		V02[1] = v02.y;
		V02[2] = v02.z;

		float V10[3];
		V10[0] = v10.x;
		V10[1] = v10.y;
		V10[2] = v10.z;

		float V11[3];
		V11[0] = v11.x;
		V11[1] = v11.y;
		V11[2] = v11.z;

		float V12[3];
		V12[0] = v12.x;
		V12[1] = v12.y;
		V12[2] = v12.z;

		return coplanar_tri_tri(N1, V00, V01, V02, V10, V11, V12)>0;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	float isect1[2];
	float isect2[2];	

	float xx;
	float yy;
	float xxyy;
	float tmp;

	xx = x0 * x1;
	yy = y0 * y1;
	xxyy = xx * yy;

	tmp = a * xxyy;
	isect1[0] = tmp + b * x1 * yy;
	isect1[1] = tmp + c * x0 * yy;

	tmp = dsec * xxyy;
	isect2[0] = tmp + e * xx * y1;
	isect2[1] = tmp + f * xx * y0;

    
	if(isect1[0] > isect1[1])
	{
		float c; 
		c = isect1[0];     
		isect1[0] = isect1[1];     
		isect1[1] = c;     
	}

	if(isect2[0] > isect2[1])    
	{          
		float c; 
		c = isect2[0];     
		isect2[0] = isect2[1];     
		isect2[1] = c;     
	}

	if(isect1[1] < isect2[0] || isect2[1] < isect1[0]) 
		return false;



	tempCD.collision = true;
	//// ALL BELOW NEEDS TO BE ADDED, not sure if above code has that data somewhere or if additional
	//// calculations are needed to get it. posx, y, z should be the position of collision between
	//// the triangles, and distance should be the distance between Mesh1 (left parameter in above function)
	//// and the intersection point, so distance can't be calculated here is done above.
	tempCD.distance = 0.0f;
	tempCD.posx = 0.0f;
	tempCD.posy = 0.0f;
	tempCD.posz = 0.0f;

	return true;
}


// Original Content: http://www.chadvernon.com/blog/resources/directx9/frustum-culling/
bool PhysicsEngine::FrustrumVsSphere( D3DXPLANE planes[], BoundingSphere& bs, D3DXMATRIX& world, float& scale ) 
{
	// various distances
	float fDistance;
	D3DXVECTOR4 sPos;
	D3DXVec3Transform(&sPos, &bs.center, &world);
	D3DXVECTOR3 sPos3 = D3DXVECTOR3(sPos.x, sPos.y, sPos.z);
	float radius = bs.radius * scale;
	
	// calculate our distances to each of the planes
	for(int i = 0; i < 6; ++i) 
	{

		// find the distance to this plane
		//fDistance = D3DXVec3Dot(&D3DXVECTOR3(planes[i].a, planes[i].b, planes[i].c), &bs.Center) + m_planes[i].Distance;
		fDistance = D3DXPlaneDotCoord(&planes[i], &sPos3);// + m_planes->d;

		// if this distance is < -sphere.radius, we are outside
		if(fDistance < -radius)
			return false;

		/* Does Not Work Correctly - Alexivan
		// else if the distance is between +- radius, then we intersect
		if((float)fabs(fDistance) < radius)
			return true;
		*/
	}

	// otherwise we are fully in view
	return true;
}






//////////////////////////////////////////////////////////////////////////
//							Specials									//
//////////////////////////////////////////////////////////////////////////

bool FindXZInGrid(float &x, float &z, float deltax, float deltaz, Vector3 terrPos, float terrWidth)
{
	bool go = true;
	if(x >= terrPos.x && x < terrPos.x + terrWidth &&
		z >= terrPos.z && z < terrPos.z + terrWidth)
	{
		// Change the if and else, invert if, the if isnt needed, all I want is the else on it
		go = true;
	}
	else
	{
		if(deltax > 0.0f)
		{
			float mindistX = (terrPos.x - x) / deltax;
			if(mindistX > 0.0f)
			{
				// Target is "infront" of us, continue
				float zatmindistx = z + deltaz * mindistX;
				if(zatmindistx >= terrPos.z && zatmindistx < terrPos.z + terrWidth)
				{
					// found our start point
					x += deltax * mindistX;
					z = zatmindistx;
				}
				else
					go = false;
			}
			else
			{
				if(deltaz > 0.0f)
				{
					// test Z-wise
					float mindistZ = (terrPos.z - z) / deltaz;
					if(mindistZ > 0.0f)
					{
						// Target is "infront" of us, continue
						float xatmindistz = x + deltax * mindistZ;
						if(xatmindistz >= terrPos.x && xatmindistz < terrPos.x + terrWidth)
						{
							// found our start point
							z += deltaz * mindistZ;
							x = xatmindistz;
						}
						else
							go = false;
					}
				}
				else
				{
					float mindistZ = ((terrPos.z + terrWidth) - z) / deltaz;
					if(mindistZ > 0.0f)
					{
						// Target is "infront" of us, continue
						float xatmindistz = x + deltax * mindistZ;
						if(xatmindistz >= terrPos.x && xatmindistz < terrPos.x + terrWidth)
						{
							// found our start point
							z += deltaz * mindistZ;
							x = xatmindistz;
						}
						else
							go = false;
					}
				}
			}
		}
		else
		{
			float mindistX = ((terrPos.x + terrWidth) - x) / deltax;
			if(mindistX > 0.0f)
			{
				// Target is "infront" of us, continue
				float zatmindistx = z + deltaz * mindistX;
				if(zatmindistx >= terrPos.z && zatmindistx < terrPos.z + terrWidth)
				{
					// found our start point
					x += deltax * mindistX;
					z = zatmindistx;
				}
				else
					go = false;
			}
			else
			{
				// test Z-wise
				float mindistZ = ((terrPos.z + terrWidth) - z) / deltaz;
				if(mindistZ > 0.0f)
				{
					// Target is "infront" of us, continue
					float xatmindistz = x + deltax * mindistZ;
					if(xatmindistz >= terrPos.x && xatmindistz < terrPos.x + terrWidth)
					{
						// found our start point
						z += deltaz * mindistZ;
						x = xatmindistz;
					}
					else
						go = false;
				}
			}
		}
	}
	return go;
}

CollisionData PhysicsEngine::GetSpecialCollisionRayTerrain( Vector3 rayOrigin, Vector3 rayDirection, iTerrain* iterr, float distanceBetweenVerticies )
{
	CollisionData cd;
	if(Terrain* terrain = dynamic_cast<Terrain*>(iterr))
	{
		float scale = max(terrain->GetScale().x, max(terrain->GetScale().y, terrain->GetScale().z));
		if(this->DoCollisionSphereVsRay(terrain->GetBoundingSphere(), terrain->GetWorldMatrix(), scale, 
			rayOrigin, rayDirection))
		{
			int vertCount = 0;
			if(!this->tempVerts)
			{
				tempVerts = new Vertex[terrain->GetNrOfIndices()];	// bigger is needed for some reason.
				this->tempVertsSize = sqrtf(terrain->GetNrOfVertices());
			}

			cd.BoundingSphereCollision = true;
			float terrWidth = (this->tempVertsSize - 1) * distanceBetweenVerticies;
			Vector3 terrPos = iterr->GetPosition();
			terrPos.x -= terrWidth * 0.5f;
			terrPos.z -= terrWidth * 0.5f;

			// Bresenham's line algorithm
			bool go = true;
			float deltax = rayDirection.x;
			float deltaz = rayDirection.z;
			float x = rayOrigin.x;
			float z = rayOrigin.z;

			go = FindXZInGrid(x, z, deltax, deltaz, terrPos, terrWidth);

			if(go)	// ray is within terrain and we found our start pos
			{
				Vertex* terrVerts = terrain->GetVerticesPointer();


				// Need to do special cases depending on if X or Z delta is bigger?
				// Because of z "skipping" several spaces when delta x is like 0.2 and deltaz 0.9 = stepZ at 5'ish?
				if(abs(deltax) > abs(deltaz))
				{
					float zStep = (deltaz / deltax) * distanceBetweenVerticies;

					// x = x start of ray in grid
					// y = y start of ray in grid
					// deltax = rayDir.x
					// deltay = rayDir.y

					if(deltax > 0.0f)
					{
						int arrX = (x - terrPos.x) / distanceBetweenVerticies;
						int arrZ = (z - terrPos.z) / distanceBetweenVerticies;
						int rows = this->tempVertsSize;
					
						for(; x < terrPos.x + terrWidth && 
							z >= terrPos.z && z < terrPos.z + terrWidth; 
							x += distanceBetweenVerticies)
						{
							arrX = (x - terrPos.x) / distanceBetweenVerticies;
							arrZ = (z - terrPos.z) / distanceBetweenVerticies;

							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX + 1];

							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX + 1];
							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX + 1];		
						

						
							z += zStep;

							int oldZ = arrZ;	// Check if several grids in Z is hit.
							arrZ = (z - terrPos.z) / distanceBetweenVerticies;
							while(oldZ != arrZ && z >= terrPos.z && z < terrPos.z + terrWidth)
							{
								if(deltaz > 0.0f)
									oldZ++;
								else
									oldZ--;

								tempVerts[vertCount++] = terrVerts[oldZ * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldZ + 1) * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldZ + 1) * rows + arrX + 1];

								tempVerts[vertCount++] = terrVerts[oldZ * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldZ + 1) * rows + arrX + 1];
								tempVerts[vertCount++] = terrVerts[oldZ * rows + arrX + 1];	
							}
						}
					}
					else
					{
						int arrX = (x - terrPos.x) / distanceBetweenVerticies;
						int arrZ = (z - terrPos.z) / distanceBetweenVerticies;
						int rows = this->tempVertsSize;


						for(; x >= terrPos.x && 
							z >= terrPos.z && z < terrPos.z + terrWidth - distanceBetweenVerticies; 
							x -= distanceBetweenVerticies)
						{
							arrX = (x - terrPos.x) / distanceBetweenVerticies;
							arrZ = (z - terrPos.z) / distanceBetweenVerticies;

							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX + 1];

							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX + 1];
							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX + 1];		



							z -= zStep;

							int oldZ = arrZ;	// Check if several grids in Z is hit.
							arrZ = (z - terrPos.z) / distanceBetweenVerticies;
							while(oldZ != arrZ && z >= terrPos.z && z < terrPos.z + terrWidth)
							{
								if(deltaz > 0.0f)
									oldZ++;
								else
									oldZ--;

								tempVerts[vertCount++] = terrVerts[oldZ * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldZ + 1) * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldZ + 1) * rows + arrX + 1];

								tempVerts[vertCount++] = terrVerts[oldZ * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldZ + 1) * rows + arrX + 1];
								tempVerts[vertCount++] = terrVerts[oldZ * rows + arrX + 1];	
							}
						}
					}
				}
				else
				{
					float xStep = (deltax / deltaz) * distanceBetweenVerticies;

					if(deltaz > 0.0f)
					{
						int arrX = (x - terrPos.x) / distanceBetweenVerticies;
						int arrZ = (z - terrPos.z) / distanceBetweenVerticies;
						int rows = this->tempVertsSize;

						for(; z < terrPos.z + terrWidth && 
							x >= terrPos.z && x < terrPos.x + terrWidth; 
							z += distanceBetweenVerticies)
						{
							arrX = (x - terrPos.x) / distanceBetweenVerticies;
							arrZ = (z - terrPos.z) / distanceBetweenVerticies;

							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX + 1];

							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX + 1];
							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX + 1];		



							x += xStep;

							int oldX = arrX;	// Check if several grids in X is hit.
							arrX = (x - terrPos.x) / distanceBetweenVerticies;
							while(oldX != arrX && x >= terrPos.x && x < terrPos.x + terrWidth)
							{
								if(deltax > 0.0f)
									oldX++;
								else
									oldX--;

								tempVerts[vertCount++] = terrVerts[oldX * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldX + 1) * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldX + 1) * rows + arrX + 1];

								tempVerts[vertCount++] = terrVerts[oldX * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldX + 1) * rows + arrX + 1];
								tempVerts[vertCount++] = terrVerts[oldX * rows + arrX + 1];	
							}
						}
					}
					else
					{
						int arrX = (x - terrPos.x) / distanceBetweenVerticies;
						int arrZ = (z - terrPos.z) / distanceBetweenVerticies;
						int rows = this->tempVertsSize;

						for(; z >= terrPos.z && 
							x >= terrPos.x && x < terrPos.x + terrWidth - distanceBetweenVerticies; 
							z -= distanceBetweenVerticies)
						{
							arrX = (x - terrPos.x) / distanceBetweenVerticies;
							arrZ = (z - terrPos.z) / distanceBetweenVerticies;

							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX + 1];

							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX];
							tempVerts[vertCount++] = terrVerts[(arrZ + 1) * rows + arrX + 1];
							tempVerts[vertCount++] = terrVerts[arrZ * rows + arrX + 1];		


							x -= xStep;

							int oldX = arrX;	// Check if several grids in X is hit.
							arrX = (x - terrPos.x) / distanceBetweenVerticies;
							while(oldX != arrX && x >= terrPos.x && x < terrPos.x + terrWidth)
							{
								if(deltax > 0.0f)
									oldX++;
								else
									oldX--;

								tempVerts[vertCount++] = terrVerts[oldX * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldX + 1) * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldX + 1) * rows + arrX + 1];

								tempVerts[vertCount++] = terrVerts[oldX * rows + arrX];
								tempVerts[vertCount++] = terrVerts[(oldX + 1) * rows + arrX + 1];
								tempVerts[vertCount++] = terrVerts[oldX * rows + arrX + 1];	
							}
						}
					}

				}
				
				this->DoSpecialCollisionRayVsTerrainTriangles(rayOrigin, rayDirection, 
					tempVerts, vertCount, NULL, 0, terrain->GetWorldMatrix(), cd);
			}
			if(!cd.collision)
				this->DoCollisionRayVsTriangles(rayOrigin, rayDirection, terrain->GetVerticesPointer(), terrain->GetNrOfVertices(), 
				terrain->GetIndicesPointer(), terrain->GetNrOfIndices(), terrain->GetWorldMatrix(), cd);
		}
	}
	//else
		//MaloW::Debug("Failed to cast iTerrain to Terrain in PhysicsEngine.cpp in RayTerrain");
		// MALOW, TILLMAN, ANYONE!!1!!! Commented this out because Björn lagged when using editor cuz this was
		// spamming out. Alex: Needs to check how he uses this in the editor, prolly sends a bad pointer.

	return cd;
}

void PhysicsEngine::DoSpecialCollisionRayVsTerrainTriangles( Vector3 rayOrigin, Vector3 rayDirection, Vertex* vertices, 
															int nrOfVertices, int* indices, int nrOfIndices, 
															D3DXMATRIX worldMat, CollisionData& cd)
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
				if(tempCD.distance > 0.0f)
				{
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
					}
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
				if(tempCD.distance > 0.0f)
				{
					if(tempCD.distance < cd.distance)
					{
						cd.distance = tempCD.distance;
						cd.posx = tempCD.posx;
						cd.posy = tempCD.posy;
						cd.posz = tempCD.posz;
						cd.collision = true;
					}
				}
			}
		}
	}
}

void PhysicsEngine::DoCollisionRayVsFBXMesh( Vector3 rayOrigin, Vector3 rayDirection, FBXMesh* mesh, CollisionData& cd )
{
	BTHFBX_RAY_BOX_RESULT res = mesh->RayVsScene(rayOrigin, rayDirection);
	if(res.DistanceToHit > 0.0f)
	{
		cd.distance = res.DistanceToHit;
		cd.collision = true;
		cd.BoundingSphereCollision = true;
		Vector3 colPos = rayOrigin + rayDirection * cd.distance;
		cd.posx = colPos.x;
		cd.posy = colPos.y;
		cd.posz = colPos.z;
	}
}

