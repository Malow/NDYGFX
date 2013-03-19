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
#include "Billboard.h"

/*
Mesh class that represents a 3d-object on the screen
*/


inline void DoMinMax(D3DXVECTOR3& min, D3DXVECTOR3& max, D3DXVECTOR3 v)
{
	min.x = min(min.x, v.x);
	min.y = min(min.y, v.y);
	min.z = min(min.z, v.z);

	max.x = max(max.x, v.x);
	max.y = max(max.y, v.y);
	max.z = max(max.z, v.z);
}



class Mesh : public virtual iMesh
{
protected:
	string filePath; 
	COLOR specialColor;
	bool usingInvisibilityEffect;

	MeshStripsResource* meshStripsResource;
	bool* isStripCulled;
	bool* isStripShadowCulled;

	bool dontRender;

	float height;
	bool hasBillboard;
	string billboardFilePath;
	Billboard* billboard;
	float distanceToSwapToBillboard;

	D3D_PRIMITIVE_TOPOLOGY topology;

	D3DXVECTOR3 pos;
	D3DXQUATERNION rotQuat;
	D3DXVECTOR3 scale;
	D3DXMATRIX worldMatrix;
	
public:
	Mesh(D3DXVECTOR3 pos, string filePath, string billboardFilePath = "", float distanceToSwapToBillboard = -1);
	virtual ~Mesh();
	
	// ID
	virtual const char* GetFilePath() const { return this->filePath.c_str(); }

	// Culling
	bool IsStripCulled(unsigned int index) { return this->isStripCulled[index]; }
	bool IsStripShadowCulled(unsigned int index) { return this->isStripShadowCulled[index]; }
	void SetStripCulledFlag(unsigned int index, bool flag) { this->isStripCulled[index] = flag; }
	void SetStripShadowCulledFlag(unsigned index, bool flag) { this->isStripShadowCulled[index] = flag; }

	MeshStripsResource* GetMeshStripsResourcePointer() { return this->meshStripsResource; }

	// Billboard
	float GetHeight() { return this->height; }
	inline bool HasBillboard() { return this->hasBillboard; }
	inline string GetBillboardFilePath() { return this->billboardFilePath; }
	Billboard* GetBillboardGFX() { return this->billboard; }
	float GetDistanceToSwapToBillboard() { return this->distanceToSwapToBillboard; }

	// Mesh Color Overlay
	virtual void SetSpecialColor(COLOR specialColor);
	virtual COLOR GetSpecialColor() const { return this->specialColor; }

	
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
	virtual void SetScale(float scale);
	virtual void SetScale(Vector3 scale);
	virtual void SetScale(D3DXVECTOR3 scale);
	virtual Vector3 GetPosition() const;
	virtual Vector4 GetRotationQuaternion() const;
	virtual Vector3 GetScaling() const;
	virtual void ResetRotation();
	virtual iBillboard* GetBillboard() { return this->billboard; }
	virtual bool GetDontRenderFlag() { return this->dontRender; }
	virtual void DontRender(bool flag) { this->dontRender = flag; }
	// Invisibility
	virtual bool IsUsingInvisibility() const { return this->usingInvisibilityEffect; }
	virtual void UseInvisibilityEffect(bool flag);


	D3DXMATRIX& GetWorldMatrix() { return this->worldMatrix; }
	D3DXVECTOR3& GetScalingD3D() { return this->scale; }
	void ResetRotationAndScale();

	// Is used internally when needed, but can be used from the outside for debugging.
	void RecreateWorldMatrix();
	void RecreateBillboardData();

	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return this->topology; }
	
	// Load Mesh From File
	virtual bool LoadFromFile(string file);

	// Get Strips
	virtual MaloW::Array<MeshStrip*>* GetStrips() = 0;

	virtual void RotateVectorByMeshesRotation(Vector3& vec);
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