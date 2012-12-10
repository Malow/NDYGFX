#ifndef MESH_H
#define MESH_H

#include "Buffer.h"
#include "Vertex.h"
#include "ObjLoader.h"
#include "Object3D.h"
#include "Material.h"
#include "MeshStrip.h"
#include "iMesh.h"
#include "Vector.h"

/*
Mesh class that represents a 3d-object on the screen
*/



class Mesh : public virtual iMesh
{
protected:
	COLOR specialColor;
	bool usingInvisibilityEffect;
	//float transparency; //default 0.0f (full visibility)

	MaloW::Array<MeshStrip*>* strips;

	D3D_PRIMITIVE_TOPOLOGY topology;

	D3DXVECTOR3 pos;
	D3DXQUATERNION rotQuat;
	D3DXVECTOR3 scale;
	D3DXMATRIX worldMatrix;
	
public:
	Mesh(D3DXVECTOR3 pos);
	virtual ~Mesh();
	
	// Mesh Color Overlay
	virtual void SetSpecialColor(COLOR specialColor);
	virtual COLOR GetSpecialColor() const { return this->specialColor; }

	// Invisibility
	void UseInvisibilityEffect(bool use);
	bool IsUsingInvisibility() const { return this->usingInvisibilityEffect; }

	// Transformation
	void SetPosition(D3DXVECTOR3 pos);
	void SetQuaternion(D3DXQUATERNION quat);
	void MoveBy(D3DXVECTOR3 moveby);
	void Rotate(D3DXVECTOR3 radians);
	void Rotate(D3DXQUATERNION quat);
	void RotateAxis(D3DXVECTOR3 around, float angle);
	void Scale(D3DXVECTOR3 scale);

	// iMesh Interface Functions
	virtual void RotateAxis(const Vector3& around, float angle);
	virtual void Rotate(const Vector4& quat);
	virtual void SetPosition(const Vector3& pos);
	virtual void SetQuaternion(const Vector4& quat);
	virtual void MoveBy(const Vector3& moveby);
	virtual void Rotate(const Vector3& radians);
	virtual void Scale(const Vector3& scale);
	virtual void Scale(float scale);
	virtual Vector3 GetPosition() const;
	virtual Vector4 GetRotationQuaternion() const;
	virtual Vector3 GetScaling() const;

	
	D3DXMATRIX GetWorldMatrix() { return this->worldMatrix; }
	void ResetRotationAndScale();

	// Is used internally when needed, but can be used from the outside for debugging.
	void RecreateWorldMatrix();

	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->topology; }
	
	// Load Mesh From File
	virtual void LoadFromFile(string file);

	// Get Strips
	virtual MaloW::Array<MeshStrip*>* GetStrips() = 0;
};

class ParticleMesh
{
protected:
	Object3D* RenderObject;
	ParticleVertex* mesh;
	int nrOfVerts;
	string texture;
	D3D_PRIMITIVE_TOPOLOGY topology;

	D3DXVECTOR3 pos;
	D3DXMATRIX worldMatrix;

public:
	ParticleMesh(D3DXVECTOR3 pos)
	{
		this->RenderObject = NULL;
		this->mesh = NULL;
		this->nrOfVerts = 0;
		this->texture = "";
		this->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		this->pos = pos;
	}
	virtual ~ParticleMesh() 
	{
		if(this->mesh)
			delete [] this->mesh;
		if(this->RenderObject)
			delete this->RenderObject;
	}

	ParticleVertex* getVerts() const { return this->mesh; }
	int getNrOfVerts() const { return this->nrOfVerts; }
	string GetTexturePath() const { return this->texture; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->topology; }

	Object3D* GetRenderObject() const { return this->RenderObject; }
	void SetRenderObject(Object3D* ro) { this->RenderObject = ro; }

	D3DXVECTOR3 GetPosition() const { return this->pos; }

	void RecreateWorldMatrix()
	{
		D3DXMATRIX translate;
		D3DXMatrixTranslation(&translate, this->pos.x, this->pos.y, this->pos.z);

		this->worldMatrix = translate;
	}

	D3DXMATRIX GetWorldMatrix() { return this->worldMatrix; }
};


#endif
#ifndef MESH_H
#define MESH_H

#include "Buffer.h"
#include "Vertex.h"
#include "ObjLoader.h"
#include "Object3D.h"
#include "Material.h"
#include "MeshStrip.h"
#include "iMesh.h"
#include "Vector.h"

/*
Mesh class that represents a 3d-object on the screen
*/



class Mesh : public iMesh
{
protected:
	COLOR specialColor;
	bool usingInvisibilityEffect;
	//float transparency; //default 0.0f (full visibility)

	MaloW::Array<MeshStrip*>* strips;

	D3D_PRIMITIVE_TOPOLOGY topology;

	D3DXVECTOR3 pos;
	D3DXQUATERNION rotQuat;
	D3DXVECTOR3 scale;
	D3DXMATRIX worldMatrix;
	
public:
	Mesh(D3DXVECTOR3 pos);
	virtual ~Mesh();
	
	/*! Sets mesh's special color. */
	virtual void SetSpecialColor(COLOR specialColor);
	
	// Transformation
	void SetPosition(D3DXVECTOR3 pos);
	void SetQuaternion(D3DXQUATERNION quat);
	void MoveBy(D3DXVECTOR3 moveby);
	void Rotate(D3DXVECTOR3 radians);
	void Rotate(D3DXQUATERNION quat);
	void RotateAxis(D3DXVECTOR3 around, float angle);
	void Scale(D3DXVECTOR3 scale);
	virtual void Scale(float scale);
	D3DXVECTOR3 getPosition() const { return pos; }
	D3DXVECTOR3 getScaling() const { return scale; }
	D3DXQUATERNION getRotation() const { return rotQuat; }

	// iMesh Transformation Wrapping
	virtual void SetPosition(const Vector3& pos);
	virtual void SetQuaternion(const Vector4& quat);
	virtual void MoveBy(const Vector3& moveby);
	virtual void Rotate(const Vector3& radians);
	virtual void Rotate(const Vector4& quat);
	virtual void RotateAxis(const Vector3& around, float angle);
	virtual void Scale(const Vector3& scale);
	virtual Vector3 GetPosition() const;
	virtual Vector3 GetRotation() const;
	virtual Vector3 GetScaling() const;

	// Load From File
	virtual void LoadFromFile(string file);

	// Special Color
	COLOR GetSpecialColor() const { return this->specialColor; }	

	// Invisibility
	void UseInvisibilityEffect(bool use);
	bool IsUsingInvisibility() const { return this->usingInvisibilityEffect; }

	D3DXMATRIX GetWorldMatrix() { return this->worldMatrix; }
	void ResetRotationAndScale();
	/* ! Is used internally when needed, but can be used from the outside for debugging. */
	void RecreateWorldMatrix();

	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->topology; }

	virtual MaloW::Array<MeshStrip*>* GetStrips() = 0;
};

class ParticleMesh
{
protected:
	Object3D* RenderObject;
	ParticleVertex* mesh;
	int nrOfVerts;
	string texture;
	D3D_PRIMITIVE_TOPOLOGY topology;

	D3DXVECTOR3 pos;
	D3DXMATRIX worldMatrix;

public:
	ParticleMesh(D3DXVECTOR3 pos)
	{
		this->RenderObject = NULL;
		this->mesh = NULL;
		this->nrOfVerts = 0;
		this->texture = "";
		this->topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		this->pos = pos;
	}
	virtual ~ParticleMesh() 
	{
		if(this->mesh)
			delete [] this->mesh;
		if(this->RenderObject)
			delete this->RenderObject;
	}

	ParticleVertex* getVerts() const { return this->mesh; }
	int getNrOfVerts() const { return this->nrOfVerts; }
	string GetTexturePath() const { return this->texture; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->topology; }

	Object3D* GetRenderObject() const { return this->RenderObject; }
	void SetRenderObject(Object3D* ro) { this->RenderObject = ro; }

	D3DXVECTOR3 GetPosition() const { return this->pos; }

	void RecreateWorldMatrix()
	{
		D3DXMATRIX translate;
		D3DXMatrixTranslation(&translate, this->pos.x, this->pos.y, this->pos.z);

		this->worldMatrix = translate;
	}

	D3DXMATRIX GetWorldMatrix() { return this->worldMatrix; }
};


#endif
