//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Manager for handling resources. 
//	Resources include:
//		Mesh(strips) - MeshResource.h.
//		Textures - TextureResource.h. //**Tillman**
//		Buffers - BufferResource.h
//	Requirements: DirectX device & device context.
//--------------------------------------------------------------------------------------------------
#pragma once

//#include "DirectX.h"
#include "TextureResource.h"
//#include "MeshResource.h" //**TILLMAN circkulär include - Object3d.h**
//#include "ObjLoader.h"
#include "ObjectDataResource.h"
//#include "StaticMesh.h" //Circulär include - TILLMAN
//#include "AnimatedMesh.h"

#include "BufferResource.h"
#include <string>
#include <map>

#include "Array.h"
#include "MaloW.h"
#include "MaloWFileDebug.h"



class ResourceManager 
{
	private:
		ID3D11Device*			gDevice;
		ID3D11DeviceContext*	gDeviceContext;

	private:
		std::map<std::string, TextureResource*> zTextureResources;
		std::map<std::string, ObjectDataResource*>			zObjectDataResources;
		//std::map<std::string, ObjData*>			zObjectDataResources;
		//std::map<std::string, MeshCounted*>	zMeshResources;
		std::map<std::string, BufferResource*>	zBufferResources;
		HANDLE mutex;

	private:
		//void DoMinMax(D3DXVECTOR3& min, D3DXVECTOR3& max, D3DXVECTOR3 v); //tillman, used by loadmesh
		//MaloW::Array<MeshStrip*>* LoadMesh(const char* filePath, ObjData* objData); //Helper function for loading the mesh from file.

	public:
		ResourceManager();
		virtual ~ResourceManager();

		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		

		//CREATE
		//Textures
		/*	Creates a texture resource from file with:
			BindFlags = D3D11_BIND_SHADER_RESOURCE;
			Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			and returns a pointer to it.
		*/
		TextureResource* CreateTextureResourceFromFile(const char* filePath);
		/*	Creates a cube texture resource from file with:
			BindFlags = D3D11_BIND_SHADER_RESOURCE;
			Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			and returns a pointer to it.
			*/
		TextureResource* CreateCubeTextureResourceFromFile(const char* filePath);
		/*	Creates a texture resource with the provided identification and D3DX11_IMAGE_LOAD_INFO and returns a pointer to it.	*/
		/*TextureResource* CreateTextureResource(const char* id, D3DX11_IMAGE_LOAD_INFO loadInfo);
		/*	Checks if the specified filepath has been used to create a texture resource already. Otherwise returns NULL.	*/
		/*TextureResource* HasTextureResource(const char* filePath);
		/*	Saves the pointer of the specified resource. */
		/*void SetTextureResource(TextureResource* textureResource);
		/*	Deletes the texture resource sent through the parameter. The pointer to the texture sent is automatically set to NULL.	*/
		void DeleteTextureResource(TextureResource* &textureResource);


		//Mesh
		//MeshCounted* CreateMeshFromFile(const char* filePath);
		/*	Deletes the mesh resource sent through the parameter. The pointer to the mesh resource sent is automatically set to NULL.	*/
		//void DeleteMesh(MeshResource* &meshResource);


		//Object data
		ObjectDataResource* LoadObjectDataResourceFromFile(const char* filePath);
		void UnloadObjectDataResource(const char* filePath);

		//Buffer
		/*	
			Creates a buffer resource and returns a pointer to it. 
			const char* fileName - used as resource identification. Using the name of the file containing the vertex data is recommended.
			BUFFER_INIT_DESC bufferInitDesc - buffer data (a description) of the buffer to create.
		*/
		BufferResource* CreateBufferResource(const char* fileName, BUFFER_INIT_DESC bufferInitDesc);
		/*	Compares the parameter with the buffer names (IDs) using partial match and returns true if an identical match is found.	*/
		bool HasBuffer(const char* fileName);
		/*	Deletes	the buffer resource sent through the parameter. The pointer to the buffer resource is automatically set to NULL.	*/
		void DeleteBufferResource(BufferResource* &bufferResource);


		//Preloading ** TILLMAN
		//void PreloadObjects(unsigned int nrOfObjects, char const* const* const objectFileNames);
};

bool ResourceManagerInit(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
ResourceManager* GetResourceManager() throw(...);
bool FreeResourceManager();