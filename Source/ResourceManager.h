//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Manager for handling resources. Resources include (but are not limited to):
//	Vertex buffers and textures.
//	Requirements: DirectX device & device context.
//--------------------------------------------------------------------------------------------------
#pragma once

#include "DirectX.h"
#include "Texture.h"
#include <string>
#include <map>

#include "MaloW.h"
#include "MaloWFileDebug.h"
using namespace std;

#ifdef _DEBUG
#include <vld.h>
#endif // _DEBUG

class ResourceManager 
{
	private:
		ID3D11Device*			gDevice;
		ID3D11DeviceContext*	gDeviceContext;

	private:
		std::map<std::string, Texture*> zTextures;

	public:
		ResourceManager();
		virtual ~ResourceManager();

		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		Texture* CreateTextureFromFile(const char* filePath);
		Texture* CreateCubeTexture(const char* filePath);
		//Deletes the texture sent. The pointer is automatically set to NULL.
		void DeleteTexture(Texture* &texture);
};

bool ResourceManagerInit(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
ResourceManager* GetResourceManager() throw(...);
bool FreeResourceManager();