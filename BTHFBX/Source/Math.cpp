#include "Math.h"

void FBXVec3TransformCoord(FbxVector4* dstVec, FbxVector4* srcVec, FbxMatrix* mat)
{
	if(dstVec && srcVec && mat)
	{
		dstVec->Set( 
			(*srcVec)[0] * mat->Get(0, 0) + (*srcVec)[1] * mat->Get(1, 0) + (*srcVec)[2] * mat->Get(2, 0) + mat->Get(3, 0),
			(*srcVec)[0] * mat->Get(0, 1) + (*srcVec)[1] * mat->Get(1, 1) + (*srcVec)[2] * mat->Get(2, 1) + mat->Get(3, 1),
			(*srcVec)[0] * mat->Get(0, 2) + (*srcVec)[1] * mat->Get(1, 2) + (*srcVec)[2] * mat->Get(2, 2) + mat->Get(3, 2),
			1 
		);
	}
}

void FBXVec3TransformNormal(FbxVector4* dstVec, FbxVector4* srcVec, FbxMatrix* mat)
{
	if(dstVec && srcVec && mat)
	{
		dstVec->Set( 
			(*srcVec)[0] * mat->Get(0, 0) + (*srcVec)[1] * mat->Get(1, 0) + (*srcVec)[2] * mat->Get(2, 0),
			(*srcVec)[0] * mat->Get(0, 1) + (*srcVec)[1] * mat->Get(1, 1) + (*srcVec)[2] * mat->Get(2, 1),
			(*srcVec)[0] * mat->Get(0, 2) + (*srcVec)[1] * mat->Get(1, 2) + (*srcVec)[2] * mat->Get(2, 2),
			0
		);
		
	}
}