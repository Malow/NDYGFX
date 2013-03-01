#include "SceneManager.h"
#include "FBXScene.h"
#include <thread>


SceneManager::SceneManager()
	: mSdkManager(0), mSDKMajor(0), mSDKMinor(0), mSDKRevision(0)
{
	
}

SceneManager::~SceneManager()
{
	m_InstancesMutex.lock();
	for(auto i = m_SceneInstances.begin(); i != m_SceneInstances.end(); i++)
	{
		if ( *i ) delete *i, *i = 0;
	}
	m_InstancesMutex.unlock();

	for(auto i = m_Scenes.begin(); i != m_Scenes.end(); i++)
	{
		if (i->second) delete i->second, i->second = 0;
	}

	if ( mSdkManager ) 
	{
		mSdkManager->Destroy();
	}
}

FBXScene* SceneManager::LoadScene(const char* fileName, Vector3& minPos, Vector3& maxPos)
{
	if(!mSdkManager)
	{
		if(!InitializeFBXSdk())
			return false;
	}

	FBXScene* scene = new FBXScene(mSdkManager);

	if(!scene->Init(fileName, minPos, maxPos))
	{
		if ( scene ) delete scene, scene = 0;
	}
	else
	{
		m_Scenes[fileName] = scene;
	}

	return scene;
}

IBTHFbxScene* SceneManager::GetScene(const char* fileName, Vector3& minPos, Vector3& maxPos)
{
	FBXScene* scene = NULL;
	FBXSceneInstance* sceneInstance = NULL;
	
	SCENE_MAP::iterator i  = m_Scenes.find(fileName);
	if(i != m_Scenes.end() )
	{
		scene = i->second;

		sceneInstance = new FBXSceneInstance(scene);
		sceneInstance->InitInstance();

		m_InstancesMutex.lock();
		m_SceneInstances.push_back(sceneInstance);
		m_InstancesMutex.unlock();
	}
	else
	{
		scene = LoadScene(fileName, minPos, maxPos);

		sceneInstance = new FBXSceneInstance(scene);
		sceneInstance->InitInstance();

		m_InstancesMutex.lock();
		m_SceneInstances.push_back(sceneInstance);
		m_InstancesMutex.unlock();

	}

	return sceneInstance;
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