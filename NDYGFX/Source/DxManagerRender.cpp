#include "DxManager.h"

#include "DxManagerDebugging.h"

HRESULT DxManager::Update(float)
{
	// update subsystems
	//ps.update(deltaTime);
	//this->camera->Update(deltaTime);				// Moved to Life below to counter update spasms due to thread-collision



	return S_OK;
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
		break;
	}
}


void DxManager::SetCamera(SetCameraEvent* ev)
{
	
	Camera* cam = ev->GetCamera();
	Camera* oldCam = this->camera;
	this->camera = cam;
	delete oldCam;
	oldCam = NULL;
	this->DelayGettingCamera = false;
}

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

void DxManager::HandleMeshEvent(MeshEvent* me)
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

void DxManager::Life()
{
	{
		//Engine Start Splash screen.
		Image* img = new Image(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->params.WindowWidth, (float)this->params.WindowHeight));
		TextureResource* tex = NULL;
		tex = GetResourceManager()->CreateTextureResourceFromFile("Media/LoadingScreen/StartingSplash.png");
		img->SetTexture(tex);
		this->images.add(img);

		while(!this->StartRender)
		{
			this->Render();
			this->framecount++;
		}
		this->DeleteImage(img);
		img = NULL;
		Sleep(100);
	}

	while(this->stayAlive)
	{
		while(MaloW::ProcessEvent* ev = this->PeekEvent())
		{
			if(dynamic_cast<RendererEvent*>(ev) != NULL)
			{
				string msg = ((RendererEvent*)ev)->getMessage();

				// DecalEvent
				if(dynamic_cast<DecalEvent*>(ev) != NULL)
				{
					this->HandleDecalEvent((DecalEvent*)ev);
				}

				// ReloadShaderEvent
				if(ReloadShaderEvent* rse = dynamic_cast<ReloadShaderEvent*>(ev))
					this->HandleReloadShaders(rse->GetShader());
				
				//ChangeShadowQualityEvent
				if(ChangeShadowQualityEvent* csqe = dynamic_cast<ChangeShadowQualityEvent*>(ev))
				{
					this->params.ShadowMapSettings = csqe->GetQuality();
					if(this->csm)
					{
						this->useShadow = true;
						this->csm->ResizeShadowmaps(this->Dx_Device, this->params.ShadowMapSettings);
					}
				}

				//WaterPlaneEvent
				if(dynamic_cast<FBXEvent*>(ev) != NULL)
				{
					this->HandleFBXEvent((FBXEvent*)ev);
				}

				//WaterPlaneEvent
				if(dynamic_cast<WaterPlaneEvent*>(ev) != NULL)
				{
					this->HandleWaterPlaneEvent((WaterPlaneEvent*)ev);
				}

				//SetCameraEvent
				if(dynamic_cast<SetCameraEvent*>(ev) != NULL)
				{
					this->SetCamera((SetCameraEvent*)ev);
				}

				// ResizeEvent
				if(dynamic_cast<ResizeEvent*>(ev) != NULL)
				{
					this->ResizeRenderer((ResizeEvent*)ev);
				}

				//TerrainEvent
				else if(dynamic_cast<TerrainEvent*>(ev) != NULL)
				{
					this->HandleTerrainEvent((TerrainEvent*)ev);
				}

				// MeshEvent
				else if(dynamic_cast<MeshEvent*>(ev) != NULL)
				{
					this->HandleMeshEvent((MeshEvent*)ev);
				}

				// LightEvent
				else if(dynamic_cast<LightEvent*>(ev) != NULL)
				{
					this->HandleLightEvent((LightEvent*)ev);
				}

				// ImageEvent
				else if(dynamic_cast<ImageEvent*>(ev) != NULL)
				{
					this->HandleImageEvent((ImageEvent*)ev);
				}

				// BillboardEvent
				else if(dynamic_cast<BillboardEvent*>(ev) != NULL)
				{
					this->HandleBillboardEvent((BillboardEvent*)ev);
				}

				// TextEvent
				else if(dynamic_cast<TextEvent*>(ev) != NULL)
				{
					this->HandleTextEvent((TextEvent*)ev);
				}
			}
			delete ev;
		}
		this->camera->Update(this->Timer - this->LastCamUpdate);

		this->LastCamUpdate = this->Timer;
		this->Render();
		this->framecount++;
	}
}

void DxManager::RenderParticles()
{
	/*
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	//Matrixes
	D3DXMATRIX world, view, proj, wvp;
	view = this->camera->GetViewMatrix();
	proj = this->camera->GetProjectionMatrix();

	// Draw particleEffects
	MaloW::Array<ParticleMesh*>* psMeshes = this->ps.getMeshes();
	for(int i = 0; i < psMeshes->size(); i++)
	{
		Object3D* obj = this->createParticleObject(psMeshes->get(i));
		
		// Set matrixes
		world = psMeshes->get(i)->GetWorldMatrix();
		wvp = world * view * proj;
		Shader_Particle->SetMatrix("WVP", wvp);
		Shader_Particle->SetMatrix("worldMatrix", world);

		Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());

		Buffer* verts = obj->GetVertBuff();
		if(verts)
			verts->Apply();

		if(ID3D11ShaderResourceView* texture = obj->GetTexture())
		{
			Shader_Particle->SetBool("textured", true);
			Shader_Particle->SetResource("tex2D", texture);
		}
		else
			Shader_Particle->SetBool("textured", false);

		Buffer* inds = obj->GetIndsBuff();
		if(inds)
			inds->Apply();

		Shader_Particle->Apply(0);

		// draw
		if(inds)
			Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
		else
			Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
	}

	// Unbind resources:
	Shader_Particle->SetResource("tex2D", NULL);
	Shader_Particle->Apply(0);

	delete psMeshes;
	*/
}

void DxManager::RenderShadowMap()
{
	
	// If special circle is used
	if(this->specialCircleParams.x) //if inner radius > 0, then send/set data
	{
		this->Shader_DeferredLightning->SetFloat4("dataPPHA", this->specialCircleParams);
	}

	// Generate and send shadowmaps to the main-shader
	//EDIT 2013-01-23 by Tillman - Added transparancy.
	if(!this->lights.size())
	{
		for (int l = 0; l < this->lights.size(); l++)
		{
			Dx_DeviceContext->OMSetRenderTargets(0, 0, this->lights[l]->GetShadowMapDSV());
			D3D11_VIEWPORT wp = this->lights[l]->GetShadowMapViewPort();
			Dx_DeviceContext->RSSetViewports(1, &wp);
			Dx_DeviceContext->ClearDepthStencilView(this->lights[l]->GetShadowMapDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			//Static meshes
			for(int i = 0; i < this->objects.size(); i++)
			{
				if(!this->objects[i]->IsUsingInvisibility() && !this->objects[i]->GetDontRenderFlag())
				{
					MaloW::Array<MeshStrip*>* strips = this->objects[i]->GetStrips();
					D3DXMATRIX wvp = this->objects[i]->GetWorldMatrix() * this->lights[l]->GetViewProjMatrix();
					this->Shader_ShadowMap->SetMatrix("lightWVP", wvp);
				
					for(int u = 0; u < strips->size(); u++)
					{
						Object3D* obj = strips->get(u)->GetRenderObject();
						
						//Vertex data
						this->Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());
						Buffer* verts = obj->GetVertBuff();
						Buffer* inds = obj->GetIndsBuff();
						if(verts)
						{
							verts->Apply();
						}
						if(inds)
						{
							inds->Apply();
						}

						//Texture
						if(obj->GetTextureResource() != NULL)
						{
							if(obj->GetTextureResource()->GetSRVPointer() != NULL)
							{
								this->Shader_ShadowMap->SetResource("diffuseMap", obj->GetTextureResource()->GetSRVPointer());
								this->Shader_ShadowMap->SetBool("textured", true);
							}
							else
							{
								this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
								this->Shader_ShadowMap->SetBool("textured", false);
							}
						}
						else
						{
							this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
							this->Shader_ShadowMap->SetBool("textured", false);
						}

						//Apply to shader
						this->Shader_ShadowMap->Apply(0);

						//Draw
						if(inds)
						{
							Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
						}
						else if(verts)
						{
							Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
						}
						else
						{
							MaloW::Debug("WARNING: DxManagerRender: RenderShadowMap(): Both vertex and indexbuffers were NULL.");
						}
					}
				}
			}
		
			//Animated meshes
			for(int i = 0; i < this->animations.size(); i++)
			{
				if(!this->animations[i]->IsUsingInvisibility() && !this->animations[i]->GetDontRenderFlag())
				{
					KeyFrame* one = NULL;
					KeyFrame* two = NULL;
					float t = 0.0f;
					this->animations[i]->SetCurrentTime(this->Timer * 1000.0f); //Timer is in seconds.
					this->animations[i]->GetCurrentKeyFrames(&one, &two, t);
					MaloW::Array<MeshStrip*>* stripsOne = one->meshStripsResource->GetMeshStripsPointer();
					MaloW::Array<MeshStrip*>* stripsTwo = two->meshStripsResource->GetMeshStripsPointer();

					//Set shader data (per object)
					this->Shader_ShadowMapAnimated->SetFloat("t", t);
					D3DXMATRIX wvp = this->animations[i]->GetWorldMatrix() * this->lights[l]->GetViewProjMatrix();
					this->Shader_ShadowMapAnimated->SetMatrix("LightWVP", wvp); 

					for(int u = 0; u < stripsOne->size(); u++)  //**Tillman todo - indices?**
					{
						//Set shader data per strip
						Object3D* objOne = stripsOne->get(u)->GetRenderObject();
						Object3D* objTwo = stripsTwo->get(u)->GetRenderObject();

						this->Dx_DeviceContext->IASetPrimitiveTopology(objOne->GetTopology()); 

						Buffer* vertsOne = objOne->GetVertBuff();
						Buffer* vertsTwo = objTwo->GetVertBuff();

						ID3D11Buffer* vertexBuffers [] = {vertsOne->GetBufferPointer(), vertsTwo->GetBufferPointer()};
						UINT strides [] = {sizeof(Vertex), sizeof(Vertex)};
						UINT offsets [] = {0, 0};

						this->Dx_DeviceContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);

						//Apply shader
						this->Shader_ShadowMapAnimated->Apply(0);

						//Draw
						this->Dx_DeviceContext->Draw(vertsOne->GetElementCount(), 0);
					}
				}
			}
		
			D3DXMATRIX lvp = this->lights[l]->GetViewProjMatrix();
		
		
			// Forward
			//this->Shader_ForwardRendering->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsMatrix(l, "lights", "LightViewProj", lvp);
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat4(l, "lights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPosition(), 1));
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat4(l, "lights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColor(), 1));
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat(l, "lights", "LightIntensity", this->lights[l]->GetIntensity());
		

			// For deferred:
			this->Shader_DeferredLightning->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsMatrix(l, "lights", "LightViewProj", lvp);
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat4(l, "lights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPositionD3DX(), 1));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat4(l, "lights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColorD3DX(), 1));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat(l, "lights", "LightIntensity", this->lights[l]->GetIntensity());
		
		
			// For deferred quad:
			//this->Shader_DeferredQuad->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsMatrix(l, "lights", "LightViewProj", lvp);
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat4(l, "lights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPosition(), 1));
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat4(l, "lights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColor(), 1));
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat(l, "lights", "LightIntensity", this->lights[l]->GetIntensity());
		
		}
	}
	
	float PCF_SIZE = (float)this->params.ShadowMapSettings + 1;
	float PCF_SQUARED = 1 / (PCF_SIZE * PCF_SIZE);

	/*
	// Forward
	this->Shader_ForwardRendering->SetFloat("PCF_SIZE", PCF_SIZE);
	this->Shader_ForwardRendering->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);
	this->Shader_ForwardRendering->SetFloat("SMAP_DX", 1.0f / (256 * pow(2.0f, this->params.ShadowMapSettings/2)));
	this->Shader_ForwardRendering->SetFloat("NrOfLights", (float)this->lights.size());
	*/

	
	// Deferred:
	this->Shader_DeferredLightning->SetFloat("SMAP_DX", 1.0f / (256.0f * pow(2.0f, this->params.ShadowMapSettings / 2.0f)));
	this->Shader_DeferredLightning->SetFloat("PCF_SIZE", PCF_SIZE);
	this->Shader_DeferredLightning->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);
	//this->Shader_DeferredLightning->SetFloat("SMAP_DX", 1.0f / 256.0f);
	this->Shader_DeferredLightning->SetFloat("NrOfLights", (float)this->lights.size());
	
	/*
	// for deferred quad:
	this->Shader_DeferredQuad->SetFloat("PCF_SIZE", PCF_SIZE);
	this->Shader_DeferredQuad->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);
	this->Shader_DeferredQuad->SetFloat("SMAP_DX", 1.0f / (256 * pow(2.0f, this->params.ShadowMapSettings/2)));
	this->Shader_DeferredQuad->SetFloat("NrOfLights", (float)this->lights.size());
	*/
}


void DxManager::RenderImages()
{
	/*
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	*/
	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	

	for(int i = 0; i < this->images.size(); i++)
	{
		Image* img = this->images[i];
		// if Convert from screenspace is needed, which it isnt.
		this->Shader_Image->SetFloat("posx", (img->GetPosition().x / this->params.WindowWidth) * 2 - 1);
		this->Shader_Image->SetFloat("posy", 2 - (img->GetPosition().y / this->params.WindowHeight) * 2 - 1);
		this->Shader_Image->SetFloat("dimx", (img->GetDimensions().x / this->params.WindowWidth) * 2);
		this->Shader_Image->SetFloat("dimy", -(img->GetDimensions().y / this->params.WindowHeight) * 2);
		this->Shader_Image->SetFloat("opacity", img->GetOpacity());
		
		/*// if -1 to 1
		this->Shader_Image->SetFloat("posx", img->GetPosition().x);
		this->Shader_Image->SetFloat("posy", img->GetPosition().y);
		this->Shader_Image->SetFloat("dimx", img->GetDimensions().x);
		this->Shader_Image->SetFloat("dimy", img->GetDimensions().y);
		*/
		if(img->GetTexture() != NULL)
		{
			this->Shader_Image->SetResource("tex2D", img->GetTexture()->GetSRVPointer());
		}
		this->Shader_Image->Apply(0);
		this->Dx_DeviceContext->Draw(1, 0);
	}
	this->Shader_Image->SetResource("tex2D", NULL);
	this->Shader_Image->Apply(0);
}
void DxManager::RenderBillboards()
{
	if(this->billboards.size() != 0)
	{
		this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
		this->Shader_Billboard->SetFloat3("g_CameraPos", this->camera->GetPositionD3DX());
		this->Shader_Billboard->SetMatrix("g_CamViewProj", this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
		this->Shader_Billboard->SetFloat("FarClip", this->params.FarClip);

		for(int i = 0; i < this->billboards.size(); i++)
		{
			Billboard* billboard = this->billboards[i];
		
			//Set bill board variables
			this->Shader_Billboard->SetFloat3("g_bb_Position", billboard->GetPositionD3DX()); 
			this->Shader_Billboard->SetFloat2("g_bb_BillboardSize", billboard->GetSizeD3DX());
			this->Shader_Billboard->SetFloat4("g_bb_Color", billboard->GetColorD3DX());

			if(billboard->GetTextureResource() != NULL)
			{
				if(billboard->GetTextureResource()->GetSRVPointer())
				{
					this->Shader_Billboard->SetResource("g_bb_DiffuseMap", billboard->GetTextureResource()->GetSRVPointer());
					this->Shader_Billboard->SetBool("g_bb_IsTextured", true);
				}
				else
				{
					this->Shader_Billboard->SetResource("g_bb_DiffuseMap", NULL);
					this->Shader_Billboard->SetBool("g_bb_IsTextured", false);
				}
			}
			else
			{
				this->Shader_Billboard->SetResource("g_bb_DiffuseMap", NULL);
				this->Shader_Billboard->SetBool("g_bb_IsTextured", false);
			}
			//Apply them
			this->Shader_Billboard->Apply(0);

			//Ignore vertex input

			//Draw one vertex
			this->Dx_DeviceContext->Draw(1, 0);
		}

		//Unbind resources
		this->Shader_Billboard->SetResource("g_bb_DiffuseMap", NULL);
		this->Shader_Billboard->Apply(0);
	}
}
void DxManager::RenderBillboardsInstanced()
{
	if(this->instancingHelper->GetNrOfBillboards() > 0)
	{
		//Sort, create instance groups and update buffer before rendering
		this->instancingHelper->PreRenderBillboards();



		//Draw billboards
		unsigned int strides[1];
		unsigned int offsets[1];
		ID3D11Buffer* bufferPointers[1];

		// Set the buffer strides.
		strides[0] = sizeof(Vertex);
		// Set the buffer offset.
		offsets[0] = 0;
		// Set the array of pointers to the vertex and instance buffers.
		bufferPointers[0] = this->instancingHelper->GetBillboardInstanceBuffer();	

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		this->Dx_DeviceContext->IASetVertexBuffers(0, 1, bufferPointers, strides, offsets);
		this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		// Set global variables per frame
		this->Shader_BillboardInstanced->SetFloat3("g_CameraPos", this->camera->GetPositionD3DX());
		this->Shader_BillboardInstanced->SetMatrix("g_CamViewProj", this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
		this->Shader_BillboardInstanced->SetFloat("g_FarClip", this->params.FarClip);

		// Set global variables per instance group
		for(unsigned int i = 0; i < this->instancingHelper->GetNrOfBillboardGroups(); ++i)
		{
			if(this->instancingHelper->GetBillboardGroup(i).s_SRV != NULL) 
			{
				this->Shader_BillboardInstanced->SetResource("g_bb_DiffuseMap", this->instancingHelper->GetBillboardGroup(i).s_SRV);
				this->Shader_BillboardInstanced->SetBool("g_bb_IsTextured", true);
			}
			else
			{
				this->Shader_BillboardInstanced->SetResource("g_bb_DiffuseMap", NULL);
				this->Shader_BillboardInstanced->SetBool("g_bb_IsTextured", false);
			}

			//Apply pass and input layout
			this->Shader_BillboardInstanced->Apply(0);

			//Draw
			int instanceCount = this->instancingHelper->GetBillboardGroup(i).s_Size;
			int startInstanceLocation = this->instancingHelper->GetBillboardGroup(i).s_StartLocation;
			this->Dx_DeviceContext->DrawInstanced(instanceCount, 1, startInstanceLocation, 0); //**TILLMAN, parameters inversed**
			
			//Debug data
			this->NrOfDrawCalls++;
		}
		//Debug data
		this->NrOfDrawnVertices += 4 * this->instancingHelper->GetNrOfBillboards();
		//Reset counter (nrofbillboards)
		this->instancingHelper->PostRenderBillboards();
	}
}

void DxManager::RenderText()
{
	/*
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	*/
	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	for(int i = 0; i < this->texts.size(); i++)
	{
		Text* txt = this->texts[i];
		// if Convert from screen space is needed, which it is
		this->Shader_Text->SetFloat("posx", (txt->GetPosition().x / this->params.WindowWidth) * 2 - 1);
		this->Shader_Text->SetFloat("posy", 2 - (txt->GetPosition().y / this->params.WindowHeight) * 2 - 1);

		this->Shader_Text->SetFloat("size", txt->GetSize());
		this->Shader_Text->SetFloat("windowWidth", (float)this->params.WindowWidth);
		this->Shader_Text->SetFloat("windowHeight", (float)this->params.WindowHeight);
		
		// Im only using ASCI 30 - 100, to reduce data sent I only send those 70 as 0-70. Therefor the t = 30 and t - 30
		static bool once = true;
		if(once)
		{
			for(int t = 30; t < 100; t++)
			{
				this->Shader_Text->SetFloatAtIndex(t - 30, "charTex", (float)(int)txt->GetFont()->charTexCoords[t]);
				this->Shader_Text->SetFloatAtIndex(t - 30, "charWidth", (float)(int)txt->GetFont()->charWidth[t]);
			}
			once = false;
		}


		this->Shader_Text->SetResource("tex2D", txt->GetFont()->textureResource->GetSRVPointer());


		string drawText = txt->GetText();

		//
		if(drawText.size() > 40)
			drawText = drawText.substr(0, 40);

		this->Shader_Text->SetFloat("NrOfChars", (float)drawText.size());
		for(int t = 0; t < (int)drawText.size(); t++)
		{
			// Im only using ASCI 30 - 100, to reduce data sent I only send those 70 as 0-70. Therefor the -30
			this->Shader_Text->SetFloatAtIndex(t, "text", (float)(int)drawText[t] - 30);
		}

		//

		/*
		bool go = true;
		do
		{
			int chars = 40;
			if(drawText.size() <= chars)
			{
				go = false;
				this->Shader_Text->SetFloat("NrOfChars", (float)drawText.size());

				for(int t = 0; t < drawText.size(); t++)
				{
					this->Shader_Text->SetFloatAtIndex(t, "text", (float)(int)drawText[t]);
				}
			}
			else
			{
				string temp = drawText.substr(0, 40);
				drawText = drawText.substr(41);


				this->Shader_Text->SetFloat("NrOfChars", (float)temp.size());

				for(int t = 0; t < temp.size(); t++)
				{
					this->Shader_Text->SetFloatAtIndex(t, "text", (float)(int)temp[t]);
				}
			}
		}
		while(go);

		*/


		this->Shader_Text->Apply(0);

		this->Dx_DeviceContext->Draw(1, 0);
	}
	this->Shader_Text->SetResource("tex2D", NULL);
	this->Shader_Text->Apply(0);
}

void DxManager::RenderCascadedShadowMap()
{
	//EDIT 2013-01-23 by Tillman - Added transparency.

	//Reset counters
	int currentRenderedTerrainShadows = 0;
	int currentRenderedMeshShadows = 0;

	if(this->useSun && this->useShadow) //* TILLMAN - sun ska också inte ligga här?*
	{
		D3DXMATRIX wvp;
		D3DXMatrixIdentity(&wvp);
		//**TILLMAN TODO: ta bort  DX-sakerna då detta görs av RenderCascadedShadowmapBillboard()?. check what cascade the object is in, object->IsIncascade(s)(indices)**
		for (int l = 0; l < this->csm->GetNrOfCascadeLevels(); l++)
		{
			this->Dx_DeviceContext->OMSetRenderTargets(0, 0, this->csm->GetShadowMapDSV(l));
			D3D11_VIEWPORT wp = this->csm->GetShadowMapViewPort(l);
			this->Dx_DeviceContext->RSSetViewports(1, &wp);
			this->Dx_DeviceContext->ClearDepthStencilView(this->csm->GetShadowMapDSV(l), D3D11_CLEAR_DEPTH, 1.0f, 0);
			
			//Terrain
			for(int i = 0; i < this->terrains.size(); i++)
			{
				//If the terrain has not been culled for shadowing, render it to shadow map.
				if(!terrains[i]->IsShadowCulled())
				{
					currentRenderedTerrainShadows++;

					//Input Assembler
					this->Dx_DeviceContext->IASetPrimitiveTopology(this->terrains[i]->GetTopology());

					//Matrices
					wvp = this->terrains[i]->GetWorldMatrix() * this->csm->GetViewProjMatrix(l);
					this->Shader_ShadowMap->SetMatrix("lightWVP", wvp);

					//Vertex data
					Buffer* verts = this->terrains[i]->GetVertexBufferPointer();
					Buffer* inds = this->terrains[i]->GetIndexBufferPointer();
					if(verts)
					{
						inds->Apply();
					}
					if(inds)
					{
						verts->Apply();
					}

					//Texture - tell the shader to not use texture(s), as this will generate a warning if set to true. **TILLMAN - fortfarande samma problem, se nedan.
					this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
					this->Shader_ShadowMap->SetBool("textured", false);

					//Apply Shader
					this->Shader_ShadowMap->Apply(0);

					//Draw
					if(inds)
					{
						this->Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
					}
					else if(verts)
					{
						this->Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
					}
					else
					{
						MaloW::Debug("WARNING: DxManagerRender: RenderCascadedShadowMap(): Both vertex and indexbuffers for terrain were NULL.");
					}
				}
			}

			//Static meshes
			for(int i = 0; i < this->objects.size(); i++)
			{
				StaticMesh* staticMesh = this->objects[i];
				if(!staticMesh->IsUsingInvisibility() && !staticMesh->GetDontRenderFlag())
				{
					D3DXVECTOR3 distance = staticMesh->GetBillboardGFX()->GetPositionD3DX() - this->camera->GetPositionD3DX();

					//If the mesh has a billboard AND is inside the billboard range, render the mesh
					float billboardRange = 0.0f;
					//If no billboard range has been set for the mesh, use the default value
					if(staticMesh->GetDistanceToSwapToBillboard() <= 0.0f)
					{
						billboardRange = this->params.FarClip * this->params.BillboardRange;
					}
					else
					{
						billboardRange = staticMesh->GetDistanceToSwapToBillboard();
					}
					if(D3DXVec3Length(&distance) < billboardRange || staticMesh->GetBillboardFilePath() == "")
					{
						MaloW::Array<MeshStrip*>* strips = staticMesh->GetStrips();
						wvp = staticMesh->GetWorldMatrix() * this->csm->GetViewProjMatrix(l);
						this->Shader_ShadowMap->SetMatrix("lightWVP", wvp);

						bool hasBeenCounted = false;

						for(int u = 0; u < strips->size(); u++)
						{
							//Render strip to shadow map if it has not been shadow culled
							if(!staticMesh->IsStripShadowCulled(u))
							{
								if(!hasBeenCounted) //only count per mesh, not strip.
								{
									currentRenderedMeshShadows++;
									hasBeenCounted = true;
								}

								Object3D* obj = strips->get(u)->GetRenderObject();

								//Vertex data
								this->Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());
								Buffer* verts = obj->GetVertBuff();
								Buffer* inds = obj->GetIndsBuff();
								if(verts)
								{
									verts->Apply();
								}
								if(inds)
								{
									inds->Apply();
								}
						
								//Texture
								if(obj->GetTextureResource() != NULL)
								{
									if(obj->GetTextureResource()->GetSRVPointer() != NULL)
									{
										this->Shader_ShadowMap->SetResource("diffuseMap", obj->GetTextureResource()->GetSRVPointer());
										this->Shader_ShadowMap->SetBool("textured", true);
									}
									else
									{
										this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
										this->Shader_ShadowMap->SetBool("textured", false);
									}
								}
								else
								{
									this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
									this->Shader_ShadowMap->SetBool("textured", false);
								}

								//Apply Shader
								this->Shader_ShadowMap->Apply(0);

								//Draw
								if(inds)
								{
									Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
								}
								else if(verts)
								{
									Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
								}
								else
								{
									MaloW::Debug("WARNING: DxManagerRender: RenderCascadedShadowMap(): Both vertex and indexbuffers for static mesh were NULL.");
								}
							}
						}
					}
					else
					{
						//Just check the first strip if it is culled.  //**TILLMAN todo: add once**
						if(!staticMesh->IsStripCulled(0))
						{
							currentRenderedMeshShadows++;

							//Add billboard info
							this->instancingHelper->AddBillboard(staticMesh);
						}
					}
				}
			}

			//Unbind shader resources
			this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
			this->Shader_ShadowMap->Apply(0);
			
			//Animated meshes
			for(int i = 0; i < this->animations.size(); i++)
			{
				AnimatedMesh* animatedMesh = this->animations[i];
				if(!animatedMesh->IsUsingInvisibility() && !animatedMesh->GetDontRenderFlag())
				{
					D3DXVECTOR3 distance = animatedMesh->GetBillboardGFX()->GetPositionD3DX() - this->camera->GetPositionD3DX();

					//If the mesh has a billboard AND is inside the billboard range, render the mesh
					float billboardRange = 0.0f;
					//If no billboard range has been set for the mesh, use the default value
					if(animatedMesh->GetDistanceToSwapToBillboard() <= 0.0f)
					{
						billboardRange = this->params.FarClip * this->params.BillboardRange;
					}
					else
					{
						billboardRange = animatedMesh->GetDistanceToSwapToBillboard();
					}

					if(D3DXVec3Length(&distance) < billboardRange || animatedMesh->GetBillboardFilePath() == "")
					{

						//**Tillman - tillräckligt att kolla 1??**
						if(!animatedMesh->IsStripShadowCulled(0))
						{
							currentRenderedMeshShadows++;
							if(animatedMesh->GetFilePath() == "Media/Tree_02.ani") //TEST
							{
								float durp= 1.0f;
							}
							KeyFrame* one = NULL;
							KeyFrame* two = NULL;
							float t = 0.0f;
							animatedMesh->SetCurrentTime(this->Timer * 1000.0f); //Timer is in seconds.
							animatedMesh->GetCurrentKeyFrames(&one, &two, t);
							MaloW::Array<MeshStrip*>* stripsOne = one->meshStripsResource->GetMeshStripsPointer();
							MaloW::Array<MeshStrip*>* stripsTwo = two->meshStripsResource->GetMeshStripsPointer();

							//Set shader data (per object)
							this->Shader_ShadowMapAnimated->SetFloat("t", t);
							D3DXMATRIX wvp = animatedMesh->GetWorldMatrix() * this->csm->GetViewProjMatrix(l);
							this->Shader_ShadowMapAnimated->SetMatrix("lightWVP", wvp); 

							for(int u = 0; u < stripsOne->size(); u++)  //**Tillman todo - indices?**
							{
								//Set shader data per strip
								Object3D* objOne = stripsOne->get(u)->GetRenderObject();
								Object3D* objTwo = stripsTwo->get(u)->GetRenderObject();

								//Vertex data
								this->Dx_DeviceContext->IASetPrimitiveTopology(objOne->GetTopology()); 
								Buffer* vertsOne = objOne->GetVertBuff();
								Buffer* vertsTwo = objTwo->GetVertBuff();
								ID3D11Buffer* vertexBuffers [] = {vertsOne->GetBufferPointer(), vertsTwo->GetBufferPointer()};
								UINT strides [] = {sizeof(VertexNormalMap), sizeof(VertexNormalMap)}; //**Tillman - input layout stämmer inte, klagar inte dock
								UINT offsets [] = {0, 0};
								if(vertsOne != NULL && vertsTwo != NULL)
								{
									this->Dx_DeviceContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
									
									//Textures
									if(objOne->GetTextureResource() != NULL && objTwo->GetTextureResource() != NULL)
									{
										if(objOne->GetTextureResource()->GetSRVPointer() != NULL && objTwo->GetTextureResource()->GetSRVPointer() != NULL)
										{
											this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", objOne->GetTextureResource()->GetSRVPointer());
											//Only need one diffuse map since no blending between maps is done.
											//this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", objTwo->GetTextureResource()->GetSRVPointer());
											this->Shader_ShadowMapAnimated->SetBool("textured", true);
										}
										else
										{
											this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", NULL);
											//Only need one diffuse map since no blending between maps is done.
											//this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", NULL);
											this->Shader_ShadowMapAnimated->SetBool("textured", false);
										}
									}
									else
									{
										this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", NULL);
										//Only need one diffuse map since no blending between maps is done.
										//this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", NULL);
										this->Shader_ShadowMapAnimated->SetBool("textured", false);
									}

									//Apply
									this->Shader_ShadowMapAnimated->Apply(0);

									//Draw
									this->Dx_DeviceContext->Draw(vertsOne->GetElementCount(), 0); 
								}
								else
								{
									MaloW::Debug("WARNING: DxManagerRender: RenderCascadedShadowMap(): One or both vertex buffers for animated mesh were NULL.");
								}
							}
						}
					}
					else
					{
						//Just check the first strip if it is culled.  //**TILLMAN todo: add once**
						if(!animatedMesh->IsStripShadowCulled(0))
						{
							currentRenderedMeshShadows++;
							//Add billboard info
							this->instancingHelper->AddBillboard(animatedMesh);
						}
					}
				}
			}
		

			//Unbind shader resources
			this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", NULL);
			this->Shader_ShadowMapAnimated->Apply(0);
			//Only need one diffuse map since no blending between maps is done.
			//this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", NULL);
		}
	}
	else
	{
		this->Shader_ShadowMapAnimated->Apply(0); //**TILLMAN - SKA INTE BEHÖVAS, MEN MÅSTE. 
		//** min gissning är att någon variabel (som ligger i stdafx.fx) sätts av denna shader
		//och används en en ANNAN shader och måste därmed appliceras. **
	}

	this->renderedMeshShadows = currentRenderedMeshShadows;
	this->renderedTerrainShadows = currentRenderedTerrainShadows;
}
void DxManager::RenderCascadedShadowMapBillboardInstanced()
{
	if(this->instancingHelper->GetNrOfBillboards() > 0)
	{
		//Sort, create instance groups and update buffer before rendering
		this->instancingHelper->PreRenderBillboards(); //**Tillman todo opt: remove redundant billboard data**



		//Draw billboards
		unsigned int strides[1];
		unsigned int offsets[1];
		ID3D11Buffer* bufferPointers[1];

		// Set the buffer strides.
		strides[0] = sizeof(Vertex);
		// Set the buffer offset.
		offsets[0] = 0;
		// Set the array of pointers to the vertex and instance buffers.
		bufferPointers[0] = this->instancingHelper->GetBillboardInstanceBuffer();	

		// Set the vertex(instance) buffer
		this->Dx_DeviceContext->IASetVertexBuffers(0, 1, bufferPointers, strides, offsets);
		this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		// Set global variables per frame
		this->Shader_ShadowMapBillboardInstanced->SetFloat3("gSunDir", this->sun.direction);
		
		// Per cascade:
		for(int i = 0; i < this->csm->GetNrOfCascadeLevels(); ++i)
		{
			// Set data
			this->Dx_DeviceContext->OMSetRenderTargets(0, 0, this->csm->GetShadowMapDSV(i));
			D3D11_VIEWPORT wp = this->csm->GetShadowMapViewPort(i);
			this->Dx_DeviceContext->RSSetViewports(1, &wp);

			this->Shader_ShadowMapBillboardInstanced->SetMatrix("gLightViewProj", this->csm->GetViewProjMatrix(i));

			// Per instance group:
			for(unsigned int j = 0; j < this->instancingHelper->GetNrOfBillboardGroups(); ++j)
			{
				if(this->instancingHelper->GetBillboardGroup(j).s_SRV != NULL) 
				{
					this->Shader_ShadowMapBillboardInstanced->SetResource("gDiffuseMap", this->instancingHelper->GetBillboardGroup(j).s_SRV);
					this->Shader_ShadowMapBillboardInstanced->SetBool("gIsTextured", true);
				}
				else
				{
					this->Shader_ShadowMapBillboardInstanced->SetResource("gDiffuseMap", NULL);
					this->Shader_ShadowMapBillboardInstanced->SetBool("gIsTextured", false);
				}
				
				//Apply pass and input layout
				this->Shader_ShadowMapBillboardInstanced->Apply(0);

				//Draw
				int instanceCount = this->instancingHelper->GetBillboardGroup(j).s_Size;
				int startInstanceLocation = this->instancingHelper->GetBillboardGroup(j).s_StartLocation;
				this->Dx_DeviceContext->DrawInstanced(instanceCount, 1, startInstanceLocation, 0); //**TILLMAN, parameters inversed**
			}
		}



		//Reset counter (nrofbillboards)
		this->instancingHelper->PostRenderBillboards();
	}
}

void DxManager::CalculateCulling()
{
	//CAMERA:
	D3DXMATRIX view = this->camera->GetViewMatrix();
	D3DXMATRIX proj = this->camera->GetProjectionMatrix();

	/*
	float zMinimum = -proj._43 / proj._33;
	float r = this->params.FarClip / (this->params.FarClip - zMinimum);
	proj._33 = r;
	proj._43 = -r * zMinimum;
	*/

	D3DXMATRIX VP;
	D3DXMatrixMultiply(&VP, &view, &proj);


	// Calculate near plane of frustum.
	FrustrumPlanes[0].a = VP._14 + VP._13;
	FrustrumPlanes[0].b = VP._24 + VP._23;
	FrustrumPlanes[0].c = VP._34 + VP._33;
	FrustrumPlanes[0].d = VP._44 + VP._43;
	D3DXPlaneNormalize(&FrustrumPlanes[0], &FrustrumPlanes[0]);

	// Calculate far plane of frustum.
	FrustrumPlanes[1].a = VP._14 - VP._13; 
	FrustrumPlanes[1].b = VP._24 - VP._23;
	FrustrumPlanes[1].c = VP._34 - VP._33;
	FrustrumPlanes[1].d = VP._44 - VP._43;
	D3DXPlaneNormalize(&FrustrumPlanes[1], &FrustrumPlanes[1]);

	// Calculate left plane of frustum.
	FrustrumPlanes[2].a = VP._14 + VP._11; 
	FrustrumPlanes[2].b = VP._24 + VP._21;
	FrustrumPlanes[2].c = VP._34 + VP._31;
	FrustrumPlanes[2].d = VP._44 + VP._41;
	D3DXPlaneNormalize(&FrustrumPlanes[2], &FrustrumPlanes[2]);

	// Calculate right plane of frustum.
	FrustrumPlanes[3].a = VP._14 - VP._11; 
	FrustrumPlanes[3].b = VP._24 - VP._21;
	FrustrumPlanes[3].c = VP._34 - VP._31;
	FrustrumPlanes[3].d = VP._44 - VP._41;
	D3DXPlaneNormalize(&FrustrumPlanes[3], &FrustrumPlanes[3]);

	// Calculate top plane of frustum.
	FrustrumPlanes[4].a = VP._14 - VP._12; 
	FrustrumPlanes[4].b = VP._24 - VP._22;
	FrustrumPlanes[4].c = VP._34 - VP._32;
	FrustrumPlanes[4].d = VP._44 - VP._42;
	D3DXPlaneNormalize(&FrustrumPlanes[4], &FrustrumPlanes[4]);

	// Calculate bottom plane of frustum.
	FrustrumPlanes[5].a = VP._14 + VP._12;
	FrustrumPlanes[5].b = VP._24 + VP._22;
	FrustrumPlanes[5].c = VP._34 + VP._32;
	FrustrumPlanes[5].d = VP._44 + VP._42;
	D3DXPlaneNormalize(&FrustrumPlanes[5], &FrustrumPlanes[5]);
	
	//Terrain
	for(int i = 0; i < this->terrains.size(); i++)
	{
		Terrain* terr = this->terrains.get(i);

		float scale = max(terr->GetScale().x, max(terr->GetScale().y, terr->GetScale().z));
		if(pe.FrustrumVsSphere(this->FrustrumPlanes, terr->GetBoundingSphere(), terr->GetWorldMatrix(), scale))
		{
			terr->SetCulled(false);
			//If the object is inside the frustum, it can cast a shadow
			terr->SetShadowCulled(false);
		}
		else
		{
			terr->SetCulled(true);
			//However, the opposite may not be true for shadowing.
		}
	}

	//Static meshes
	for(int i = 0; i < this->objects.size(); i++)
	{
		StaticMesh* ms = this->objects.get(i);
		if ( ms->GetMeshStripsResourcePointer() != NULL)
		{
			MaloW::Array<MeshStrip*>* strips = ms->GetStrips();
			for(int u = 0; u < strips->size(); u++)
			{
				MeshStrip* s = strips->get(u);
				float scale = max(ms->GetScaling().x, max(ms->GetScaling().y, ms->GetScaling().z));
				if(pe.FrustrumVsSphere(this->FrustrumPlanes, s->GetBoundingSphere(), ms->GetWorldMatrix(), scale))
				{
					ms->SetStripCulledFlag(u, false);
					//If the object is inside the frustum, it can cast a shadow
					ms->SetStripShadowCulledFlag(u, false);
				}
				else
				{
					ms->SetStripCulledFlag(u, true);
					//However, the opposite may not be true for shadowing.
				}
			}
		}
	}

	//Animated meshes
	for(int i = 0; i < this->animations.size(); i++)
	{
		AnimatedMesh* animatedMesh = this->animations.get(i);
		if ( animatedMesh->GetKeyFrames()->get(0)->meshStripsResource != NULL)
		{
			if ( !animatedMesh->GetKeyFrames()->get(0)->meshStripsResource->GetMeshStripsPointer()->isEmpty() )
			{
				float scale = max(animatedMesh->GetScaling().x, max(animatedMesh->GetScaling().y, animatedMesh->GetScaling().z));
				MaloW::Array<MeshStrip*>* strips = animatedMesh->GetKeyFrames()->get(0)->meshStripsResource->GetMeshStripsPointer();
				for(int u = 0; u < strips->size(); u++)
				{
					MeshStrip* strip = strips->get(u);
					if(pe.FrustrumVsSphere(this->FrustrumPlanes, strip->GetBoundingSphere(), animatedMesh->GetWorldMatrix(), scale))
					{
						animatedMesh->SetStripCulledFlag(u, false);
						//If the object is inside the frustum, it can cast a shadow
						animatedMesh->SetStripShadowCulledFlag(u, false);
					}
					else
					{
						animatedMesh->SetStripCulledFlag(u, true);
						//However, the opposite may not be true for shadowing.
					}
				}
			}
		}
	}


	//SHADOW CULLING - determine if an object is inside a cascade.
	if(this->csm != NULL && this->useShadow) //**TILLMAN TODO, kolla att OBB blir rätt, verkar inte så...**
	{
		//Calculate frustums - the frustum in this case i an OBB (the cascade). 
		this->csm->CalcCascadePlanes();



		//Terrain
		for(int i = 0; i < this->terrains.size(); i++)
		{
			Terrain* terrain = this->terrains.get(i);
			float scale = max(terrain->GetScale().x, max(terrain->GetScale().y, terrain->GetScale().z));

			//Terrain already in the cameras view frustum does not need to be checked,
			//so only check the ones that are outside of it. (The ones that have already been culled)
			if(terrain->IsCulled())
			{
				//See if the terrain is inside or intersects the bounding boxes(cascades).
				bool notDone = true;
				for(int k = 0; k < this->csm->GetNrOfCascadeLevels() && notDone; k++)
				{
					if(pe.FrustrumVsSphere(csm->GetCascadePlanes(k), terrain->GetBoundingSphere(), terrain->GetWorldMatrix(), scale))
					{
						//As long as the terrain is inside ONE of the cascades, it needs to be drawn to the shadow map.
						terrain->SetShadowCulled(false); 
						notDone = false;
					}
					else
					{
						terrain->SetShadowCulled(true);
					}
				}
			}
		}



		//Static meshes
		for(int i = 0; i < this->objects.size(); i++)
		{
			StaticMesh* staticMesh = this->objects.get(i);
			float scale = max(staticMesh->GetScaling().x, max(staticMesh->GetScaling().y, staticMesh->GetScaling().z));

			MaloW::Array<MeshStrip*>* strips = staticMesh->GetStrips();
			for(int j = 0; j < strips->size(); j++)
			{
				MeshStrip* strip = strips->get(j);

				//Objects already in the cameras view frustum does not need to be checked,
				//so only check the ones that are outside of it. (The ones that have already been culled)
				if(staticMesh->IsStripCulled(j))
				{
					//See if the strip is inside the bounding boxes(cascades) or intersects.
					bool notDone = true;
					for(int k = 0; k < this->csm->GetNrOfCascadeLevels() && notDone; k++)
					{
						if(pe.FrustrumVsSphere(csm->GetCascadePlanes(k), strip->GetBoundingSphere(), staticMesh->GetWorldMatrix(), scale))
						{
							//As long as the strip is inside ONE of the cascades, it needs to be drawn to the shadow map.
							staticMesh->SetStripShadowCulledFlag(j, false); //**tillman.b - onödig? görs vid vanlig culling?
							notDone = false;
						}
						else
						{
							staticMesh->SetStripShadowCulledFlag(j, true); //**tillman.a - onödig? görs vid vanlig culling?
						}
					}
				}
			}
		}



		//Animated meshes
		for(int i = 0; i < this->animations.size(); i++)
		{
			AnimatedMesh* animatedMesh = this->animations.get(i);

			if ( !animatedMesh->GetKeyFrames()->get(0)->meshStripsResource->GetMeshStripsPointer()->isEmpty() )
			{
				float scale = max(animatedMesh->GetScaling().x, max(animatedMesh->GetScaling().y, animatedMesh->GetScaling().z));
				
				MaloW::Array<MeshStrip*>* strips = animatedMesh->GetStrips();
				for(int j = 0; j < strips->size(); j++)
				{
					MeshStrip* strip = strips->get(j);
				
					//Animations already in the cameras view frustum does not need to be checked,
					//so only check the ones that are outside of it. (The ones that have already been culled)
					if(animatedMesh->IsStripCulled(j))
					{
						//See if the strip is inside the bounding boxes(cascades) or intersects.
						bool notDone = true;
						for(int k = 0; k < this->csm->GetNrOfCascadeLevels() && notDone; k++)
						{
							if(pe.FrustrumVsSphere(csm->GetCascadePlanes(k), strip->GetBoundingSphere(), animatedMesh->GetWorldMatrix(), scale))
							{
								//As long as the strip is inside ONE of the cascades, it needs to be drawn to the shadow map.
								animatedMesh->SetStripShadowCulledFlag(j, false); //b
								notDone = false;
							}
							else
							{
								animatedMesh->SetStripShadowCulledFlag(j, true); //b
							}
						}
					}
				}
			}
		}
	}
}

void DxManager::RenderFBXMeshes()
{
	D3DXMATRIX proj = this->camera->GetProjectionMatrix();
	D3DXMATRIX view = this->camera->GetViewMatrix();
	for(int i = 0; i < this->FBXMeshes.size(); i++)
	{
		float dt = this->Timer - this->LastFBXUpdate;
		this->FBXMeshes[i]->Update(dt);
		this->FBXMeshes[i]->Render(dt, proj, view, this->Shader_FBX, this->Dx_DeviceContext);
	}


	this->LastFBXUpdate = this->Timer;
}

void DxManager::RenderEnclosingFog()
{
	// only draw fog if there should be fog drawn.
	Vector3 camToFogCenter = this->camera->GetPosition() - this->fogCenter;
	float distanceToFogCenter = camToFogCenter.GetLength();
	if(distanceToFogCenter > this->fogRadius)
	{
		float fogfactor = 0.0f;
		float maxFog = this->fogRadius * this->fogFadeFactor;
		if(distanceToFogCenter > this->fogRadius + maxFog)
			fogfactor = 1.0f;
		else
		{
			float curFog = distanceToFogCenter - this->fogRadius;

			fogfactor = curFog / maxFog;

			fogfactor += fogfactor - (fogfactor * fogfactor);	
			// Exponential fog, making it intense quickly at first and slow at the end.
		}

		this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
		this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
		this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		this->Shader_FogEnclosement->SetFloat("fogfactor", fogfactor);
		this->Shader_FogEnclosement->Apply(0);

		this->Dx_DeviceContext->Draw(1, 0);
	}
}

HRESULT DxManager::Render()
{
	if(this->RendererSleep > 0)
		Sleep((DWORD)this->RendererSleep);
	if(GetForegroundWindow() != this->hWnd)	// Sleep a little if you're alt tabbed out of the game to prevent desktop lag.
		Sleep((DWORD)10);

	// Timer
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	float diff = (li.QuadPart - prevTimeStamp) / this->PCFreq;
	this->prevTimeStamp = li.QuadPart;

	this->Timer += diff * 0.001f;

	this->CalculateCulling();

	this->PreRender();


	this->RenderShadowMap();
	this->RenderCascadedShadowMap();
	this->RenderCascadedShadowMapBillboardInstanced(); //Must be after RenderCascadedShadowMap()

	//this->RenderForward();


	this->RenderDeferredGeoTerrains();
	this->RenderDecals();

	this->RenderDeferredGeoObjects();
	this->RenderDeferredGeometryInstanced(); //Must be after RenderDeferredGeometry()

	this->RenderBillboards();
	this->RenderBillboardsInstanced(); //Must be after RenderDeferredGeometry()

	
	//this->RenderQuadDeferred();
	//this->RenderDeferredTexture();

	this->RenderDeferredPerPixel();

	this->RenderDeferredGeoTranslucent();

	this->RenderDeferredPerPixelTranslucent();
	
	if(this->invisibleGeometry)
		this->RenderInvisibilityEffect(); 

	//this->RenderWaterPlanes();

	this->RenderFBXMeshes();
	
	this->RenderDeferredSkybox();

	this->RenderParticles();

	if(this->useEnclosingFog)
		this->RenderEnclosingFog();

	this->RenderImages();

	this->RenderText();

	this->RenderAntiAliasing();


	

	// Debugging:
	// Debug: Render Normals
	//MaloW::Array<StaticMesh*>* meshes = &this->objects;
	//DrawNormals(meshes, this->Dx_Device, this->Dx_DeviceContext, this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
	//MaloW::Array<Terrain*>* terrains = &this->terrains;
	//DrawNormals(terrains, this->Dx_Device, this->Dx_DeviceContext, this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
	// Debug: Render Wireframe
	//MaloW::Array<StaticMesh*>* meshes = &this->objects;
	//DrawWireFrame(meshes, this->Dx_Device, this->Dx_DeviceContext, this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
	// Debug: Render Wireframe
	//MaloW::Array<Terrain*>* terrs = &this->terrains;
	//DrawWireFrame(terrs, this->Dx_Device, this->Dx_DeviceContext, this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
	// Debug: Render BoundingSpheres
	//MaloW::Array<StaticMesh*>* meshes = &this->objects;
	//DrawBoundingSpheres(meshes, this->Dx_Device, this->Dx_DeviceContext, this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());

	/*
	// Render RTs pictures
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	
	for(int q = 0; q < this->NrOfRenderTargets; q++)
		DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_Image, this->Dx_GbufferSRVs[q], q); 
	*/
	
	
	// Render shadowmap pictures:
	//for(int q = 0; q < this->lights.size(); q++)
		//DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_Image, this->lights[q]->GetShadowMapSRV(), q); 
	//for(int q = 0; q < this->csm->GetNrOfCascadeLevels(); q++)
	//	DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_Image, this->csm->GetShadowMapSRV(q), q); 

	
	
	if(FAILED(Dx_SwapChain->Present( 0, 0 )))
		return E_FAIL;

	return S_OK;
}
