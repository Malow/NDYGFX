#include "FBXModelPartDataD3D.h"

#include "FBXModelD3D.h"
#include "..\Source\BTHFBX\Shared\Helpers\ResourceManager.h"

FBXModelPartDataD3D::FBXModelPartDataD3D()
{
	mVB_Position = NULL;
	mVB_Normal = NULL;
	mVB_Tangent = NULL;
	mVB_TexCoord = NULL;
	mVB_BlendWeights = NULL;
	mIB = NULL;

	mDiffuseTexture = NULL;
	mNormalTexture = NULL;
}

FBXModelPartDataD3D::~FBXModelPartDataD3D()
{
	SAFE_DELETE(mVB_Position);
	SAFE_DELETE(mVB_Normal);
	SAFE_DELETE(mVB_Tangent);
	SAFE_DELETE(mVB_TexCoord);
	SAFE_DELETE(mVB_BlendWeights);
	SAFE_DELETE(mIB);
}

inline std::string GetFileName(std::string str)
{
	std::string::size_type pos = str.find_last_of("/\\");

	if( pos != std::string::npos )
		return str.substr( pos+1, std::string::npos );
	return str;
}

void FBXModelPartDataD3D::Init(FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, Device3D* device3D)
{
	mDiffuseTexture = ResourceManager::GetInstance(device3D)->GetTexture(modelPart->GetMaterial()->GetDiffuseTextureFilename());
	if(!mDiffuseTexture)
	{
		std::string filename = "../Meshes/" + GetFileName(modelPart->GetMaterial()->GetDiffuseTextureFilename());
		mDiffuseTexture = ResourceManager::GetInstance(device3D)->GetTexture(filename.c_str());
	}

	mNormalTexture = ResourceManager::GetInstance(device3D)->GetTexture(modelPart->GetMaterial()->GetNormalTextureFilename());
	if(!mNormalTexture)
	{
		std::string filename = "../Meshes/" + GetFileName(modelPart->GetMaterial()->GetNormalTextureFilename());
		mNormalTexture = ResourceManager::GetInstance(device3D)->GetTexture(filename.c_str());
	}

	BUFFER_INIT_DESC bufferDesc;
	bufferDesc.ElementSize = sizeof(D3DXVECTOR3);
	bufferDesc.InitData = modelPart->GetVertexPositionData();
	bufferDesc.NumElements = modelPart->GetVertexCount();
	bufferDesc.Type = VERTEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mVB_Position = myNew Buffer();
	if(FAILED(mVB_Position->Init(device3D, bufferDesc)))
	{
		return;
	}


	bufferDesc.ElementSize = sizeof(D3DXVECTOR3);
	bufferDesc.InitData = modelPart->GetVertexNormalData();
	bufferDesc.NumElements = modelPart->GetVertexCount();
	bufferDesc.Type = VERTEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mVB_Normal = myNew Buffer();
	if(FAILED(mVB_Normal->Init(device3D, bufferDesc)))
	{
		return;
	}



	if(modelPart->GetVertexTangentData())
	{
		bufferDesc.ElementSize = sizeof(D3DXVECTOR3);
		bufferDesc.InitData = modelPart->GetVertexTangentData();
		bufferDesc.NumElements = modelPart->GetVertexCount();
		bufferDesc.Type = VERTEX_BUFFER;
		bufferDesc.Usage = BUFFER_DEFAULT;

		mVB_Tangent = myNew Buffer();
		if(FAILED(mVB_Tangent->Init(device3D, bufferDesc)))
		{
			return;
		}
	}



	bufferDesc.ElementSize = sizeof(D3DXVECTOR2);
	bufferDesc.InitData = modelPart->GetVertexTexCoordData();
	bufferDesc.NumElements = modelPart->GetVertexCount();
	bufferDesc.Type = VERTEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mVB_TexCoord = myNew Buffer();
	if(FAILED(mVB_TexCoord->Init(device3D, bufferDesc)))
	{
		return;
	}


	bufferDesc.ElementSize = sizeof(BTHFBX_BLEND_WEIGHT_DATA);
	bufferDesc.InitData = modelPart->GetVertexBoneWeightData();
	bufferDesc.NumElements = modelPart->GetVertexCount();
	bufferDesc.Type = VERTEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mVB_BlendWeights = myNew Buffer();
	if(FAILED(mVB_BlendWeights->Init(device3D, bufferDesc)))
	{
		return;
	}


	bufferDesc.ElementSize = sizeof(unsigned long);
	bufferDesc.InitData = modelPart->GetIndexData();
	bufferDesc.NumElements = modelPart->GetIndexCount();
	bufferDesc.Type = INDEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mIB = myNew Buffer();
	if(FAILED(mIB->Init(device3D, bufferDesc)))
	{
		return;
	}
}


FBXModelPartDataD3DManager* FBXModelPartDataD3DManager::instance = NULL;
FBXModelPartDataD3DManager* FBXModelPartDataD3DManager::GetInstance()
{
	if(!instance)
		instance = myNew FBXModelPartDataD3DManager();

	return instance;
}

void FBXModelPartDataD3DManager::DeleteInstance()
{
	if(instance)
		SAFE_DELETE(instance);
}

FBXModelPartDataD3DManager::~FBXModelPartDataD3DManager()
{
	Cleanup();
}

FBXModelPartDataD3D* FBXModelPartDataD3DManager::GetModelData(class FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, Device3D* device3D, int partIndex)
{
	char toFind[255];
	sprintf_s(toFind, sizeof(toFind), "%s_%d", parentModel->GetName(), partIndex);
	MODEL_PART_DATA_MAP::iterator i = mModelParts.find(toFind);

	if(i != mModelParts.end())
	{
		return i->second;
	}

	FBXModelPartDataD3D* data = myNew FBXModelPartDataD3D();
	data->Init(parentModel, modelPart, device3D);

	mModelParts[toFind] = data;

	return data;
}

void FBXModelPartDataD3DManager::Cleanup()
{
	for(MODEL_PART_DATA_MAP::iterator i = mModelParts.begin(); i != mModelParts.end(); i++)
	{
		SAFE_DELETE(i->second);
	}
}