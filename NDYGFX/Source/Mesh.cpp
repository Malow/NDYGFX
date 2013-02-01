#include "Mesh.h"	

Mesh::Mesh(D3DXVECTOR3 pos)
{
	this->filePath = "";
	this->specialColor = NULL_COLOR;
	this->usingInvisibilityEffect = false;
	//this->transparency = 0.0f;

	this->strips = new MaloW::Array<MeshStrip*>();

	this->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	this->pos = pos;

	this->rotQuat = D3DXQUATERNION(0, 0, 0, 1);
	this->scale = D3DXVECTOR3(1, 1, 1);
}

Mesh::~Mesh() 
{
	if(this->strips)
	{
		while(this->strips->size() > 0)
			delete this->strips->getAndRemove(0);

		delete this->strips;
	}
}

bool Mesh::LoadFromFile(string file)
{
	this->filePath = file;

	ObjData* od = GetResourceManager()->LoadObjectDataResourceFromFile(this->filePath.c_str())->GetObjectDataPointer();
	
	try
	{
		if(od)
		{
			MaloW::Array<MaterialData>* mats = od->mats;
			for(int q = 0; q < mats->size(); q++)
			{
				bool hasFace = false;
				MeshStrip* strip = new MeshStrip();


				/////// For hit/bounding boxes
				D3DXVECTOR3 min = D3DXVECTOR3(99999.9f, 99999.9f, 99999.9f);
				D3DXVECTOR3 max = -min;
			

				int nrOfVerts = 0;
			
				Vertex* tempverts = new Vertex[od->faces->size()*3];
		
				for(int i = 0;  i < od->faces->size(); i++)
				{
					if(od->faces->get(i).material == mats->get(q).name)
					{
						int vertpos = od->faces->get(i).data[0][0] - 1;
						int textcoord = od->faces->get(i).data[0][1] - 1;
						int norm = od->faces->get(i).data[0][2] - 1;
						tempverts[nrOfVerts] = Vertex(od->vertspos->get(vertpos), od->textcoords->get(textcoord), od->vertsnorms->get(norm));
						DoMinMax(min, max, tempverts[nrOfVerts].pos);
						nrOfVerts++;

						vertpos = od->faces->get(i).data[2][0] - 1;
						textcoord = od->faces->get(i).data[2][1] - 1;
						norm = od->faces->get(i).data[2][2] - 1;
						tempverts[nrOfVerts] = Vertex(od->vertspos->get(vertpos), od->textcoords->get(textcoord), od->vertsnorms->get(norm));
						DoMinMax(min, max, tempverts[nrOfVerts].pos);
						nrOfVerts++;

						vertpos = od->faces->get(i).data[1][0] - 1;
						textcoord = od->faces->get(i).data[1][1] - 1;
						norm = od->faces->get(i).data[1][2] - 1;
						tempverts[nrOfVerts] = Vertex(od->vertspos->get(vertpos), od->textcoords->get(textcoord), od->vertsnorms->get(norm));
						DoMinMax(min, max, tempverts[nrOfVerts].pos);
						nrOfVerts++;

						hasFace = true;
					}
				}

				if(!hasFace)
				{
					delete tempverts;
					delete strip;
				}
				else
				{
					strip->setNrOfVerts(nrOfVerts);
					Vertex* verts = new Vertex[nrOfVerts];
					for(int z = 0; z < nrOfVerts; z++)
					{
						verts[z] = tempverts[z];
					}
					delete tempverts;
					strip->SetVerts(verts);

					strip->SetTexturePath(od->mats->get(q).texture);

					Material* mat = new Material();
					mat->AmbientColor = od->mats->get(q).ka;
					if(mat->AmbientColor == D3DXVECTOR3(0.0f, 0.0f, 0.0f))				//////////// MaloW Fix, otherwise completely black with most objs
						mat->AmbientColor += D3DXVECTOR3(0.2f, 0.2f, 0.2f);			//////////// MaloW Fix, otherwise completely black with most objs

					mat->DiffuseColor = od->mats->get(q).kd;
					if(mat->DiffuseColor == D3DXVECTOR3(0.0f, 0.0f, 0.0f))				//////////// MaloW Fix, otherwise completely black with most objs
						mat->DiffuseColor += D3DXVECTOR3(0.6f, 0.6f, 0.6f);			//////////// MaloW Fix, otherwise completely black with most objs

					mat->SpecularColor = od->mats->get(q).ks;
					strip->SetMaterial(mat);

					strip->SetBoundingSphere(BoundingSphere(min, max));

					this->strips->add(strip);
				}
			}
			this->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			//delete od;

			return true;
		}
	}
	catch (...)
	{
		MaloW::Debug("Warning: Mesh: LoadFromFile(): Failed to set/copy loaded object data.");
	}
			
	return false;
}

void Mesh::SetSpecialColor(COLOR specialColor)
{
	this->specialColor = specialColor;
}

void Mesh::UseInvisibilityEffect(bool use)
{
	this->usingInvisibilityEffect = use;
}

void Mesh::SetPosition(D3DXVECTOR3 pos)
{ 
	this->pos = pos;
	this->RecreateWorldMatrix();
}

void Mesh::SetPosition( const Vector3& pos )
{
	this->SetPosition( D3DXVECTOR3(pos.x,pos.y,pos.z) );
}

void Mesh::SetQuaternion(D3DXQUATERNION quat)
{
	this->rotQuat = quat;
	this->RecreateWorldMatrix();
}

void Mesh::SetQuaternion( const Vector4& quat )
{
	this->SetQuaternion( D3DXQUATERNION(quat.x,quat.y,quat.z,quat.w) );
}

void Mesh::MoveBy(D3DXVECTOR3 moveby)
{ 
	this->pos += moveby; 
	this->RecreateWorldMatrix();
}

void Mesh::MoveBy( const Vector3& moveby )
{
	this->MoveBy( D3DXVECTOR3(moveby.x,moveby.y,moveby.z) );
}

void Mesh::Rotate(D3DXVECTOR3 radians)
{
	D3DXQUATERNION quaternion;
	D3DXQuaternionRotationYawPitchRoll(&quaternion, radians.y, radians.x, radians.z);
	D3DXQuaternionMultiply(&this->rotQuat, &this->rotQuat, &quaternion);
	this->RecreateWorldMatrix();
}

void Mesh::Rotate( const Vector3& radians )
{
	this->Rotate( D3DXVECTOR3(radians.x,radians.y,radians.z) );
}

void Mesh::Rotate(D3DXQUATERNION quat)
{
	D3DXQuaternionMultiply(&this->rotQuat, &this->rotQuat, &quat);
	this->RecreateWorldMatrix();
}

void Mesh::Rotate( const Vector4& quat )
{
	this->Rotate( D3DXQUATERNION(quat.x,quat.y,quat.z,quat.w) );
}

void Mesh::RotateAxis(D3DXVECTOR3 around, float angle)
{
	D3DXQUATERNION quaternion = D3DXQUATERNION(0, 0, 0, 1);
	D3DXQuaternionRotationAxis(&quaternion, &around, angle);
	
	D3DXQuaternionMultiply(&this->rotQuat, &this->rotQuat, &quaternion);
	this->RecreateWorldMatrix();
}

void Mesh::RotateAxis( const Vector3& around, float angle )
{
	this->RotateAxis( D3DXVECTOR3(around.x,around.y,around.z), angle );
}

void Mesh::Scale(D3DXVECTOR3 scale)
{
	this->scale.x *= scale.x;
	this->scale.y *= scale.y;
	this->scale.z *= scale.z;
	this->RecreateWorldMatrix();
}

void Mesh::Scale( const Vector3& scale )
{
	this->Scale( D3DXVECTOR3(scale.x,scale.y,scale.z) );
}

void Mesh::Scale(float scale)
{
	this->scale.x *= scale;
	this->scale.y *= scale;
	this->scale.z *= scale;
	this->RecreateWorldMatrix();
}

void Mesh::RecreateWorldMatrix()
{
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, this->pos.x, this->pos.y, this->pos.z);

	D3DXMATRIX scaling;
	D3DXMatrixScaling(&scaling, this->scale.x, this->scale.y, this->scale.z);

	/*
	// Euler
	D3DXMATRIX x, y, z;
	D3DXMatrixRotationX(&x, this->rot.x);
	D3DXMatrixRotationY(&y, this->rot.y);
	D3DXMatrixRotationZ(&z, this->rot.z);

	D3DXMATRIX world = scaling*x*y*z*translate;
	*/

	// Quaternions
	D3DXMATRIX QuatMat;
	D3DXMatrixRotationQuaternion(&QuatMat, &this->rotQuat); 

	D3DXMATRIX world = scaling*QuatMat*translate;




	this->worldMatrix = world;
}

void Mesh::ResetRotationAndScale()
{
	this->rotQuat = D3DXQUATERNION(0, 0, 0, 1);
	this->scale = D3DXVECTOR3(1, 1, 1);
	this->RecreateWorldMatrix();
}

Vector3 Mesh::GetPosition() const
{
	return Vector3(this->pos.x, this->pos.y, this->pos.z);
}

Vector4 Mesh::GetRotationQuaternion() const
{
	return Vector4(this->rotQuat.x, this->rotQuat.y, this->rotQuat.z, this->rotQuat.w);
}

Vector3 Mesh::GetScaling() const
{
	return Vector3(this->scale.x, this->scale.y, this->scale.z);
}

void Mesh::SetScale( float scale )
{
	this->scale.x = scale;
	this->scale.y = scale;
	this->scale.z = scale;
	this->RecreateWorldMatrix();
}

void Mesh::SetScale( Vector3 scale )
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;
	this->RecreateWorldMatrix();
}

void Mesh::SetScale(D3DXVECTOR3 scale)
{
	this->SetScale(Vector3(scale.x, scale.y, scale.z));
}

void Mesh::ResetRotation()
{
	this->rotQuat = D3DXQUATERNION(0, 0, 0, 1);
}



//**TILLMAN**
/*
	//**tillman
	/*if(this->strips)
	{
		while(this->strips->size() > 0)
			delete this->strips->getAndRemove(0);

		delete this->strips;
	}
	GetResourceManager()->DeleteMesh(this->strips);
	//Create mesh
	this->strips = GetResourceManager()->CreateMeshFromFile(file.c_str());
	//Set topology
	this->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if(this->strips != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
*/