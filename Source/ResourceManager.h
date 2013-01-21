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
#include "ObjLoader.h"

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
		std::map<std::string, ObjData*>			zObjectDataResources;
		//std::map<std::string, MeshCounted*>	zMeshResources;
		std::map<std::string, BufferResource*>	zBufferResources;

	private:
		//void DoMinMax(D3DXVECTOR3& min, D3DXVECTOR3& max, D3DXVECTOR3 v); //tillman, used by loadmesh
		//MaloW::Array<MeshStrip*>* LoadMesh(const char* filePath, ObjData* objData); //Helper function for loading the mesh from file.

	public:
		ResourceManager();
		virtual ~ResourceManager();

		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		

		//CREATE
		//Textures
		/*	Creates a texture resource from file and returns a pointer to it.	*/
		TextureResource* CreateTextureResourceFromFile(const char* filePath);
		/*	Creates a cube texture resource from file and returns a pointer to it.	*/
		TextureResource* CreateCubeTextureResourceFromFile(const char* filePath);
		/*	Deletes the texture resource sent through the parameter. The pointer to the texture sent is automatically set to NULL.	*/
		void DeleteTextureResource(TextureResource* &textureResource);


		//Mesh
		//MeshCounted* CreateMeshFromFile(const char* filePath);
		/*	Deletes the mesh resource sent through the parameter. The pointer to the mesh resource sent is automatically set to NULL.	*/
		//void DeleteMesh(MeshResource* &meshResource);


		//Object data
		ObjData* LoadObjectDataFromFile(const char* filePath);
		void SetObjectData(const char* filePath, ObjData* objectData);
		void UnloadObjectData(const char* filePath);

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
};

bool ResourceManagerInit(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
ResourceManager* GetResourceManager() throw(...);
bool FreeResourceManager();