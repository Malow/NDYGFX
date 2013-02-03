#ifndef __bthfbx_material_h__
#define __bthfbx_material_h__

#include "BTHFbxCommon.h"

extern "C"
{
	class DECLDIR IBTHFbxMaterial
	{
	public:
		virtual const BTHFBX_VEC3& GetAmbientColor() = 0;
		virtual const BTHFBX_VEC3& GetEmissiveColor() = 0;
		virtual const BTHFBX_VEC3& GetDiffuseColor() = 0;
		virtual const BTHFBX_VEC3& GetSpecularColor() = 0;
		virtual float SetSpecularPower() = 0;
		virtual float SetTransparency() = 0;

		virtual bool HasDiffuseTexture() = 0;
		virtual const char* GetDiffuseTextureFilename() = 0;
		virtual bool HasNormalTexture() = 0;
		virtual const char* GetNormalTextureFilename() = 0;
	};
}

#endif