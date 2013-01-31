#pragma once

#include "BTHFbxCommon.h"

#include "FBXSDK.h"

void FBXVec3TransformCoord(FbxVector4* dstVec, FbxVector4* srcVec, FbxMatrix* mat);
void FBXVec3TransformNormal(FbxVector4* dstVec, FbxVector4* srcVec, FbxMatrix* mat);
