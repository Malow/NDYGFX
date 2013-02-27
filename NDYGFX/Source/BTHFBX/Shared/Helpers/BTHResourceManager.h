#pragma once

#include <map>
#include "..\..\..\DirectX.h"

class BTHTexture;

class BTHResourceManager
{
	static BTHResourceManager*	resourceManagerInstance;
	BTHResourceManager();
	~BTHResourceManager();

	std::map<std::string, BTHTexture*> mTextures;
	std::map<BTHTexture*, unsigned int> zRefCounters;

	BTHTexture* LoadTexture(const std::string& filename, ID3D11Device* dev, ID3D11DeviceContext* devCont);
public:
	static BTHResourceManager* GetInstance();
	static void DeleteInstance();

	BTHTexture* GetTexture(const std::string& filename, ID3D11Device* dev, ID3D11DeviceContext* devCont);
	void FreeTexture( BTHTexture*& texture );

	void Cleanup();
};