#include "FBXModelPartDataD3D.h"
#include "FBXModelD3D.h"
#include "..\Helpers\BTHResourceManager.h"
#include "FBXSceneD3D.h"
#include <sstream>


FBXModelPartDataD3D::FBXModelPartDataD3D(const std::string& uniqueID) :
	zID(uniqueID),
	mVB_Position(0),
	mVB_Normal(0),
	mVB_Tangent(0),
	mVB_TexCoord(0),
	mVB_BlendWeights(0),
	mIB(0),
	mDiffuseTexture(0),
	mNormalTexture(0)
{
}

FBXModelPartDataD3D::~FBXModelPartDataD3D()
{
	SAFE_DELETE(mVB_Position);
	SAFE_DELETE(mVB_Normal);
	SAFE_DELETE(mVB_Tangent);
	SAFE_DELETE(mVB_TexCoord);
	SAFE_DELETE(mVB_BlendWeights);
	SAFE_DELETE(mIB);

	if ( mDiffuseTexture ) BTHResourceManager::GetInstance()->FreeTexture(mDiffuseTexture);
	if ( mNormalTexture ) BTHResourceManager::GetInstance()->FreeTexture(mNormalTexture);
}

inline std::string GetFileName(const std::string& str)
{
	std::string::size_type pos = str.find_last_of("/\\");

	if( pos != std::string::npos )
		return str.substr( pos+1, std::string::npos );

	return str;
}

inline std::string GetFilePath(const std::string& str)
{
	std::string::size_type pos = str.find_last_of("/\\");

	if ( pos == std::string::npos )
		return str;

	return str.substr( 0, pos+1 );
}

void FBXModelPartDataD3D::Init(FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	// Scene Path
	std::string scenePath = GetFilePath(parentModel->GetScene()->GetFileName());

	// Textures
	mDiffuseTexture = BTHResourceManager::GetInstance()->GetTexture(scenePath+modelPart->GetMaterial()->GetDiffuseTextureFilename(), dev, devCont);
	mNormalTexture = BTHResourceManager::GetInstance()->GetTexture(scenePath+modelPart->GetMaterial()->GetNormalTextureFilename(), dev, devCont);

	BUFFER_INIT_DESC bufferDesc;
	bufferDesc.ElementSize = sizeof(D3DXVECTOR3);
	bufferDesc.InitData = modelPart->GetVertexPositionData();
	bufferDesc.NumElements = modelPart->GetVertexCount();
	bufferDesc.Type = VERTEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mVB_Position = new Buffer();
	if(FAILED(mVB_Position->Init(dev, devCont, bufferDesc)))
	{
		return;
	}


	bufferDesc.ElementSize = sizeof(D3DXVECTOR3);
	bufferDesc.InitData = modelPart->GetVertexNormalData();
	bufferDesc.NumElements = modelPart->GetVertexCount();
	bufferDesc.Type = VERTEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mVB_Normal = new Buffer();
	if(FAILED(mVB_Normal->Init(dev, devCont, bufferDesc)))
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

		mVB_Tangent = new Buffer();
		if(FAILED(mVB_Tangent->Init(dev, devCont, bufferDesc)))
		{
			return;
		}
	}



	bufferDesc.ElementSize = sizeof(D3DXVECTOR2);
	bufferDesc.InitData = modelPart->GetVertexTexCoordData();
	bufferDesc.NumElements = modelPart->GetVertexCount();
	bufferDesc.Type = VERTEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mVB_TexCoord = new Buffer();
	if(FAILED(mVB_TexCoord->Init(dev, devCont, bufferDesc)))
	{
		return;
	}

	// Is Model Skinned?
	if ( modelPart->IsSkinnedModel() )
	{
		bufferDesc.ElementSize = sizeof(BTHFBX_BLEND_WEIGHT_DATA);
		bufferDesc.InitData = modelPart->GetVertexBoneWeightData();
		bufferDesc.NumElements = modelPart->GetVertexCount();
		bufferDesc.Type = VERTEX_BUFFER;
		bufferDesc.Usage = BUFFER_DEFAULT;

		mVB_BlendWeights = new Buffer();
		if(FAILED(mVB_BlendWeights->Init(dev, devCont, bufferDesc)))
		{
			return;
		}
	}

	// Index Buffer
	bufferDesc.ElementSize = sizeof(unsigned long);
	bufferDesc.InitData = modelPart->GetIndexData();
	bufferDesc.NumElements = modelPart->GetIndexCount();
	bufferDesc.Type = INDEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;

	mIB = new Buffer();
	if(FAILED(mIB->Init(dev, devCont, bufferDesc)))
	{
		return;
	}
}


FBXModelPartDataD3DManager* FBXModelPartDataD3DManager::instance = NULL;
FBXModelPartDataD3DManager* FBXModelPartDataD3DManager::GetInstance()
{
	static FBXModelPartDataD3DManager manager;

	/*
	if(!instance)
		instance = new FBXModelPartDataD3DManager();
	*/

	return &manager;
}

void FBXModelPartDataD3DManager::DeleteInstance()
{
	if(instance)
		delete instance;
}

FBXModelPartDataD3DManager::~FBXModelPartDataD3DManager()
{
	Cleanup();
}

FBXModelPartDataD3D* FBXModelPartDataD3DManager::GetModelData(FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex, ID3D11Device* dev, ID3D11DeviceContext* devCont)
{
	// Unique Part Identifier
	std::stringstream ss;
	ss << parentModel->GetScene()->GetFileName() << "_";
	ss << parentModel->GetName() << "_";
	ss << partIndex;

	// Find Cached Model Part
	auto i = mModelParts.find(ss.str());

	// Model Part Found
	if( i != mModelParts.end() )
	{
		zRefCounters[i->second]++;
		return i->second;
	}

	// Insert Data
	FBXModelPartDataD3D* data = new FBXModelPartDataD3D(ss.str());
	data->Init(parentModel, modelPart, dev, devCont);
	mModelParts[ss.str()] = data;
	zRefCounters[data] = 1;

	return data;
}

void FBXModelPartDataD3DManager::FreeModelData(FBXModelPartDataD3D*& data)
{
	// Try Finding Reference Counter
	auto i = zRefCounters.find(data);
	if ( i != zRefCounters.end() )
	{	
		i->second--;
		if ( i->second == 0 )
		{
			mModelParts.erase(mModelParts.find(i->first->zID));
			zRefCounters.erase(i);
			delete data;
		}
	}

	data=0;
}

void FBXModelPartDataD3DManager::Cleanup()
{
	for(auto i = mModelParts.begin(); i != mModelParts.end(); i++)
	{
		SAFE_DELETE(i->second);
	}
}