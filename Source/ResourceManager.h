//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Manager for handling resources. Resources include (but are not limited to):
//	Vertex buffers and textures.
//	Requirements: DirectX device & device context.
//--------------------------------------------------------------------------------------------------
#include "DirectX.h"
#include "Texture.h"
#include <string>
#include <map>

#include "MaloW.h"
#include "MaloWFileDebug.h"
using namespace std;

#include <vld.h>

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

	//ID3D11Texture2D* CreateTexture2D(const char* filePath);
	ID3D11ShaderResourceView* CreateShaderResourceViewFromFile(const char* filePath);

};
