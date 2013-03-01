#pragma once

#include "FBXScene.h"
#include "BoneWeights.h"
#include <string>
#include "..\..\NDYGFX\Include\Vector.h"


bool LoadCachedScene(const std::string& fileName, FBXScene* scene, Vector3& minPos, Vector3& maxPos);
bool CacheScene(const std::string& fileName, FBXScene* scene);