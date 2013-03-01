#pragma once

#include "Array.h"
#include "Mesh.h"
#include "iFBXMesh.h"
#include <mutex>

// FBX
#include "BTHFBX\Shared\FBX\FBXSceneD3D.h"

#ifdef _DEBUG
#pragma comment(lib, "BTHFbxD.lib")
#else
#pragma comment(lib, "BTHFbx.lib")
#endif

#pragma warning ( push )
#pragma warning ( disable : 4250 ) //  warning C4250: 'FBXMesh' : inherits via dominance

class FBXMesh : public Mesh, public virtual iFBXMesh
{
private:
	std::mutex zSceneMutex;
	FBXSceneD3D* zScene;

	std::map< iMesh*, std::string > zBoundMeshes;

	bool culled;
	BoundingSphere bs;

public:
	FBXMesh(D3DXVECTOR3 pos, string filePath);
	virtual ~FBXMesh();

	virtual MaloW::Array<MeshStrip*>* GetStrips();

	virtual bool LoadFromFile(string file, IBTHFbx* fbx, ID3D11Device* dev, ID3D11DeviceContext* devCont );

	void Update(float dt);
	void Render(float dt, D3DXMATRIX camProj, D3DXMATRIX camView, Shader* shad, ID3D11DeviceContext* devCont);
	void RenderShadow(float dt, D3DXMATRIX lightViewProj, Shader* shad, ID3D11DeviceContext* devCont);

	virtual bool SetAnimation(unsigned int ani);
	virtual bool SetAnimation(const char* name);
	bool GetBonePosition(const std::string& name, float& x, float& y, float& z);

	// Mesh Bounds
	virtual bool BindMesh(const char* boneName, iMesh* mesh);
	virtual void UnbindMesh(iMesh* mesh);

	void SetCulled(bool cull) { this->culled = cull; }
	bool IsCulled() const { return this->culled; }

	BoundingSphere GetBoundingSphere() const { return this->bs; }

	//BTHFBX_RAY_BOX_RESULT RayVsScene(const BTHFBX_RAY& ray, BTHFBX_MATRIX* worldMatrix);
};

#pragma warning ( pop )