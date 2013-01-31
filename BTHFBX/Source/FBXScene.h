#pragma once

#include "FBXSDK.h"
#include "FBXCommon.h"
#include "Model.h"
#include "Skeleton.h"
#include "BoneWeights.h"
#include "Curve.h"

class FBXScene
{
	friend class FBXSceneInstance;

	FbxManager* mSdkManager;
	FbxScene* mScene;

	bool LoadScene(const char* filename);

	void DisplayMetaData();

	//FbxNode* mHierarchyRoot;
	//std::map<FbxString, FbxNode*> mHierarchyEntries;

private:
	std::string mFilename;

	std::vector<FbxSurfaceMaterial*> m_FBXMaterials;
	std::vector<Material*> m_Materials;
	std::vector<Curve>	m_Curves;
	Dictionary<Model*> m_Models;

	Skeleton* m_pSkeleton;
	AnimationController* m_pAnimationController;
	BTHFBX_AABB_DATA m_BoundingBox;

	void UpdateBoundingBoxDataFromVertex(BTHFBX_VEC3 vertexPosition);

public:
	FBXScene(FbxManager* sdkManager);
	~FBXScene();

	bool Init(const char* filename);
	
	FbxScene* GetScene() { return mScene; }

	int GetModelCount() { return m_Models.GetCount(); }
	Model* GetModel(int index) { return m_Models.GetValueAt(index); }
	bool IsSkinned() {return m_pSkeleton != NULL; }
	Skeleton* GetSkeleton() { return m_pSkeleton; }

	void ProcessBlendWeights();
	void ProcessScene(FbxScene* pScene);
	void ProcessMaterials(FbxScene* pScene);
	void ProcessNode(FbxNode* pNode, FbxNodeAttribute::EType attributeType);
	void ProcessSkeleton(FbxNode* pNode);
	void ProcessMesh(FbxNode* pNode);
	void ProcessCurve(FbxNode* pNode);
	void ProcessSkeleteonBoundingBoxes();

	void ProcessAnimations(FbxScene* pScene);
	void ProcessAnimation(FbxNode* pNode, const char* strTakeName, float fFrameRate, float fStart, float fStop);

	void ProcessBoneWeights(FbxMesh* pFBXMesh, std::vector<BoneWeights>& meshBoneWeights);
	void ProcessBoneWeights(FbxSkin* pFBXSkin, std::vector<BoneWeights>& meshBoneWeights);

	bool HasFBXAnimation(FbxNode *pNode);

	//D3DXMATRIX GetGeometricOffset(FbxNode* pNode);
	BTHFBX_VEC2 GetTexCoord(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	FbxVector4 GetNormal(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	FbxVector4 GetTangent(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	int GetMappingIndex(FbxLayerElement::EMappingMode MappingMode, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	Material* GetMaterial(FbxSurfaceMaterial* pFBXMaterial);
	Material* GetMaterialLinkedWithPolygon(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	//D3DXMATRIX GetAbsoluteTransformFromCurrentTake(FbxNode* pNode, FbxTime time);
	//D3DXVECTOR3 GetMaterialColor(KFbxPropertyDouble3 FBXColorProperty, KFbxPropertyDouble1 FBXFactorProperty);


	FbxMatrix GetGeometricOffset2(FbxNode* pNode);
	FbxMatrix GetAbsoluteTransformFromCurrentTake2(FbxNode* pNode, FbxTime time);

	BTHFBX_VEC3 GetMaterialColor2(FbxPropertyT<FbxDouble3> FBXColorProperty, FbxPropertyT<FbxDouble> FBXFactorProperty);
};