#pragma once

#include <BTHFbxCommon.h>

inline void FbxMatrixToBTHFBX_MATRIX(const FbxMatrix& srcMatrix, BTHFBX_MATRIX& dstMatrix)
{
	for(int y = 0; y < 4; y++)
		for(int x = 0; x < 4; x++)
			dstMatrix.f[y*4+x] = (float)srcMatrix.Get(y, x);
}

inline void BTHFBX_MATRIXToFbxMatrix(const BTHFBX_MATRIX& srcMatrix, FbxMatrix& dstMatrix)
{
	for(int y = 0; y < 4; y++)
		for(int x = 0; x < 4; x++)
			dstMatrix.Set(y, x, (double)srcMatrix.f[y*4+x]);
}

inline BTHFBX_VEC2 KFbxVector2ToBTHFBX_VEC2(const FbxVector2& vSrcVector)
{
	return BTHFBX_VEC2( (float)vSrcVector[0], (float)vSrcVector[1] );
}

inline BTHFBX_VEC3 FbxVector4ToBTHFBX_VEC3(const FbxVector4& vSrcVector)
{
	return BTHFBX_VEC3( (float)vSrcVector[0], (float)vSrcVector[1], (float)vSrcVector[2] );
}

inline BTHFBX_VEC2 FBXTexCoordToD3DTexCoord(const BTHFBX_VEC2& vSrcVector)
{
	return BTHFBX_VEC2( vSrcVector.x, 1.0f - vSrcVector.y );
}

inline FbxQuaternion qLerp(const FbxQuaternion &q1, const FbxQuaternion &q2, double t) 
{
	FbxQuaternion tmp = (q1*(1-t) + q2*t);
	tmp.Normalize();
	return tmp;
}

inline FbxQuaternion qSlerp(const FbxQuaternion &q1, const FbxQuaternion &q2, double t) 
{
	FbxQuaternion q3;
	double dot = q1.DotProduct(q2);

	/*	dot = cos(theta)
		if (dot < 0), q1 and q2 are more than 90 degrees apart,
		so we can invert one to reduce spinning	*/
	if (dot < 0)
	{
		dot = -dot;
		q3 = -q2;
	} else q3 = q2;
		
	if (dot < 0.95f)
	{
		double angle = acos(dot);
		return (q1*sin(angle*(1-t)) + q3*sin(angle*t))/sin(angle);
	} else // if the angle is small, use linear interpolation								
		return qLerp(q1,q3,t);
}