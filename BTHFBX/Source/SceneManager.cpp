#include "SceneManager.h"
#include "FBXScene.h"

SceneManager::SceneManager()
	: mSdkManager(0), mSDKMajor(0), mSDKMinor(0), mSDKRevision(0)
{
	
}

SceneManager::~SceneManager()
{
	for(auto i = m_SceneInstances.begin(); i != m_SceneInstances.end(); i++)
	{
		if ( *i ) delete *i, *i = 0;
	}

	for(auto i = m_Scenes.begin(); i != m_Scenes.end(); i++)
	{
		if (i->second) delete i->second, i->second = 0;
	}

	if(mSdkManager)	mSdkManager->Destroy();
}

FBXScene* SceneManager::LoadScene(char* filename)
{
	if(!mSdkManager)
	{
		if(!InitializeFBXSdk())
			return false;
	}

	FBXScene* scene = new FBXScene(mSdkManager);

	if(!scene->Init(filename))
	{
		if ( scene ) delete scene, scene = 0;
	}
	else
	{
		m_Scenes[filename] = scene;
	}

	return scene;
}

IBTHFbxScene* SceneManager::GetScene(char* filename)
{
	FBXScene* scene = NULL;
	FBXSceneInstance* sceneInstance = NULL;
	
	SCENE_MAP::iterator i  = m_Scenes.find(filename);
	if(i != m_Scenes.end() )
	{
		scene = i->second;

		sceneInstance = new FBXSceneInstance(scene);
		sceneInstance->InitInstance();

		m_SceneInstances.push_back(sceneInstance);
	}
	else
	{
		scene = LoadScene(filename);

		//mResources.AddModelsFromScene(filename, scene);

		sceneInstance = new FBXSceneInstance(scene);
		sceneInstance->InitInstance();

		m_SceneInstances.push_back(sceneInstance);
	}

	return sceneInstance;
}

void SceneManager::UpdateScenes(float deltaTime, bool bEnableAnimation)
{
	/*
	for(SCENE_MAP::iterator i = m_Scenes.begin(); i != m_Scenes.end(); i++)
	{
		i->second->UpdateScene(deltaTime, bEnableAnimation);
	}
	*/

	for(SCENE_INSTANCE_VEC::iterator i = m_SceneInstances.begin(); i != m_SceneInstances.end(); i++)
	{
		(*i)->UpdateScene(deltaTime, bEnableAnimation);
	}
}

bool SceneManager::InitializeFBXSdk()
{
    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(mSDKMajor, mSDKMinor, mSDKRevision);

    // The first thing to do is to create the FBX SDK manager which is the 
    // object allocator for almost all the classes in the SDK.
	mSdkManager = FbxManager::Create();
	if(!mSdkManager) return false;
	
	FbxString lPath = FbxGetApplicationDirectory();
	FbxString lExtension = "dll";
	mSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	printf("FBX version number for this FBX SDK is %d.%d.%d\n", mSDKMajor, mSDKMinor, mSDKRevision);

	return true;
}