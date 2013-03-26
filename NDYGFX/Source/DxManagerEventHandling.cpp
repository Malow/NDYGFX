#include "DxManager.h"

void DxManager::HandleDecalEvent(DecalEvent* de)
{
	if(de->IsAdding())
	{
		this->decals.add(de->GetDecal());
	}
	else
	{
		Decal* dec = de->GetDecal();
		for(unsigned int i = 0; i < this->decals.size(); i++)
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
	if(me->IsAdding())
	{
		this->terrains.add(me->GetTerrain());
	}
	else
	{
		Terrain* terrain = me->GetTerrain();
		for(unsigned int i = 0; i < this->terrains.size(); i++)
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
	if(me->IsAdding())
	{
		this->objects.add(me->GetStaticMesh());
	}
	else
	{
		StaticMesh* mesh = me->GetStaticMesh();
		for(unsigned int i = 0; i < this->objects.size(); i++)
		{
			if(this->objects[i] == mesh)
			{
				/*
				delete this->objects.getAndRemove(i);
				mesh = NULL;
				*/
				this->helperThread->PutEvent(new DeleteObjectEvent(this->objects.getAndRemove(i)));
				break;
			}
		}
	}
}

void DxManager::HandleAnimatedMeshEvent(AnimatedMeshEvent* me)
{
	if(me->IsAdding())
	{
		this->animations.add(me->GetAnimatedMesh());
	}
	else
	{
		AnimatedMesh* mesh = me->GetAnimatedMesh();
		for(unsigned int i = 0; i < this->animations.size(); i++)
		{
			if(this->animations[i] == mesh)
			{
				/*
				delete this->animations.getAndRemove(i);
				mesh = NULL;
				*/
				this->helperThread->PutEvent(new DeleteObjectEvent(this->animations.getAndRemove(i)));


				break;
			}
		}
	}
}


void DxManager::HandleFBXEvent(FBXEvent* me)
{
	if(me->IsAdding())
	{
		this->FBXMeshes.add(me->GetFBXMesh());
	}
	else
	{
		FBXMesh* mesh = me->GetFBXMesh();
		for(unsigned int i = 0; i < this->FBXMeshes.size(); i++)
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
	if(le->IsAdding())
	{
		if(le->IsUsingShadows())
		{			
			le->GetLight()->InitShadowMap(this->Dx_Device, this->params.ShadowMapSettings);
			this->lights.add(le->GetLight());
		}
		else
			this->lights.add(le->GetLight());
	}
	else
	{
		Light* light = le->GetLight();
		for(unsigned int i = 0; i < this->lights.size(); i++)
		{
			if(this->lights[i] == light)
			{
				delete this->lights.getAndRemove(i);
				light = NULL;
			}
		}
	}
}

void DxManager::HandleImageEvent(ImageEvent* ie)
{
	if(ie->IsAdding())
		this->images.add(ie->GetImage());
	else
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
	if(ie->IsAdding())
		this->billboards.add(ie->GetBillboard());
	else
	{
		Billboard* billboard = ie->GetBillboard();
		for(unsigned int i = 0; i < this->billboards.size(); i++)
		{
			if(this->billboards[i] == billboard)
			{
				delete this->billboards.getAndRemoveStaySorted(i);
				billboard = NULL;
			}
		}
	}
}

void DxManager::HandleBillboardCollectionEvent(BillboardCollectionEvent* ie)
{
	if(ie->IsAdding())
	{
		this->billboardCollections.add(ie->GetBillboardCollection());
	}
	else
	{
		BillboardCollection* billboardCollection = ie->GetBillboardCollection();
		for(unsigned int i = 0; i < this->billboardCollections.size(); i++)
		{
			if(this->billboardCollections[i] == billboardCollection)
			{
				delete this->billboardCollections.getAndRemoveStaySorted(i);
				billboardCollection = NULL;
			}
		}
	}
}

void DxManager::HandleTextEvent(TextEvent* te)
{
	if(te->IsAdding())
		this->texts.add(te->GetText());
	else
	{
		Text* txt = te->GetText();
		for(unsigned int i = 0; i < this->texts.size(); i++)
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
	if(ie->IsAdding())
		this->waterplanes.add(ie->GetWaterPlane());
	else
	{
		WaterPlane* wp = ie->GetWaterPlane();
		for(unsigned int i = 0; i < this->waterplanes.size(); i++)
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
	case 22:
		if(this->Shader_DeferredGeometryInstanced)	
			this->Shader_DeferredGeometryInstanced->Reload();
		break;
	case 23:
		if(this->Shader_DeferredAnimatedGeometryInstanced)	
			this->Shader_DeferredAnimatedGeometryInstanced->Reload();
		break;
	case 24:
		if(this->Shader_ShadowMapInstanced)	
			this->Shader_ShadowMapInstanced->Reload();
		break;
	case 25:
		if(this->Shader_ShadowMapAnimatedInstanced)	
			this->Shader_ShadowMapAnimatedInstanced->Reload();
		break;
	case 26:
		if(this->Shader_FogEnclosement)	
			this->Shader_FogEnclosement->Reload();
		break;
	case 27:
		if(this->Shader_BillboardInstanced)
			this->Shader_BillboardInstanced->Reload();
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

void DxManager::HandleSkyBoxEvent(SkyBoxEvent* sbe)
{
	if(sbe->IsAdding())
	{
		if(this->skybox)
			delete this->skybox;

		this->skybox = sbe->GetSkyBox();
	}
}