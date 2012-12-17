//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Texture-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h.
//--------------------------------------------------------------------------------------------------

#include "ReferenceCounted.h"
#include <d3dx11.h>

class Texture : public ReferenceCounted
{
	private:
		ID3D11ShaderResourceView* zSRV;

	public:
		virtual ~Texture();

	public:
		Texture();
		ID3D11ShaderResourceView* GetSRVPointer() const { return this->zSRV; }
		void SetSRVPointer(ID3D11ShaderResourceView* pointer) { this->zSRV = pointer; }

};
