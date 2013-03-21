//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Manager for handling resources. 
//	Resources include:
//		Mesh strips - MeshStripsResource.h.
//		Textures - TextureResource.h.
//		Buffers - BufferResource.h
//	Requirements: DirectX device & device context.
//--------------------------------------------------------------------------------------------------
#pragma once

#include "TextureResource.h"
#include "ObjectDataResource.h"
#include "MeshStripsResource.h" 

#include "BufferResource.h"
#include <string>
#include <map>
#include <vector>

#include "Array.h"
#include "MaloW.h"
#include "MaloWFileDebug.h"



class ResourceManager 
{
	private:
		ID3D11Device*			gDevice;
		ID3D11DeviceContext*	gDeviceContext;

	private:
		ID3D11DeviceContext*	zDeferredContext;
	    HANDLE mutex;

		std::map<std::string, ObjectDataResource*>	zObjectDataResources; //Used by meshstripresources

		std::map<std::string, TextureResource*>		zTextureResources;
		std::map<std::string, BufferResource*>		zBufferResources;
		std::map<std::string, MeshStripsResource*>	zMeshStripsResources;
		std::map<std::string, float>					zMeshHeights; //Used by meshstripresources

	private:
		void DoMinMax(D3DXVECTOR3& min, D3DXVECTOR3& max, D3DXVECTOR3 v); //used by LoadMeshStrips
		MaloW::Array<MeshStrip*>* LoadMeshStrips(const char* filePath, ObjData* objData, float& height); //Helper function for loading the mesh strips from file.

		//Object data
		ObjectDataResource* LoadObjectDataResourceFromFile(const char* filePath);
		void UnloadObjectDataResource(const char* filePath);


	public:
		ResourceManager();
		virtual ~ResourceManager();

		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		

		//Preloading
		/*
			Texture resources are created with the default format (CreateTextureResourceFromFile(..)).
			Buffer resources are not supported.
			Supported resources are objectDataResources(.obj & .ani), Texture resources(default format)(.png & .dds).
		*/
		void PreLoadResources(unsigned int nrOfResources, std::vector<string> resourcesFileNames);
		


		//Textures
		/*	Creates a texture resource from file with:
			BindFlags = D3D11_BIND_SHADER_RESOURCE;
			Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			and returns a pointer to it.
			Is the default format used.
		*/
		TextureResource* CreateTextureResourceFromFile(const char* filePath, bool generateMipMap = false);
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
		/*	Returns a pointer to the found texture resource. Returns NULL if no texture resource was found.	*/
		const TextureResource* GetTextureResourcePointer(const char* id) const;
		/*	Deletes the texture resource sent through the parameter. The pointer to the texture sent is automatically set to NULL.	*/
		void DeleteTextureResource(TextureResource* &textureResource);



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



		//MeshStrips
		/*	Creates and returns a MeshStripResource from an .obj-file. Height of the mesh is returned through the second parameter.	*/
		MeshStripsResource* CreateMeshStripsResourceFromFile(const char* filePath, float& height);
		/*	Compares the parameter with the mesh strips resource names (IDs) using exact match and returns true if an identical match is found.
			OBS! fileName ending in .ani is supported but may be slow due to hard drive read access,	
			please consider using .obj instead.		*/
		bool HasMeshStripsResource(const char* fileName);
		/*	Deletes the MeshStripsResource sent through the parameter. The pointer to MeshStripsResource is automatically set to NULL.	*/
		void DeleteMeshStripsResource(MeshStripsResource* &meshStripsResource);


};

bool ResourceManagerInit(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
ResourceManager* GetResourceManager() throw(...);
bool FreeResourceManager();