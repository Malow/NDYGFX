#ifndef __bthfbx_model_part_h__
#define __bthfbx_model_part_h__

#include "BTHFbxCommon.h"

#include "BTHFbxMaterial.h"

extern "C"
{
	class DECLDIR IBTHFbxModelPart
	{
	public:
		virtual int GetVertexCount() = 0;

		virtual BTHFBX_VEC3* GetVertexPositionData() = 0;
		virtual BTHFBX_VEC3* GetVertexNormalData() = 0;
		virtual BTHFBX_VEC3* GetVertexTangentData() = 0;
		virtual BTHFBX_VEC2* GetVertexTexCoordData() = 0;
		virtual BTHFBX_BLEND_WEIGHT_DATA* GetVertexBoneWeightData() = 0;
		
		virtual unsigned int GetIndexCount() = 0;
		virtual unsigned int* GetIndexData() = 0;

		virtual bool IsSkinnedModel() = 0;
		virtual IBTHFbxMaterial* GetMaterial() = 0;
	};
}

#endif