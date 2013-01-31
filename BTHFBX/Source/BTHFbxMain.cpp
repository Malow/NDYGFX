#include <BTHFbx.h>
#include <cstdio>
#include "SceneManager.h"

static IBTHFbx* instance = 0;

DECLDIR IBTHFbx* InitBTHFbx()
{
	if(!instance)
		instance = new SceneManager();
		
	return instance;
}

DECLDIR void DestroyBTHFbx()
{
	SceneManager* sm = static_cast<SceneManager*>(instance);
	if(sm)
	{
		delete sm;
		instance = 0;
	}	
}