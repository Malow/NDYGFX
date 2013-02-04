#pragma once

#include "FBXModelPartD3D.h"
#include <map>

class FBXModelPartDataD3D
{
public:
	Buffer*						mVB_Position;
	Buffer*						mVB_Normal;
	Buffer*						mVB_Tangent;
	Buffer*						mVB_TexCoord;
	Buffer*						mVB_BlendWeights;
	Buffer*						mIB;

	BTHTexture*					mDiffuseTexture;
	BTHTexture*					mNormalTexture;

	FBXModelPartDataD3D();
	~FBXModelPartDataD3D();

	void Init(class FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, ID3D11Device* dev, ID3D11DeviceContext* devCont);
};

typedef std::map<std::string, FBXModelPartDataD3D*> MODEL_PART_DATA_MAP;
class FBXModelPartDataD3DManager
{
	MODEL_PART_DATA_MAP	mModelParts;
	FBXModelPartDataD3DManager() {}
	~FBXModelPartDataD3DManager();

	static FBXModelPartDataD3DManager* instance;
public:
	static FBXModelPartDataD3DManager* GetInstance();
	static void DeleteInstance();

	FBXModelPartDataD3D* GetModelData(class FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex, ID3D11Device* dev, ID3D11DeviceContext* devCont);

	void Cleanup();
};