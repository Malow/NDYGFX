/*-------------------------------------------------------------------------------------
 *
 * Copyright (c) Blekinge Institute of Technology
 *
 *-------------------------------------------------------------------------------------*/

#ifndef __bthfbx_h__
#define __bthfbx_h__

#include "..\Source\BTHFBX\Include\BTHFbxCommon.h"
#include "..\Source\BTHFBX\Include\BTHFbxScene.h"

extern "C"
{
	class DECLDIR IBTHFbx
	{
	public:
		virtual IBTHFbxScene* GetScene(const char* filename) = 0;

		virtual void UpdateScenes(float deltaTime, bool bEnableAnimation) = 0;
	};

	DECLDIR IBTHFbx* InitBTHFbx();
	DECLDIR void DestroyBTHFbx();
}
#endif