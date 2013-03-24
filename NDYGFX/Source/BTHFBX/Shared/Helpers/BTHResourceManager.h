#pragma once

#include <map>
#include "..\..\..\DirectX.h"
#include <mutex>

class BTHTexture;

class BTHResourceManager
{
	std::mutex zMutex;
	BTHResourceManager();
	~BTHResourceManager();

	std::map<std::string, BTHTexture*> mTextures;
	std::map<BTHTexture*, unsigned int> zRefCounters;

	BTHTexture* LoadTexture(const std::string& filename, ID3D11Device* dev, ID3D11DeviceContext* devCont);
public:

	BTHTexture* GetTexture(const std::string& filename, ID3D11Device* dev, ID3D11DeviceContext* devCont);
	void FreeTexture( BTHTexture*& texture );

	void Cleanup();

	// Singleton
	static BTHResourceManager* GetInstance();
};