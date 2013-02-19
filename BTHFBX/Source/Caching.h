#pragma once

#include "FBXScene.h"
#include "BoneWeights.h"
#include <string>


bool LoadCachedBoneWeights(const std::string& fileName, std::vector<BoneWeights>& weights);
bool CacheBoneWeights(const std::string& fileName, const std::vector<BoneWeights>& weights);

bool LoadCachedScene(const std::string& fileName, FBXScene* scene);
bool CacheScene(const std::string& fileName, FBXScene* scene);