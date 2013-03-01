#pragma once

#include "FBXScene.h"
#include "FBXSceneInstance.h"

#include "BTHFbx.h"
#include "BTHFbxScene.h"
#include "..\..\NDYGFX\Include\Vector.h"

#include <mutex>

typedef std::map<std::string, FBXScene*> SCENE_MAP;
typedef std::vector<FBXSceneInstance*> SCENE_INSTANCE_VEC;

extern "C"
{
	class SceneManager : public IBTHFbx
	{
		int mSDKMajor,  mSDKMinor,  mSDKRevision;

		FbxManager* mSdkManager;
		SCENE_MAP m_Scenes;
		SCENE_INSTANCE_VEC m_SceneInstances;
		std::mutex m_InstancesMutex;

		
		FBXScene* LoadScene(const char* filename, Vector3& minPos, Vector3& maxPos);

		bool InitializeFBXSdk();

	public:
		SceneManager();
		virtual ~SceneManager();
		
		virtual IBTHFbxScene* GetScene(const char* filename, Vector3& minPos, Vector3& maxPos);
	};
}