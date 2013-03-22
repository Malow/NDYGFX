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
		Vector3 pos;
		Vector4 rot;

		// Bone Position
		if ( GetBoneTransformation(i->second, &pos, &rot) )
		{
			if ( i->first )
			{
				i->first->SetPosition(pos);
				i->first->SetQuaternion(rot);
			}
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
	this->zScene->Render(dt, 
		world, 
		worldInverseTranspose, 
		camProj, 
		camView, 
		camViewProj,
		shad, 
		devCont,
		zHiddenModels );
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
	{
		zBoundMeshes.erase(i);
	}
}

bool FBXMesh::GetBoneTransformation(const std::string& name, Vector3* pos, Vector4* rot)
{
	zSceneMutex.lock();
	if ( zScene && zScene->GetSkeleton() )
	{
		IBTHFbxSkeletonBone* bone = zScene->GetSkeleton()->GetBone(name.c_str());

		if ( bone )
		{
			// 4x4 Matrix
			const float* matrix = bone->GetCombinedTransform();
			D3DXMATRIX worldMat = GetWorldMatrix();

			// Convert To DX Matrix
			D3DXMATRIX boneMatrix;
			for( unsigned int x=0; x<16; ++x )
			{
				boneMatrix[x] = matrix[x];
			}

			// Translation
			if ( pos )
			{
				// World Position
				D3DXVECTOR4 worldPos;
				worldPos.x = boneMatrix._41 * -1.0f;
				worldPos.y = boneMatrix._42;
				worldPos.z = boneMatrix._43;
				worldPos.w = 1.0;

				D3DXVec4Transform(&worldPos, &worldPos, &worldMat);

				pos->x = worldPos.x;
				pos->y = worldPos.y;
				pos->z = worldPos.z;
			}

			// Rotation
			if ( rot )
			{
				D3DXMATRIX rotationMatrix = boneMatrix;

				// Extract Bone Quaternion Rotation
				D3DXQUATERNION quat;
				D3DXQuaternionRotationMatrix(&quat, &rotationMatrix);

				// Rotation around x axis
				D3DXQUATERNION xAxisRotation;
				D3DXQuaternionRotationAxis(&xAxisRotation, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), 3.141592f);

				// Multiply
				D3DXQuaternionMultiply(&quat, &quat, &xAxisRotation);

				// World matrix rotation
				D3DXQUATERNION worldQuat;
				D3DXQuaternionRotationMatrix(&worldQuat, &worldMat);

				// Multiple
				D3DXQuaternionMultiply(&quat, &quat, &worldQuat);

				// Normalize
				D3DXQuaternionNormalize(&quat, &quat);

				rot->x = quat.x;
				rot->y = quat.y;
				rot->z = quat.z;
				rot->w = quat.w;
			}

			zSceneMutex.unlock();
			return true;
		}
	}

	zSceneMutex.unlock();
	return false;
}

void FBXMesh::RenderShadow( float dt, D3DXMATRIX& lightViewProj, Shader* shad, ID3D11DeviceContext* devCont, D3DXVECTOR3 sunDir )
{
	this->RecreateWorldMatrix();
	D3DXMATRIX world = this->GetWorldMatrix();
	this->zScene->RenderShadow(0, world, lightViewProj, shad, devCont, sunDir );
}

BTHFBX_RAY_BOX_RESULT FBXMesh::RayVsScene(Vector3& rayOrigin, Vector3& rayDirection)
{
	D3DXMATRIX worldMat = this->GetWorldMatrix();
	BTHFBX_RAY ray;
	ray.Origin = BTHFBX_VEC3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
	ray.Direction = BTHFBX_VEC3(rayDirection.x, rayDirection.y, rayDirection.z);
	
	BTHFBX_MATRIX mat;
	for(int i = 0; i < 16; i++)
	{
		mat.f[i] = worldMat[i];
	}

	return this->zScene->RayVsScene(ray, &mat);
}

void FBXMesh::HideModel( const char* name, const bool& flag )
{
	if ( flag )
	{
		zHiddenModels.insert(name);
	}
	else
	{
		zHiddenModels.erase(name);
	}
}

bool FBXMesh::IsModelHidden( const char* modelName )
{
	return ( zHiddenModels.count(modelName) > 0 );
}


/*
void FBXMesh::Rotate( const Vector3& radians )
{
	Mesh::Rotate( D3DXVECTOR3(-radians.x,radians.y,radians.z) );
}

void FBXMesh::RotateAxis( const Vector3& around, float angle )
{
	Mesh::RotateAxis( D3DXVECTOR3(-around.x,around.y,around.z), angle );
}
*/
