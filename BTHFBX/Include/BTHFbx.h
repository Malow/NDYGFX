/*-------------------------------------------------------------------------------------
 *
 * Copyright (c) Blekinge Institute of Technology
 *
 *-------------------------------------------------------------------------------------*/

#ifndef __bthfbx_h__
#define __bthfbx_h__

#include "BTHFbxCommon.h"
#include "BTHFbxScene.h"
#include "..\..\NDYGFX\Include\Vector.h"

extern "C"
{
	class DECLDIR IBTHFbx
	{
	public:
		virtual IBTHFbxScene* GetScene(const char* filename, Vector3& minPos, Vector3& maxPos) = 0;
	};

	DECLDIR IBTHFbx* InitBTHFbx();
	DECLDIR void DestroyBTHFbx();
}
#endif