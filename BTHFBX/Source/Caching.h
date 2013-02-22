#pragma once

#include "FBXScene.h"
#include "BoneWeights.h"
#include <string>

bool LoadCachedScene(const std::string& fileName, FBXScene* scene);
bool CacheScene(const std::string& fileName, FBXScene* scene);