#pragma once

#include "Array.h"
#include "Mesh.h"
#include "iFBXMesh.h"
#include <mutex>
#include <chrono>
#include <set>

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
	std::set< std::string > zHiddenModels;

	bool culled;
	BoundingSphere bs;

	// Animation Queue
	std::vector< std::pair<std::string, float> > zAnimationQueue;
	unsigned int zLastAnimationIndex;
	std::chrono::system_clock::time_point zQueueStarted;

public:
	FBXMesh(D3DXVECTOR3 pos, string filePath);
	virtual ~FBXMesh();

	virtual MaloW::Array<MeshStrip*>* GetStrips();

	virtual bool LoadFromFile(string file, IBTHFbx* fbx, ID3D11Device* dev, ID3D11DeviceContext* devCont );

	void Update(float dt);
	void Render(float dt, D3DXMATRIX& camProj, D3DXMATRIX& camView, D3DXMATRIX& camViewProj, Shader* shad, ID3D11DeviceContext* devCont);
	void RenderShadow(float dt, D3DXMATRIX& lightViewProj, Shader* shad, ID3D11DeviceContext* devCont, D3DXVECTOR3 sunDir );

	// Sets the correct animation according to queue
	void UpdateAnimationQueue();

	virtual bool SetAnimation(unsigned int ani);
	virtual bool SetAnimation(const char* name);
	virtual void SetAnimationQueue( const char* const* names, const float* times, const unsigned int& count );

	// Hide models
	virtual void HideModel( const char* name, const bool& flag );
	virtual bool IsModelHidden( const char* modelName );

	bool GetBoneTransformation(const std::string& name, Vector3* pos, Vector4* rot);

	// Mesh Bounds
	virtual bool BindMesh(const char* boneName, iMesh* mesh);
	virtual void UnbindMesh(iMesh* mesh);

	void SetCulled(bool cull) { this->culled = cull; }
	bool IsCulled() const { return this->culled; }

	BoundingSphere& GetBoundingSphere() { return this->bs; }

	BTHFBX_RAY_BOX_RESULT RayVsScene(Vector3& rayOrigin, Vector3& rayDirection);

	/*
	virtual void RotateAxis(const Vector3& around, float angle);
	virtual void Rotate(const Vector3& radians);
	*/
};

#pragma warning ( pop )