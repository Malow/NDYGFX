//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Texture-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h.
//--------------------------------------------------------------------------------------------------

#pragma once

#include "ReferenceCounted.h"
#include <d3dx11.h>
#include <string>
using namespace std;

class Texture : public ReferenceCounted
{
	private:
		string						zFilePath;
		ID3D11ShaderResourceView*	zSRV;

	public: //public so that the resource manager can delete it in case it doesn't delete itself. **tillman - private, tillåta minnesläcka, dock fortsätta skriva ut varning**
		virtual ~Texture();

	public:
		Texture();
		Texture(ID3D11ShaderResourceView* SRV);
		Texture(string filePath, ID3D11ShaderResourceView* SRV);

		const string& GetName() const { return this->zFilePath; }
		ID3D11ShaderResourceView* GetSRVPointer() const { return this->zSRV; }
		void SetSRVPointer(ID3D11ShaderResourceView* pointer) { this->zSRV = pointer; }

};
