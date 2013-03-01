#pragma once

#include "FBXSDK.h"
#include "FBXCommon.h"
#include "Model.h"
#include "Skeleton.h"
#include "BoneWeights.h"
#include "Curve.h"
#include <sstream>
#include "..\..\NDYGFX\include\Vector.h"

class FBXScene
{
	friend class FBXSceneInstance;

	FbxManager* mSdkManager;
	FbxScene* mScene;

	std::string mFilename;

	std::vector<FbxSurfaceMaterial*> m_FBXMaterials;
	std::vector<Material*> m_Materials;
	std::vector<Curve>	m_Curves;
	Dictionary<Model*> m_Models;

	Skeleton* m_pSkeleton;
	AnimationController* m_pAnimationController;
	BTHFBX_AABB_DATA m_BoundingBox;

	void UpdateBoundingBoxDataFromVertex(BTHFBX_VEC3 vertexPosition);
	bool LoadScene(const char* filename, std::ostream& output, Vector3& minPos, Vector3& maxPos );
	void DisplayMetaData();

public:
	FBXScene(FbxManager* sdkManager);
	~FBXScene();

	// Initialize
	bool Init(const char* filename, Vector3& minPos, Vector3& maxPos);
	
	// Data Access
	inline Dictionary<Model*>& getModels() {return m_Models; }
	inline std::vector<Material*>& GetMaterials() {return m_Materials; }
	inline FbxScene* GetScene() { return mScene; }
	inline unsigned int GetModelCount() { return m_Models.GetCount(); }
	inline Model* GetModel(const unsigned int& index) { return m_Models.GetValueAt(index); }
	inline bool IsSkinned() {return m_pSkeleton != NULL; }
	inline Skeleton* GetSkeleton() { return m_pSkeleton; }
	inline AnimationController* GetAnimationController() { return m_pAnimationController; }
	inline void SetSkeleton(Skeleton* skeleton) { m_pSkeleton = skeleton; }
	inline void SetAnimationController( AnimationController* controller ) { m_pAnimationController = controller; }

	// Processing
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

	// Logic
	bool HasFBXAnimation(FbxNode *pNode);
	BTHFBX_VEC2 GetTexCoord(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	FbxVector4 GetNormal(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	FbxVector4 GetTangent(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	int GetMappingIndex(FbxLayerElement::EMappingMode MappingMode, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	Material* GetMaterial(FbxSurfaceMaterial* pFBXMaterial);
	Material* GetMaterialLinkedWithPolygon(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex);
	FbxMatrix GetGeometricOffset2(FbxNode* pNode);
	FbxMatrix GetAbsoluteTransformFromCurrentTake2(FbxNode* pNode, FbxTime time);
	BTHFBX_VEC3 GetMaterialColor2(FbxPropertyT<FbxDouble3> FBXColorProperty, FbxPropertyT<FbxDouble> FBXFactorProperty);
};