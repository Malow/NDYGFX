#include "DxManager.h"

void DxManager::HandleDecalEvent(DecalEvent* de)
{
	string msg = de->getMessage();
	if(msg == "Add Decal")
	{
		this->decals.add(de->GetDecal());
	}
	else if(msg == "Delete Decal")
	{
		Decal* dec = de->GetDecal();
		for(int i = 0; i < this->decals.size(); i++)
		{
			if(this->decals[i] == dec)
			{
				delete this->decals.getAndRemove(i);
				dec = NULL;
				break;
			}
		}
	}
}

void DxManager::HandleTerrainEvent(TerrainEvent* me)
{
	string msg = me->getMessage();
	if(msg == "Add Terrain")
	{
		this->terrains.add(me->GetTerrain());
	}
	else if(msg == "Delete Terrain")
	{
		Terrain* terrain = me->GetTerrain();
		for(int i = 0; i < this->terrains.size(); i++)
		{
			if(this->terrains[i] == terrain)
			{
				delete this->terrains.getAndRemove(i);
				terrain = NULL;
			}
		}
	}
}

void DxManager::HandleStaticMeshEvent(StaticMeshEvent* me)
{
	string msg = me->getMessage();
	if(msg == "Add Mesh")
	{
		this->objects.add(me->GetStaticMesh());
	}
	else if(msg == "Delete Mesh")
	{
		StaticMesh* mesh = me->GetStaticMesh();
		for(int i = 0; i < this->objects.size(); i++)
		{
			if(this->objects[i] == mesh)
			{
				delete this->objects.getAndRemove(i);
				mesh = NULL;
				break;
			}
		}
	}
}

void DxManager::HandleAnimatedMeshEvent(AnimatedMeshEvent* me)
{
	string msg = me->getMessage();
	if(msg == "Add AniMesh")
	{
		this->animations.add(me->GetAnimatedMesh());
	}
	else if(msg == "Delete AniMesh")
	{
		AnimatedMesh* mesh = me->GetAnimatedMesh();
		for(int i = 0; i < this->animations.size(); i++)
		{
			if(this->animations[i] == mesh)
			{
				delete this->animations.getAndRemove(i);
				mesh = NULL;
				break;
			}
		}
	}
}


void DxManager::HandleFBXEvent(FBXEvent* me)
{
	string msg = me->getMessage();
	if(msg == "Add FBX")
	{
		this->FBXMeshes.add(me->GetFBXMesh());
	}
	else if(msg == "Delete FBX")
	{
		FBXMesh* mesh = me->GetFBXMesh();
		for(int i = 0; i < this->FBXMeshes.size(); i++)
		{
			if(this->FBXMeshes[i] == mesh)
			{
				delete this->FBXMeshes.getAndRemove(i);
				mesh = NULL;
				break;
			}
		}
	}
}

void DxManager::HandleLightEvent(LightEvent* le)
{
	string msg = le->getMessage();
	if(msg == "Add Light with shadows")
	{
		le->GetLight()->InitShadowMap(this->Dx_Device, this->params.ShadowMapSettings);
		this->lights.add(le->GetLight());
	}
	else if(msg == "Add Light")
	{
		this->lights.add(le->GetLight());
	}
	else if(msg == "Delete Light")
	{
		Light* light = le->GetLight();
		for(int i = 0; i < this->lights.size(); i++)
		{
			if(this->lights[i] == light)
			{
				//delete this->lights.getAndRemove(i);
				light = NULL;
			}
		}
	}
}

void DxManager::HandleImageEvent(ImageEvent* ie)
{
	string msg = ie->getMessage();
	if(msg == "Add Image")
		this->images.add(ie->GetImage());
	else if(msg == "Delete Image")
	{
		Image* img = ie->GetImage();
		for(int i = 0; i < this->images.size(); i++)
		{
			if(this->images[i] == img)
			{
				delete this->images.getAndRemoveStaySorted(i);
				img = NULL;
			}
		}
	}
}

void DxManager::HandleBillboardEvent(BillboardEvent* ie)
{
	string msg = ie->getMessage();
	if(msg == "Add Billboard")
		this->billboards.add(ie->GetBillboard());
	else if(msg == "Delete Billboard")
	{
		Billboard* billboard = ie->GetBillboard();
		for(int i = 0; i < this->billboards.size(); i++)
		{
			if(this->billboards[i] == billboard)
			{
				delete this->billboards.getAndRemoveStaySorted(i);
				billboard = NULL;
			}
		}
	}
}

void DxManager::HandleTextEvent(TextEvent* te)
{
	string msg = te->getMessage();
	if(msg == "Add Text")
		this->texts.add(te->GetText());
	else if(msg == "Delete Text")
	{
		Text* txt = te->GetText();
		for(int i = 0; i < this->texts.size(); i++)
		{
			if(this->texts[i] == txt)
			{
				delete this->texts.getAndRemove(i);
				txt = NULL;
			}
		}
	}
}

void DxManager::HandleWaterPlaneEvent(WaterPlaneEvent* ie)
{
	string msg = ie->getMessage();
	if(msg == "Add WaterPlane")
		this->waterplanes.add(ie->GetWaterPlane());
	else if(msg == "Delete WaterPlane")
	{
		WaterPlane* wp = ie->GetWaterPlane();
		for(int i = 0; i < this->waterplanes.size(); i++)
		{
			if(this->waterplanes[i] == wp)
			{
				delete this->waterplanes.getAndRemove(i);
				wp = NULL;
			}
		}
	}
}

void DxManager::HandleReloadShaders(int shader)
{
	switch (shader) 
	{
	case 0:
		if(this->Shader_ForwardRendering)
			Shader_ForwardRendering->Reload();
		break;
	case 1:
		if(this->Shader_FBX)
			Shader_FBX->Reload();
		break;
	case 3:
		if(this->Shader_Skybox)
			Shader_Skybox->Reload();
		break;
	case 4:
		if(this->Shader_Image)
			Shader_Image->Reload();
		break;
	case 5:
		if(this->Shader_Billboard)
			Shader_Billboard->Reload();
		break;
	case 6:
		if(this->Shader_Text)
			Shader_Text->Reload();
		break;
	case 7:
		if(this->Shader_ShadowMap)
			Shader_ShadowMap->Reload();
		break;
	case 8:
		if(this->Shader_ShadowMapAnimated)
			Shader_ShadowMapAnimated->Reload();
		break;
	case 9:
		if(this->Shader_DeferredGeometry)
			Shader_DeferredGeometry->Reload();
		break;
	case 10:
		if(this->Shader_TerrainEditor)
			Shader_TerrainEditor->Reload();
		break;
	case 11:
		if(this->Shader_DeferredLightning)
			Shader_DeferredLightning->Reload();
		break;
	case 12:
		if(this->Shader_InvisibilityEffect)
			Shader_InvisibilityEffect->Reload();
		break;
	case 13:
		if(this->Shader_DeferredQuad)
			Shader_DeferredQuad->Reload();
		break;
	case 14:
		if(this->Shader_DeferredTexture)
			Shader_DeferredTexture->Reload();
		break;
	case 15:
		if(this->Shader_DeferredAnimatedGeometry)
			Shader_DeferredAnimatedGeometry->Reload();
		break;
	case 16:
		if(this->Shader_DeferredGeoTranslucent)
			Shader_DeferredGeoTranslucent->Reload();
		break;
	case 17:
		if(this->Shader_DeferredPerPixelTranslucent)
			Shader_DeferredPerPixelTranslucent->Reload();
		break;
	case 18:
		//if(this->Shader_Fxaa)		 FXAA Shader doesnt like being reloaded :(
		//Shader_Fxaa->Reload();
		break;
	case 19:
		if(this->Shader_Decal)		
			Shader_Decal->Reload();
		break;
	case 20:
		if(this->Shader_ShadowMapBillboardInstanced)	
			Shader_ShadowMapBillboardInstanced->Reload();
	case 21:
		if(this->Shader_ShadowMapFBX)	
			Shader_ShadowMapFBX->Reload();
		break;
	}
}


void DxManager::HandleSetCameraEvent(SetCameraEvent* ev)
{
	Camera* cam = ev->GetCamera();
	Camera* oldCam = this->camera;
	this->camera = cam;
	delete oldCam;
	oldCam = NULL;
	this->DelayGettingCamera = false;
}