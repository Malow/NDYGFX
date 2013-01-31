#ifndef __bthfbx_model_h__
#define __bthfbx_model_h__

#include "BTHFbxCommon.h"

#include "BTHFbxModelPart.h"

extern "C"
{
	class DECLDIR IBTHFbxModel
	{
	public:
		virtual int GetModelPartCount() = 0;

		virtual IBTHFbxModelPart* GetModelPart(int index) = 0;

		virtual const char* GetName() = 0;

		virtual float* GetGeometricOffset() = 0;
		virtual float* GetAnimationTransform() = 0;
	};
}

#endif