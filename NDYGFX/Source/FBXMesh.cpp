#include "FBXMesh.h"


FBXMesh::FBXMesh( D3DXVECTOR3 pos, string filePath ) : Mesh(pos, filePath)
{
	this->zScene = new FBXSceneD3D();
}

FBXMesh::~FBXMesh()
{
	zSceneMutex.lock();
	if(this->zScene)
	{
		delete this->zScene;
	}
	zSceneMutex.unlock();
}

MaloW::Array<MeshStrip*>* FBXMesh::GetStrips()
{
	return NULL;
}

void FBXMesh::Update( float dt )
{
	// Update Animation
	UpdateAnimationQueue();

	// Lock Scene
	zSceneMutex.lock();

	// Update Scene
	this->zScene->Update(dt);

	// Unlock Scene
	zSceneMutex.unlock();

	// Move Bound Meshes
	for( auto i = zBoundMeshes.begin(); i != zBoundMeshes.end(); ++i )
	{
		float x, y, z;

		// Bone Position
		if ( GetBonePosition(i->second, x, y, z) )
		{
			i->first->SetPosition(Vector3(x, y, z));
		}
	}
}

void FBXMesh::Render(float dt, D3DXMATRIX& camProj, D3DXMATRIX& camView, D3DXMATRIX& camViewProj, Shader* shad, ID3D11DeviceContext* devCont)
{
	this->RecreateWorldMatrix();
	D3DXMATRIX world = this->GetWorldMatrix();

	D3DXMATRIX worldInverseTranspose;
	D3DXMatrixIdentity(&worldInverseTranspose);
	D3DXMatrixInverse(&worldInverseTranspose, NULL, &world);
	D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);

	zSceneMutex.lock();
	this->zScene->Render(dt, world, worldInverseTranspose, camProj, camView, camViewProj, shad, devCont );
	zSceneMutex.unlock();
}

bool FBXMesh::LoadFromFile( string file, IBTHFbx* fbx, ID3D11Device* dev, ID3D11DeviceContext* devCont )
{
	Vector3 min = Vector3(99999.9f, 99999.9f, 99999.9f);
	Vector3 max = Vector3(-99999.9f, -99999.9f, -99999.9f);

	zSceneMutex.lock();
	this->zScene->Init(file.c_str(), fbx, dev, devCont, min, max);
	this->zScene->GetAnimationController()->SetCurrentAnimation(0);
	this->zScene->GetAnimationController()->Play();
	zSceneMutex.unlock();

	this->bs = BoundingSphere(min, max);

	return true;
}

void FBXMesh::UpdateAnimationQueue()
{
	if ( zAnimationQueue.empty() ) return;

	// Calculate Current Time
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - zQueueStarted);
	float seconds = (float)ms.count() * 0.001f;

	// Set Check Current
	unsigned int curAnimationIndex = 0;
	for( unsigned int x=0; x<zAnimationQueue.size(); ++x )
	{
		if ( seconds >= zAnimationQueue[x].second )
		{
			seconds -= zAnimationQueue[x].second;
			curAnimationIndex++;
		}
		else
		{
			break;
		}
	}

	// Limit to last animation
	if ( curAnimationIndex >= zAnimationQueue.size() )
	{
		curAnimationIndex = zAnimationQueue.size() - 1;
	}

	// Check Animation Index
	if ( curAnimationIndex != zLastAnimationIndex )
	{
		if ( zScene && zScene->GetAnimationController() )
		{
			zScene->GetAnimationController()->SetCurrentAnimation(zAnimationQueue[curAnimationIndex].first.c_str());
			zScene->GetAnimationController()->Update(seconds);
		}

		zLastAnimationIndex = curAnimationIndex;
	}
}

bool FBXMesh::SetAnimation( unsigned int ani )
{
	zSceneMutex.lock();
	if ( !this->zScene->GetAnimationController() )
	{
		MaloW::Debug(
			"Tried to apply animation at index " + 
			MaloW::convertNrToString(ani) + 
			" to an FBX mesh that only has not been loaded!"); 

		zSceneMutex.unlock();
		return false;
	}

	if ( !this->zScene->GetAnimationController()->SetCurrentAnimation(ani) )
	{	
		MaloW::Debug(
			"Tried to apply animation at index " + 
			MaloW::convertNrToString(ani) + 
			" to an FBX mesh that only has " + 
			MaloW::convertNrToString(this->zScene->GetAnimationController()->GetAnimationCount()) + 
			" animations.");
		
		zSceneMutex.unlock();
		return false;
	}

	// Delete Queue
	zAnimationQueue.clear();

	zSceneMutex.unlock();
	return true;
}

bool FBXMesh::SetAnimation( const char* name )
{
	zSceneMutex.lock();

	if ( !this->zScene->GetAnimationController() )
	{
		MaloW::Debug(
			"Tried to apply animation named " + 
			std::string(name) + 
			" to an FBX mesh that only has not been loaded!"); 

		zSceneMutex.unlock();
		return false;
	}

	if ( !this->zScene->GetAnimationController()->SetCurrentAnimation(name) )
	{	
		MaloW::Debug(
			"Tried to apply animation at index " + 
			std::string(name) + 
			" to an FBX mesh that only has " + 
			MaloW::convertNrToString(this->zScene->GetAnimationController()->GetAnimationCount()) + 
			" animations."); 
		
		zSceneMutex.unlock();
		return false;
	}

	zSceneMutex.unlock();

	// Delete Queue
	zAnimationQueue.clear();

	return true;
}

void FBXMesh::SetAnimationQueue( const char* const* names, const float* times, const unsigned int& count )
{
	zAnimationQueue.clear();
	for( unsigned int x=0; x<count; ++x )
	{
		zAnimationQueue.push_back( std::pair<std::string, float>(names[x], times[x]) );
	}

	zLastAnimationIndex = count;
	zQueueStarted = std::chrono::system_clock::now();
	UpdateAnimationQueue();
}

bool FBXMesh::BindMesh(const char* boneName, iMesh* mesh)
{
	// Lock Scene
	zSceneMutex.lock();

	// Bind
	zBoundMeshes[mesh] = boneName;

	// Unlock Mutex
	zSceneMutex.unlock();
	
	return true;
}

void FBXMesh::UnbindMesh(iMesh* mesh)
{
	auto i = zBoundMeshes.find(mesh);
	if ( i != zBoundMeshes.end() )
		zBoundMeshes.erase(i);
}

bool FBXMesh::GetBonePosition(const std::string& name, float& x, float& y, float& z)
{
	zSceneMutex.lock();
	if ( zScene && zScene->GetSkeleton() )
	{
		IBTHFbxSkeletonBone* bone = zScene->GetSkeleton()->GetBone(name.c_str());

		if ( bone )
		{
			// 4x4 Matrix
			const float* matrix;
			matrix = bone->GetCombinedTransform();

			D3DXVECTOR4 worldPos;
			worldPos.x = matrix[12] * -1.0f;
			worldPos.y = matrix[13];
			worldPos.z = matrix[14];
			worldPos.w = 1.0;

			D3DXMATRIX worldMat = GetWorldMatrix();

			D3DXVec4Transform(&worldPos, &worldPos, &worldMat);

			x = worldPos.x;
			y = worldPos.y;
			z = worldPos.z;

			zSceneMutex.unlock();
			return true;
		}
	}

	zSceneMutex.unlock();
	return false;
}

void FBXMesh::RenderShadow( float dt, D3DXMATRIX& lightViewProj, Shader* shad, ID3D11DeviceContext* devCont )
{
	this->RecreateWorldMatrix();
	D3DXMATRIX world = this->GetWorldMatrix();
	this->zScene->RenderShadow(0, world, lightViewProj, shad, devCont );
}

BTHFBX_RAY_BOX_RESULT FBXMesh::RayVsScene(Vector3& rayOrigin, Vector3& rayDirection)
{
	D3DXMATRIX worldMat = this->GetWorldMatrix();
	BTHFBX_RAY ray;
	ray.Origin = BTHFBX_VEC3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
	ray.Direction = BTHFBX_VEC3(rayDirection.x, rayDirection.y, rayDirection.z);
	BTHFBX_MATRIX mat;
	for(int i = 0; i < 16; i++)
		mat.f[i] = worldMat[i];
	return this->zScene->RayVsScene(ray, &mat);
}
