#ifndef __bthfbx_curve_h__
#define __bthfbx_curve_h__

#include "BTHFbxCommon.h"

extern "C"
{
	class DECLDIR IBTHFbxCurve
	{
	public:
		virtual const char* GetName() = 0;

		virtual unsigned int GetNumControlPoints() = 0;
		
		virtual const BTHFBX_VEC3& GetControlPoint(unsigned int index) = 0;
	};
}

#endif