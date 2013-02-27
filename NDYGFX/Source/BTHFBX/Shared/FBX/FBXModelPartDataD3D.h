#pragma once

#include "FBXModelPartD3D.h"
#include <map>

class FBXModelD3D;
class IBTHFbxModelPart;

#pragma warning ( push )
#pragma warning ( disable : 4512 ) // Warning C4512: 'FBXModelPartDataD3D' : assignment operator could not be generated


class FBXModelPartDataD3D
{
public:
	const std::string zID;

	Buffer*						mVB_Position;
	Buffer*						mVB_Normal;
	Buffer*						mVB_Tangent;
	Buffer*						mVB_TexCoord;
	Buffer*						mVB_BlendWeights;
	Buffer*						mIB;

	BTHTexture*					mDiffuseTexture;
	BTHTexture*					mNormalTexture;

	FBXModelPartDataD3D(const std::string& uniqueID);
	~FBXModelPartDataD3D();

	void Init(FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, ID3D11Device* dev, ID3D11DeviceContext* devCont);
};

class FBXModelPartDataD3DManager
{
	std::map<std::string, FBXModelPartDataD3D*>	mModelParts;
	std::map<FBXModelPartDataD3D*, unsigned int> zRefCounters;

	FBXModelPartDataD3DManager() {}
	~FBXModelPartDataD3DManager();

	static FBXModelPartDataD3DManager* instance;
public:
	static FBXModelPartDataD3DManager* GetInstance();
	static void DeleteInstance();

	FBXModelPartDataD3D* GetModelData(FBXModelD3D* parentModel, IBTHFbxModelPart* modelPart, int partIndex, ID3D11Device* dev, ID3D11DeviceContext* devCont);
	void FreeModelData(FBXModelPartDataD3D*& data);

	void Cleanup();
};

#pragma warning ( pop )