//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Manager for handling resources. 
//	Resources include:
//		Mesh(strips).
//		Textures.
//	Requirements: DirectX device & device context.
//--------------------------------------------------------------------------------------------------
#pragma once

//#include "DirectX.h"
#include "Texture.h"
//#include "MeshCounted.h" //**TILLMAN circkulär include - Object3d.h**
//#include "ObjLoader.h"
#include <string>
#include <map>

#include "Array.h"
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
		//std::map<std::string, MeshCounted*> zMeshes;

	private:
		//void DoMinMax(D3DXVECTOR3& min, D3DXVECTOR3& max, D3DXVECTOR3 v); //tillman, used by loadmesh
		//MaloW::Array<MeshStrip*>* LoadMesh(const char* filePath, ObjData* objData); //Helper function for loading the mesh from file.

	public:
		ResourceManager();
		virtual ~ResourceManager();

		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		//Textures
		Texture* CreateTextureFromFile(const char* filePath);
		Texture* CreateCubeTexture(const char* filePath);

		//Mesh
		//MeshCounted* CreateMeshFromFile(const char* filePath);

		//Deletes the texture sent through the parameter. The pointer to the texture sent is automatically set to NULL.
		void DeleteTexture(Texture* &texture);
		//Deletes the mesh sent through the parameter. The pointer to the mesh sent is automatically set to NULL.
		//void DeleteMesh(MeshCounted* &meshCounted);
};

bool ResourceManagerInit(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
ResourceManager* GetResourceManager() throw(...);
bool FreeResourceManager();