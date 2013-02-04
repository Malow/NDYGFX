#pragma once

#include <map>
#include "..\..\..\DirectX.h"

class BTHTexture;

typedef std::map<std::string, BTHTexture*> TEXTURE_MAP;

class BTHResourceManager
{
	static BTHResourceManager*	resourceManagerInstance;
	BTHResourceManager();
	~BTHResourceManager();

	TEXTURE_MAP	mTextures;

	BTHTexture* LoadTexture(const char* filename, ID3D11Device* dev, ID3D11DeviceContext* devCont);
public:
	static BTHResourceManager* GetInstance();
	static void DeleteInstance();

	BTHTexture* GetTexture(const char* filename, ID3D11Device* dev, ID3D11DeviceContext* devCont);

	void Cleanup();
};