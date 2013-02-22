#pragma once

#include "BTHFbxCommon.h"
#include "FBXSDK.h"

bool ClipSegment(double min, double max, double a, double b, double d, double& t0, double& t1);
bool ClipSegment(const BTHFBX_AABB_DATA& box, FbxVector4 start, FbxVector4 end, float& dist1, float& dist2);

float BoxVsRay(FbxVector4 rayOrigin, FbxVector4 rayDir, const BTHFBX_AABB_DATA& box);