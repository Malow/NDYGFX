#pragma once

#include "iDecal.h"
#include "DirectX.h"
#include "ResourceManager.h"

class Decal : public virtual iDecal
{
private:
	D3DXMATRIX world;
	D3DXMATRIX matrix;
	Vector3 position;
	float size;
	float opa;
	Vector3 dir;
	Vector3 up;

	MeshStrip* strip;

public:
	Decal(Vector3 pos, Vector3 dir, Vector3 up);
	virtual ~Decal();

	virtual float GetOpacity() const { return this->opa; }
	virtual void SetOpacity(float opacity) { this->opa = opacity; }
	virtual float GetSize() const { return this->size; }
	virtual void SetSize(float size) { this->size = size; }
	virtual Vector3 GetPosition() const { return this->position; }
	virtual void SetPosition(Vector3 pos) { this->position = pos; }
	D3DXMATRIX GetMatrix() const;
	D3DXMATRIX GetWorldMatrix();
	void SetWorldMatrix(D3DXMATRIX mat);
	void SetMatrix(D3DXMATRIX mat) { this->matrix = mat; }
	virtual Vector3 GetDirection() { return this->dir; }
	virtual Vector3 GetUp() { return this->up; }
	MeshStrip* GetStrip() const { return this->strip; }

	// virtual void SetDirection / SetUp, if needed remember to rebuild matrixes after setting it
	// code for it is in dxmanager.cpp createdecals

	MeshStrip* GetStrip() { return this->strip; }
};