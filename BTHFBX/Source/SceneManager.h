#pragma once

#include "FBXScene.h"
#include "FBXSceneInstance.h"

#include "BTHFbx.h"
#include "BTHFbxScene.h"

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

		
		FBXScene* LoadScene(char* filename);

		bool InitializeFBXSdk();

	public:
		SceneManager();
		virtual ~SceneManager();
		
		virtual IBTHFbxScene* GetScene(char* filename);

		virtual void UpdateScenes(float deltaTime, bool bEnableAnimation);
	};
}