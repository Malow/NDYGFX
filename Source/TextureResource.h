//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Texture-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h.
//--------------------------------------------------------------------------------------------------

#pragma once

#include "ReferenceCounted.h"
#include "DirectX.h"
#include <d3dx11.h>
#include <string>
using namespace std;

class TextureResource : public ReferenceCounted
{
	private:
		string						zFilePath;
		ID3D11ShaderResourceView*	zSRV;

	private:
		virtual ~TextureResource();

	public:
		TextureResource();
		TextureResource(ID3D11ShaderResourceView* SRV);
		TextureResource(string filePath, ID3D11ShaderResourceView* SRV);

		const string& GetName() const { return this->zFilePath; }
		ID3D11ShaderResourceView* GetSRVPointer() const { return this->zSRV; }
		void SetSRVPointer(ID3D11ShaderResourceView* pointer) { this->zSRV = pointer; }

};
