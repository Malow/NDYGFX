#pragma once

#include "Array.h"
#include "Mesh.h"
#include "iFBXMesh.h"

// FBX
#include "BTHFBX\Shared\FBX\FBXSceneD3D.h"

#ifdef _DEBUG
#pragma comment(lib, "BTHFbxD.lib")
#else
#pragma comment(lib, "BTHFbx.lib")
#endif







// FBX

class FBXMesh : public Mesh, public virtual iFBXMesh
{
private:
	FBXSceneD3D* scene;


public:
	FBXMesh(D3DXVECTOR3 pos);
	virtual ~FBXMesh();

	virtual MaloW::Array<MeshStrip*>* GetStrips();

	virtual bool LoadFromFile(string file, IBTHFbx* fbx );

	void Update(float dt);
	void Render(float dt, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* shad, ID3D11DeviceContext* devCont);


	//BTHFBX_RAY_BOX_RESULT RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix);
};

