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

class Texture : public ReferenceCounted
{
	private:
		ID3D11ShaderResourceView* zSRV;

	public: //public so that the resource manager can delete it in case it doesn't delete itself.
		virtual ~Texture();

	public:
		Texture();
		Texture(ID3D11ShaderResourceView* SRV);

		ID3D11ShaderResourceView* GetSRVPointer() const { return this->zSRV; }
		void SetSRVPointer(ID3D11ShaderResourceView* pointer) { this->zSRV = pointer; }

};
