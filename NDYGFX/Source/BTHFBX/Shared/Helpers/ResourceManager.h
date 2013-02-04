/*#pragma once

#include <stdlib.h>

class Shader;
class Texture;
class Device3D;

typedef std::map<std::string, Texture*> TEXTURE_MAP;
typedef std::map<std::string, Shader*> SHADER_MAP;

class ResourceManager
{
	static ResourceManager*	resourceManagerInstance;
	ResourceManager(Device3D* device3D);
	~ResourceManager();

	TEXTURE_MAP	mTextures;
	SHADER_MAP mShaders;

	Device3D*	mDevice3D;

	Texture* LoadTexture(const char* filename);
	Shader* LoadShader(const char* filename, const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numElements);
public:
	static ResourceManager* GetInstance(Device3D* device3D = NULL);
	static void DeleteInstance();

	Texture* GetTexture(const char* filename);
	Shader* GetShader(const char* filename, const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int numElements);

	void Cleanup();
};*/