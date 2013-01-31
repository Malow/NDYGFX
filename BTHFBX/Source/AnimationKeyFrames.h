#pragma once

#include <vector>
#include <string>
#include "FBXSDK.h"


class AnimationQuaternionKeyFrame
{
public:
	//AnimationQuaternionKeyFrame(const D3DXMATRIX& matTransform, const FbxMatrix& matTransform2)
	AnimationQuaternionKeyFrame(const FbxMatrix& matTransform2)
	{
		//D3DXQuaternionRotationMatrix(&m_Quaternion, &matTransform);
		//m_vTranslation = D3DXVECTOR3(matTransform._41,matTransform._42,matTransform._43);


		FbxVector4 tmp0, tmp1;
		double tmpD;
		matTransform2.GetElements(m_vTranslation2, m_Quaternion2, tmp0, tmp1, tmpD);
	}

	//const D3DXQUATERNION& GetQuaternion() const	{ return m_Quaternion; }
	//const D3DXVECTOR3& GetTranslation() const	{ return m_vTranslation; }

	const FbxQuaternion& GetQuaternion2() const	{ return m_Quaternion2; }
	const FbxVector4& GetTranslation2() const	{ return m_vTranslation2; }
protected:
	//D3DXQUATERNION m_Quaternion;
	//D3DXVECTOR3 m_vTranslation;

	FbxQuaternion m_Quaternion2;
	FbxVector4 m_vTranslation2;
};

class AnimationKeyFrames
{
public:
	AnimationKeyFrames(const std::string& strAnimationName);
	~AnimationKeyFrames();

	//void AddKeyFrame(const D3DXMATRIX& matTransform, const FbxMatrix& matTransform2);
	void AddKeyFrame(const FbxMatrix& matTransform2);

	std::string GetAnimationName();

	//const D3DXMATRIX& GetKeyFrameTransform(int nKeyFrame);
	const FbxMatrix& GetKeyFrameTransform2(int nKeyFrame);
	const AnimationQuaternionKeyFrame& GetKeyFrameQuaternion(int nKeyFrame);

protected:
	std::string m_strAnimationName;
	std::vector<std::pair<FbxMatrix, AnimationQuaternionKeyFrame>> m_KeyFrames2;
};