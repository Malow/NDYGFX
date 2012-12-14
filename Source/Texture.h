//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Texture-resource class. Used by the ResourceManager.
//--------------------------------------------------------------------------------------------------

#include "ReferenceCounted.h"
#include <d3dx11.h>

class Texture : public ReferenceCounted
{
	public:
		//handle to dx tex.
		ID3D11Texture2D
	public:
		Texture();


};
