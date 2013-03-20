#include "Mesh.h"	

Mesh::Mesh(D3DXVECTOR3 pos, string filePath, string billboardFilePath, float distanceToSwapToBillboard)
{
	this->filePath = filePath;
	this->specialColor = NULL_COLOR;
	this->usingInvisibilityEffect = false;
	//this->transparency = 0.0f;

	this->meshStripsResource = NULL;
	this->isStripCulled = NULL;
	this->isStripShadowCulled = NULL;

	this->dontRender = false;

	this->height = -1.0f;
	this->billboardFilePath = billboardFilePath;
	if(billboardFilePath != "")
	{
		this->hasBillboard = true;
	}
	else
	{
		this->hasBillboard = false;
	}
	this->billboard = NULL;
	this->distanceToSwapToBillboard = distanceToSwapToBillboard;

	this->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	this->pos = pos;

	this->rotQuat = D3DXQUATERNION(0, 0, 0, 1);
	this->scale = D3DXVECTOR3(1, 1, 1);
	this->RecreateWorldMatrix();
}
Mesh::~Mesh() 
{
	GetResourceManager()->DeleteMeshStripsResource(this->meshStripsResource);
	if(this->isStripCulled) delete[] this->isStripCulled; this->isStripCulled = NULL;
	if(this->isStripShadowCulled) delete[] this->isStripShadowCulled; this->isStripShadowCulled = NULL;
	if(this->billboard) delete this->billboard; this->billboard = NULL;
}

bool Mesh::LoadFromFile(string file)
{
	this->filePath = file;

	this->meshStripsResource = GetResourceManager()->CreateMeshStripsResourceFromFile(file.c_str(), this->height);
	if(this->meshStripsResource != NULL)
	{
		//Create arrays for culling
		this->isStripCulled = new bool[this->meshStripsResource->GetMeshStripsPointer()->size()];
		this->isStripShadowCulled = new bool[this->meshStripsResource->GetMeshStripsPointer()->size()];
		for(unsigned int i = 0; i < this->meshStripsResource->GetMeshStripsPointer()->size(); i++)
		{
			this->isStripCulled[i] = false;
			this->isStripShadowCulled[i] = false;
		}

		this->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		//Create billboard.
		//Calculate billboard position(this needs to be updated as the mesh position changes).(don't forget to include the scale).
		float halfHeightScaled = this->height * 0.5f * this->scale.y; //(yOffset)
		D3DXVECTOR3 billboardPos = this->pos;
		billboardPos.y += halfHeightScaled;
		//Calculate the size using Pythagoras theorem (don't forget to include the scale).
		//Note that this returns the half of the half size, so multiply by 4.
		float size = sqrtf(powf(halfHeightScaled, 2.0f) * 0.5f) * 4.0f;
		this->billboard = new Billboard(billboardPos, D3DXVECTOR2(size, size), D3DXVECTOR3(1.0f, 1.0f, 1.0f));

		return true;
	}
	else
	{
		return false;
	}
}

void Mesh::SetSpecialColor(COLOR specialColor)
{
	this->specialColor = specialColor;
}

void Mesh::UseInvisibilityEffect(bool flag)
{
	this->usingInvisibilityEffect = flag;
}

void Mesh::SetPosition(D3DXVECTOR3 pos)
{ 
	this->pos = pos;
	this->RecreateWorldMatrix();
	this->RecreateBillboardData();
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
void Mesh::RecreateBillboardData()
{
	if(this->billboard != NULL)
	{
		//Calculate billboard position.(don't forget to include the scale).
		float halfHeightScaled = this->height * 0.5f * this->scale.y; //(yOffset)
		D3DXVECTOR3 billboardPos = this->pos;
		billboardPos.y += halfHeightScaled;
		this->billboard->SetPosition(billboardPos);
		//Calculate the size using Pythagoras theorem (don't forget to include the scale).
		//Note that this returns the half of the half size, so multiply by 4.
		float size = sqrtf(powf(halfHeightScaled, 2.0f) * 0.5f) * 4.0f;
		this->billboard->SetSize(D3DXVECTOR2(size, size));
	}
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
	this->RecreateBillboardData();
}

void Mesh::SetScale( Vector3 scale )
{
	this->scale.x = scale.x;
	this->scale.y = scale.y;
	this->scale.z = scale.z;
	this->RecreateWorldMatrix();
	this->RecreateBillboardData();
}

void Mesh::SetScale(D3DXVECTOR3 scale)
{
	this->SetScale(Vector3(scale.x, scale.y, scale.z));
}

void Mesh::ResetRotation()
{
	this->rotQuat = D3DXQUATERNION(0, 0, 0, 1);
}

void Mesh::RotateVectorByMeshesRotation( Vector3& vec )
{
	D3DXMATRIX QuatMat;
	D3DXMatrixRotationQuaternion(&QuatMat, &this->rotQuat); 
	D3DXVECTOR4 o;
	D3DXVECTOR3 i = D3DXVECTOR3(vec.x, vec.y, vec.z);
	D3DXVec3Transform(&o, &i, &QuatMat);
	vec.x = o.x;
	vec.y = o.y;
	vec.z = o.z;
}
