#include "DxManager.h"

#include "DxManagerDebugging.h"


HRESULT DxManager::Update(float)
{
	// update subsystems
	//ps.update(deltaTime);
	//this->camera->Update(deltaTime);				// Moved to Life below to counter update spasms due to thread-collision



	return S_OK;
}
void DxManager::Life()
{
	{
		//Engine Start Splash screen.
		Image* img = new Image(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->params.WindowWidth, (float)this->params.WindowHeight));
		TextureResource* tex = NULL;
		tex = GetResourceManager()->CreateTextureResourceFromFile("Media/LoadingScreen/StartingSplash.png");
		img->SetTexture(tex);
		img->SetStrata(10.0f);
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
#ifdef MALOWTESTPERF
	this->perf.ResetAll();
#endif
	while(this->stayAlive)
	{
#ifdef MALOWTESTPERF
		this->perf.PreMeasure("Renderer - Entire Frame", 0);
#endif


#ifdef MALOWTESTPERF
		this->perf.PreMeasure("Renderer - Life Overhead", 1);
#endif
		while(MaloW::ProcessEvent* ev = this->PeekEvent())
		{
			if(dynamic_cast<RendererEvent*>(ev) != NULL)
			{
				// StaticMeshEvent
				if(dynamic_cast<StaticMeshEvent*>(ev) != NULL)
				{
					this->HandleStaticMeshEvent((StaticMeshEvent*)ev);
				}

				// AnimatedMeshEvent
				else if(dynamic_cast<AnimatedMeshEvent*>(ev) != NULL)
				{
					this->HandleAnimatedMeshEvent((AnimatedMeshEvent*)ev);
				}

				//TerrainEvent
				else if(dynamic_cast<TerrainEvent*>(ev) != NULL)
				{
					this->HandleTerrainEvent((TerrainEvent*)ev);
				}

				// BillboardEvent
				else if(dynamic_cast<BillboardEvent*>(ev) != NULL)
				{
					this->HandleBillboardEvent((BillboardEvent*)ev);
				}

				// BillboardCollectionEvent
				else if(dynamic_cast<BillboardCollectionEvent*>(ev) != NULL)
				{
					this->HandleBillboardCollectionEvent((BillboardCollectionEvent*)ev);
				}

				//FBXEvent
				else if(dynamic_cast<FBXEvent*>(ev) != NULL)
				{
					this->HandleFBXEvent((FBXEvent*)ev);
				}

				//WaterPlaneEvent
				else if(dynamic_cast<WaterPlaneEvent*>(ev) != NULL)
				{
					this->HandleWaterPlaneEvent((WaterPlaneEvent*)ev);
				}

				// DecalEvent
				else if(dynamic_cast<DecalEvent*>(ev) != NULL)
				{
					this->HandleDecalEvent((DecalEvent*)ev);
				}

				// ImageEvent
				else if(dynamic_cast<ImageEvent*>(ev) != NULL)
				{
					this->HandleImageEvent((ImageEvent*)ev);
				}

				// TextEvent
				else if(dynamic_cast<TextEvent*>(ev) != NULL)
				{
					this->HandleTextEvent((TextEvent*)ev);
				}

				// LightEvent
				else if(dynamic_cast<LightEvent*>(ev) != NULL)
				{
					this->HandleLightEvent((LightEvent*)ev);
				}

				//SetCameraEvent
				else if(dynamic_cast<SetCameraEvent*>(ev) != NULL)
				{
					this->HandleSetCameraEvent((SetCameraEvent*)ev);
				}

				// ResizeEvent
				else if(dynamic_cast<ResizeEvent*>(ev) != NULL)
				{
					this->ResizeRenderer((ResizeEvent*)ev);
				}

				//ChangeShadowQualityEvent
				else if(ChangeShadowQualityEvent* csqe = dynamic_cast<ChangeShadowQualityEvent*>(ev))
				{
					this->params.ShadowMapSettings = csqe->GetQuality();
					if(this->csm)
					{
						this->useShadow = true;
						this->csm->ResizeShadowmaps(this->Dx_Device, this->params.ShadowMapSettings);
					}
				}

				// ReloadShaderEvent
				else if(ReloadShaderEvent* rse = dynamic_cast<ReloadShaderEvent*>(ev))
					this->HandleReloadShaders(rse->GetShader());
			}
			delete ev;
		}
		this->camera->Update(this->Timer - this->LastCamUpdate);
		this->LastCamUpdate = this->Timer;


#ifdef MALOWTESTPERF
		this->Dx_DeviceContext->Flush();
		this->perf.PostMeasure("Renderer - Life Overhead", 1);
#endif

#ifdef MALOWTESTPERF
		this->perf.PreMeasure("Renderer - Render", 1);
#endif
		this->Render();
		this->framecount++;
#ifdef MALOWTESTPERF
		this->Dx_DeviceContext->Flush();
		this->perf.PostMeasure("Renderer - Render", 1);
#endif

#ifdef MALOWTESTPERF
		this->Dx_DeviceContext->Flush();
		this->perf.PostMeasure("Renderer - Entire Frame", 0);
#endif
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
	// Generate and send shadowmaps to the main-shader
	//EDIT 2013-01-23 by Tillman - Added transparancy.
	if(this->lights.size() > 0)
	{
		for (unsigned int l = 0; l < this->lights.size(); l++)
		{
			Dx_DeviceContext->OMSetRenderTargets(0, 0, this->lights[l]->GetShadowMapDSV());
			Dx_DeviceContext->RSSetViewports(1, &this->lights[l]->GetShadowMapViewPort());
			Dx_DeviceContext->ClearDepthStencilView(this->lights[l]->GetShadowMapDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			//Static meshes
			for(unsigned int i = 0; i < this->objects.size(); i++)
			{
				if(!this->objects[i]->IsUsingInvisibility() && !this->objects[i]->GetDontRenderFlag())
				{
					MaloW::Array<MeshStrip*>* strips = this->objects[i]->GetStrips();
					D3DXMATRIX wvp = this->objects[i]->GetWorldMatrix() * this->lights[l]->GetViewProjMatrix();
					this->Shader_ShadowMap->SetMatrix("lightWVP", wvp);
				
					for(unsigned int u = 0; u < strips->size(); u++)
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
			for(unsigned int i = 0; i < this->animations.size(); i++)
			{
				if(!this->animations[i]->IsUsingInvisibility() && !this->animations[i]->GetDontRenderFlag())
				{
					KeyFrame* one = NULL;
					KeyFrame* two = NULL;
					float t = 0.0f;
					this->animations[i]->GetCurrentKeyFrames(one, two, t, this->Timer * 1000.0f);//Timer is in seconds.
					MaloW::Array<MeshStrip*>* stripsOne = one->meshStripsResource->GetMeshStripsPointer();
					MaloW::Array<MeshStrip*>* stripsTwo = two->meshStripsResource->GetMeshStripsPointer();

					//Set shader data (per object)
					this->Shader_ShadowMapAnimated->SetFloat("t", t);
					D3DXMATRIX wvp = this->animations[i]->GetWorldMatrix() * this->lights[l]->GetViewProjMatrix();
					this->Shader_ShadowMapAnimated->SetMatrix("LightWVP", wvp); 

					for(unsigned int u = 0; u < stripsOne->size(); u++)  
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


			// FBX meshes
			for(unsigned int i = 0; i < this->FBXMeshes.size(); i++)
			{
				this->FBXMeshes[i]->RenderShadow(0, this->lights[l]->GetViewProjMatrix(), this->Shader_ShadowMapFBX, this->Dx_DeviceContext, this->sun.direction);
			}




			D3DXMATRIX lvp = this->lights[l]->GetViewProjMatrix();
		
		
			// Forward
			//this->Shader_ForwardRendering->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsMatrix(l, "gLights", "LightViewProj", lvp);
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat4(l, "gLights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPosition(), 1));
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat4(l, "gLights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColor(), 1));
			//this->Shader_ForwardRendering->SetStructMemberAtIndexAsFloat(l, "gLights", "LightIntensity", this->lights[l]->GetIntensity());
		

			// For deferred: TILLE TILLMAN: Detta ska inte ligga här. också: detta ska göras för DPPTranslucent.fx
			this->Shader_DeferredLightning->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsMatrix(l, "gLights", "LightViewProj", lvp);
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat4(l, "gLights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPositionD3DX(), 1));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat4(l, "gLights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColorD3DX(), 1));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat(l, "gLights", "LightIntensity", this->lights[l]->GetIntensity());
		
		
			// For deferred quad:
			//this->Shader_DeferredQuad->SetResourceAtIndex(l, "ShadowMap", this->lights[l]->GetShadowMapSRV());
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsMatrix(l, "gLights", "LightViewProj", lvp);
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat4(l, "gLights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPosition(), 1));
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat4(l, "gLights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColor(), 1));
			//this->Shader_DeferredQuad->SetStructMemberAtIndexAsFloat(l, "gLights", "LightIntensity", this->lights[l]->GetIntensity());
		
		}
		
		//This shouldnt be here, should be in RenderPerPixel?
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
		this->Shader_DeferredLightning->SetFloat("SMAP_DX", 1.0f / (256.0f * powf(2.0f, (float)this->params.ShadowMapSettings * 0.5f)));
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
}


void DxManager::RenderImages()
{
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	

	for(unsigned int i = 0; i < this->images.size(); i++)
	{
		Image* img = this->images[i];
		// if Convert from screenspace is needed, which it isnt.
		this->Shader_Image->SetFloat("posx", (img->GetPosition().x / this->params.WindowWidth) * 2 - 1);
		this->Shader_Image->SetFloat("posy", 2 - (img->GetPosition().y / this->params.WindowHeight) * 2 - 1);
		this->Shader_Image->SetFloat("dimx", (img->GetDimensions().x / this->params.WindowWidth) * 2);
		this->Shader_Image->SetFloat("dimy", -(img->GetDimensions().y / this->params.WindowHeight) * 2);
		this->Shader_Image->SetFloat("opacity", img->GetOpacity());
		this->Shader_Image->SetFloat("strata", img->GetStrata01());

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
		strides[0] = sizeof(VertexBillboard1);
		// Set the buffer offset.
		offsets[0] = 0;
		// Set the array of pointers to the vertex and instance buffers.
		bufferPointers[0] = this->instancingHelper->GetBillboardInstanceBuffer();	

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		this->Dx_DeviceContext->IASetVertexBuffers(0, 1, bufferPointers, strides, offsets);
		this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		// Set global variables per frame
		this->Shader_BillboardInstanced->SetFloat3("g_CameraPos", this->camera->GetOldPos());
		this->Shader_BillboardInstanced->SetMatrix("g_CamViewProj", this->camera->GetViewProjMatrix());
		this->Shader_BillboardInstanced->SetFloat("g_FarClip", this->params.FarClip);

		// Set global variables per instance group
		for(unsigned int i = 0; i < this->instancingHelper->GetNrOfBillboardGroups(); ++i)
		{
			BillboardGroup bbGroup = this->instancingHelper->GetBillboardGroup(i);
			this->Shader_BillboardInstanced->SetFloat("g_bb_CullNear", bbGroup.s_CullNearDistance);
			this->Shader_BillboardInstanced->SetFloat("g_bb_CullFar", bbGroup.s_CullFarDistance);

			if(bbGroup.s_SRV != NULL) 
			{
				this->Shader_BillboardInstanced->SetResource("g_bb_DiffuseMap", bbGroup.s_SRV);
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
			int instanceCount = bbGroup.s_Size;
			int startInstanceLocation = bbGroup.s_StartLocation;
			this->Dx_DeviceContext->Draw(instanceCount, startInstanceLocation);

			//Debug data
			this->CurrentNrOfDrawCalls++; 
		}
		//Debug data
		this->CurrentRenderedNrOfVertices += 4 * this->instancingHelper->GetNrOfBillboards(); 
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

	for(unsigned int i = 0; i < this->texts.size(); i++)
	{
		Text* txt = this->texts[i];
		// if Convert from screen space is needed, which it is
		this->Shader_Text->SetFloat("posx", (txt->GetPosition().x / this->params.WindowWidth) * 2 - 1);
		this->Shader_Text->SetFloat("posy", 2 - (txt->GetPosition().y / this->params.WindowHeight) * 2 - 1);
		this->Shader_Text->SetFloat("strata", txt->GetStrata01());

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

		this->Shader_Text->SetFloat3("overlayColor", txt->GetColor());
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
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Cascaded Shadowmap", 3);
#endif

	//Reset counters
	int currentRenderedTerrainShadows = 0;
	int currentRenderedMeshShadows = 0;

	if(this->useSun && this->useShadow)
	{
		bool* hasStaticMeshBeenAdded = new bool[this->objects.size()]; 
		for(unsigned int i = 0; i < this->objects.size(); ++i)
		{
			hasStaticMeshBeenAdded[i] = false;
		}
		bool* hasAnimatedMeshBeenAdded = new bool[this->animations.size()]; 
		for(unsigned int i = 0; i < this->animations.size(); ++i)
		{
			hasAnimatedMeshBeenAdded[i] = false;
		}

		D3DXMATRIX wvp;
		
		for (unsigned int l = 0; l < this->csm->GetNrOfCascadeLevels(); l++)
		{
			this->Dx_DeviceContext->OMSetRenderTargets(0, 0, this->csm->GetShadowMapDSV(l));
			this->Dx_DeviceContext->RSSetViewports(1, &this->csm->GetShadowMapViewPort(l));
			this->Dx_DeviceContext->ClearDepthStencilView(this->csm->GetShadowMapDSV(l), D3D11_CLEAR_DEPTH, 1.0f, 0);

			//Terrain
			//No terrain has a transparent texture
			/*this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
			this->Shader_ShadowMap->SetBool("textured", false);
			for(unsigned int i = 0; i < this->terrains.size(); i++)
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
			}*/

			//Static meshes
			for(unsigned int i = 0; i < this->objects.size(); i++)
			{
				StaticMesh* staticMesh = this->objects[i];
				if(!staticMesh->IsUsingInvisibility() && !staticMesh->GetDontRenderFlag())
				{
					D3DXVECTOR3 distance = staticMesh->GetBillboardGFX()->GetPositionD3DX() - this->camera->GetOldPos();

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
					if(D3DXVec3Length(&distance) < billboardRange || !staticMesh->HasBillboard())
					{
						//Only add once (same object can be in several cascades).
						if(!hasStaticMeshBeenAdded[i])
						{
							//As long as one strip has not been culled, add whole mesh for instancing
							MaloW::Array<MeshStrip*>* strips = staticMesh->GetStrips();
							unsigned int index = 0;
							bool oneStripIsNotCulled = false;
							while(!oneStripIsNotCulled && index < strips->size())
							{
								if(!staticMesh->IsStripShadowCulled(index++))
								{
									oneStripIsNotCulled = true;
									currentRenderedMeshShadows++;
									//Add billboard info
									this->instancingHelper->AddStaticMesh(staticMesh);
									hasStaticMeshBeenAdded[i] = true;
								}
							}
						}
					}
					else
					{
						//Only add once (same object can be in several cascades).
						if(!hasStaticMeshBeenAdded[i])
						{
							//As long as one strip has not been culled, add whole mesh for instancing
							MaloW::Array<MeshStrip*>* strips = staticMesh->GetStrips();
							unsigned int index = 0;
							bool oneStripIsNotCulled = false;
							while(!oneStripIsNotCulled && index < strips->size())
							{
								if(!staticMesh->IsStripShadowCulled(index++))
								{
									oneStripIsNotCulled = true;
									currentRenderedMeshShadows++;
									//Add billboard info
									this->instancingHelper->AddBillboard(staticMesh);
									hasStaticMeshBeenAdded[i] = true;
								}
							}
						}
					}
				}
			}

			//Unbind shader resources
			this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
			this->Shader_ShadowMap->Apply(0);

			//Animated meshes
			for(unsigned int i = 0; i < this->animations.size(); i++)
			{
				AnimatedMesh* animatedMesh = this->animations[i];
				if(!animatedMesh->IsUsingInvisibility() && !animatedMesh->GetDontRenderFlag())
				{
					D3DXVECTOR3 distance = animatedMesh->GetBillboardGFX()->GetPositionD3DX() - this->camera->GetOldPos();

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

					if(D3DXVec3Length(&distance) < billboardRange || !animatedMesh->HasBillboard())
					{
						//Only add once (same object can be in several cascades).
						if(!hasAnimatedMeshBeenAdded[i])
						{
							//As long as one strip has not been culled, add whole mesh for instancing
							MaloW::Array<MeshStrip*>* strips = animatedMesh->GetStrips();
							unsigned int index = 0;
							bool oneStripIsNotCulled = false;
							while(!oneStripIsNotCulled && index < strips->size())
							{
								if(!animatedMesh->IsStripShadowCulled(index++))
								{
									oneStripIsNotCulled = true;
									currentRenderedMeshShadows++;
									//Add billboard info
									this->instancingHelper->AddAnimatedMesh(animatedMesh, this->Timer);
									hasAnimatedMeshBeenAdded[i] = true;
								}
							}
						}
					}
					else
					{
						//Only add once (same object can be in several cascades).
						if(!hasAnimatedMeshBeenAdded[i])
						{
							//As long as one strip has not been culled, add whole mesh for instancing
							MaloW::Array<MeshStrip*>* strips = animatedMesh->GetStrips();
							unsigned int index = 0;
							bool oneStripIsNotCulled = false;
							while(!oneStripIsNotCulled && index < strips->size())
							{
								if(!animatedMesh->IsStripShadowCulled(index++))
								{
									oneStripIsNotCulled = true;
									currentRenderedMeshShadows++;
									//Add billboard info
									this->instancingHelper->AddBillboard(animatedMesh);
									hasAnimatedMeshBeenAdded[i] = true;
								}
							}
						}
					}
				}
			}
			//Unbind shader resources
			this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", NULL);
			this->Shader_ShadowMapAnimated->Apply(0);
			//Only need one diffuse map since no blending between maps is done.
			//this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", NULL);
				
			// FBX meshes
			for(unsigned int i = 0; i < this->FBXMeshes.size(); i++)
			{
				this->FBXMeshes[i]->RenderShadow(0, this->csm->GetViewProjMatrix(l), this->Shader_ShadowMapFBX, this->Dx_DeviceContext, this->sun.direction);
			}
		}

		delete [] hasStaticMeshBeenAdded;
		delete [] hasAnimatedMeshBeenAdded;
	}

	this->renderedMeshShadows = currentRenderedMeshShadows;
	this->renderedTerrainShadows = currentRenderedTerrainShadows;

#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Cascaded Shadowmap", 3);
#endif	
}



void DxManager::RenderCascadedShadowMapInstanced()
{
	if(this->csm != NULL)
	{

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Cascaded Shadowmap Instanced", 3);
#endif
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Cascaded Shadowmap Instanced Billboards", 4);
#endif
		//BILLBOARDS
		if(this->instancingHelper->GetNrOfBillboards() > 0)
		{
			//Sort, create instance groups and update buffer before rendering
			this->instancingHelper->PreRenderBillboards(true); 

			//Draw billboards
			//Set the vertex(instance) buffer
			unsigned int billboardStrides[1];
			unsigned int billboardOffsets[1];
			ID3D11Buffer* billboardBufferPointers[1];
			billboardStrides[0] = sizeof(VertexBillboard1);
			billboardOffsets[0] = 0;
			billboardBufferPointers[0] = this->instancingHelper->GetBillboardInstanceBuffer();	
			this->Dx_DeviceContext->IASetVertexBuffers(0, 1, billboardBufferPointers, billboardStrides, billboardOffsets);
			this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			//Per frame:
			this->Shader_ShadowMapBillboardInstanced->SetFloat3("gSunDir", this->sun.direction);

			//Per cascade:
			for(unsigned int i = 0; i < this->csm->GetNrOfCascadeLevels(); ++i)
			{
				//Set render targets & view ports - TILLE TILLMAN TODO: sätta ihop med terräng för färre API CALLS
				this->Dx_DeviceContext->OMSetRenderTargets(0, 0, this->csm->GetShadowMapDSV(i));
				D3D11_VIEWPORT wp = this->csm->GetShadowMapViewPort(i);
				this->Dx_DeviceContext->RSSetViewports(1, &wp);

				//Set variables
				this->Shader_ShadowMapBillboardInstanced->SetMatrix("gLightViewProj", this->csm->GetViewProjMatrix(i));

				//Per instance group:
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
					this->Dx_DeviceContext->Draw(instanceCount, startInstanceLocation);
				}
			}

			//Reset counter (nrofbillboards)
			this->instancingHelper->PostRenderBillboards();
		}
#ifdef MALOWTESTPERF
		this->Dx_DeviceContext->Flush();
		this->perf.PostMeasure("Renderer - Render Cascaded Shadowmap Instanced Billboards", 4);
	#endif

	#ifdef MALOWTESTPERF
		this->perf.PreMeasure("Renderer - Render Cascaded Shadowmap Instanced Static meshes", 4);
	#endif
		//STATIC OBJECTS
		if(this->instancingHelper->GetNrOfStrips() > 0)
		{
			//Sort, create instance groups and update buffer before rendering
			this->instancingHelper->PreRenderStrips(); 

			//Draw static meshes(meshstrips)
			//"Set" the instance buffer
			ID3D11Buffer* bufferPointers[2];
			unsigned int strides[2] = {sizeof(VertexNormalMap), sizeof(StripData::InstancedDataStruct)};
			unsigned int offsets[2] = {0, 0};
			bufferPointers[1] = this->instancingHelper->GetStripInstanceBuffer();	
			
			//Per cascade: 
			for(unsigned int i = 0; i < this->csm->GetNrOfCascadeLevels(); ++i)
			{
				//Set depth stencils and view ports.
				this->Dx_DeviceContext->OMSetRenderTargets(0, 0, this->csm->GetShadowMapDSV(i));
				D3D11_VIEWPORT wp = this->csm->GetShadowMapViewPort(i);
				this->Dx_DeviceContext->RSSetViewports(1, &wp);
			
				//Set variables
				this->Shader_ShadowMapInstanced->SetMatrix("g_LightViewProj", this->csm->GetViewProjMatrix(i));

				//Per instance group:
				for(unsigned int j = 0; j < this->instancingHelper->GetNrOfStripGroups(); ++j)
				{
					StripGroup stripGroup = this->instancingHelper->GetStripGroup(j);
					MeshStrip* strip = stripGroup.s_MeshStrip;
					Object3D* renderObject = strip->GetRenderObject();

					//Set topology
					this->Dx_DeviceContext->IASetPrimitiveTopology(renderObject->GetTopology());

					//Set texture for alpha discarding
					if(renderObject->GetTextureResource() != NULL)
					{
						if(renderObject->GetTextureResource()->GetSRVPointer() != NULL)
						{
							this->Shader_ShadowMapInstanced->SetResource("g_DiffuseMap", renderObject->GetTextureResource()->GetSRVPointer());
							this->Shader_ShadowMapInstanced->SetBool("g_IsTextured", true);
						}
						else
						{
							this->Shader_ShadowMapInstanced->SetResource("g_DiffuseMap", NULL);
							this->Shader_ShadowMapInstanced->SetBool("g_IsTextured", false);
						}
					}
					else
					{
						this->Shader_ShadowMapInstanced->SetResource("g_DiffuseMap", NULL);
						this->Shader_ShadowMapInstanced->SetBool("g_IsTextured", false);
					}

					//Change vertex buffer and set it and the instance buffer.
					bufferPointers[0] = renderObject->GetVertexBufferResource()->GetBufferPointer()->GetBufferPointer();
					this->Dx_DeviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

					//**TODO: INDEX BUFFER**

					//Apply pass input layout
					this->Shader_ShadowMapInstanced->Apply(0);

					//Draw instanced
					unsigned int vertexCount = strip->getNrOfVerts();
					int instanceCount = stripGroup.s_Size;
					int startLoc = stripGroup.s_StartLocation;
					this->Dx_DeviceContext->DrawInstanced(vertexCount, instanceCount, 0, startLoc); 

					//Debug data
					this->CurrentNrOfDrawCalls++;
					this->CurrentRenderedNrOfVertices += vertexCount;
				}
			}


			//Reset data for next frame
			this->instancingHelper->PostRenderStrips();
		}
#ifdef MALOWTESTPERF
		this->Dx_DeviceContext->Flush();
		this->perf.PostMeasure("Renderer - Render Cascaded Shadowmap Instanced Static meshes", 4);
	#endif

	#ifdef MALOWTESTPERF
		this->perf.PreMeasure("Renderer - Render Cascaded Shadowmap Instanced Animated meshes", 4);
	#endif
		//ANIMATED MESHES
		if(this->instancingHelper->GetNrOfAnimatedStrips() > 0)
		{
			//Sort, create instance groups and update buffer before rendering
			this->instancingHelper->PreRenderAnimatedStrips(); 

			//Draw meshes(meshstrips)
			//"Set" the instance buffer
			ID3D11Buffer* bufferPointers[3];
			unsigned int strides[3] = {sizeof(VertexNormalMap), sizeof(VertexNormalMap), sizeof(AnimatedStripData::AnimatedInstancedDataStruct)};
			unsigned int offsets[3] = {0, 0, 0};
			bufferPointers[2] = this->instancingHelper->GetAnimatedStripInstanceBuffer();	
			
			//Per cascade:
			for(unsigned int i = 0; i < this->csm->GetNrOfCascadeLevels(); ++i)
			{
				//Set depth stencils and view ports.
				this->Dx_DeviceContext->OMSetRenderTargets(0, 0, this->csm->GetShadowMapDSV(i));
				D3D11_VIEWPORT wp = this->csm->GetShadowMapViewPort(i);
				this->Dx_DeviceContext->RSSetViewports(1, &wp);

				//Set variables
				this->Shader_ShadowMapAnimatedInstanced->SetMatrix("gLightViewProj", this->csm->GetViewProjMatrix(i));

				//Per instance group:
				for(unsigned int j = 0; j < this->instancingHelper->GetNrOfAnimatedStripGroups(); ++j)
				{
					AnimatedStripGroup animatedStripGroup = this->instancingHelper->GetAnimatedStripGroup(j);
					MeshStrip* stripOne = animatedStripGroup.s_MeshStripOne;
					MeshStrip* stripTwo = animatedStripGroup.s_MeshStripTwo;
					Object3D* renderObjectOne = stripOne->GetRenderObject();
					Object3D* renderObjectTwo = stripTwo->GetRenderObject();

					//Set topology (topology is same for both render objects).
					this->Dx_DeviceContext->IASetPrimitiveTopology(renderObjectOne->GetTopology());

					//Set texture for alpha discarding (only first one is used)
					if(renderObjectOne->GetTextureResource() != NULL)
					{
						if(renderObjectOne->GetTextureResource()->GetSRVPointer() != NULL)
						{
							this->Shader_ShadowMapAnimatedInstanced->SetResource("gDiffuseMap0", renderObjectOne->GetTextureResource()->GetSRVPointer());
							this->Shader_ShadowMapAnimatedInstanced->SetBool("gIsTextured", true);
						}
						else
						{
							this->Shader_ShadowMapAnimatedInstanced->SetResource("gDiffuseMap0", NULL);
							this->Shader_ShadowMapAnimatedInstanced->SetBool("gIsTextured", false);
						}
					}
					else
					{
						this->Shader_ShadowMapAnimatedInstanced->SetResource("gDiffuseMap0", NULL);
						this->Shader_ShadowMapAnimatedInstanced->SetBool("gIsTextured", false);
					}

					//Set vertex buffers
					bufferPointers[0] = renderObjectOne->GetVertexBufferResource()->GetBufferPointer()->GetBufferPointer();
					bufferPointers[1] = renderObjectTwo->GetVertexBufferResource()->GetBufferPointer()->GetBufferPointer();
					this->Dx_DeviceContext->IASetVertexBuffers(0, 3, bufferPointers, strides, offsets);

					//Apply pass and input layout.
					this->Shader_ShadowMapAnimatedInstanced->Apply(0);

					//Draw instanced.
					unsigned int vertexCount = stripOne->getNrOfVerts(); //Same for strip two.
					int instanceCount = animatedStripGroup.s_Size;
					int startLoc = animatedStripGroup.s_StartLocation;
					this->Dx_DeviceContext->DrawInstanced(vertexCount, instanceCount, 0, startLoc); 

					//Debug data
					this->CurrentNrOfDrawCalls++;
					this->CurrentRenderedNrOfVertices += vertexCount * 2;
				}
			}

			//Reset data for next frame
			this->instancingHelper->PostRenderAnimatedStrips();
		}
	}
	
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Cascaded Shadowmap Instanced Animated meshes", 4);
#endif
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Cascaded Shadowmap Instanced", 3);
#endif
}

void DxManager::CalculateCulling()
{
	//CAMERA:
	D3DXMATRIX* VP = &this->camera->GetViewProjMatrix();

	// Calculate near plane of frustum.
	FrustrumPlanes[0].a = VP->_14 + VP->_13;
	FrustrumPlanes[0].b = VP->_24 + VP->_23;
	FrustrumPlanes[0].c = VP->_34 + VP->_33;
	FrustrumPlanes[0].d = VP->_44 + VP->_43;
	D3DXPlaneNormalize(&FrustrumPlanes[0], &FrustrumPlanes[0]);

	// Calculate far plane of frustum.
	FrustrumPlanes[1].a = VP->_14 - VP->_13; 
	FrustrumPlanes[1].b = VP->_24 - VP->_23;
	FrustrumPlanes[1].c = VP->_34 - VP->_33;
	FrustrumPlanes[1].d = VP->_44 - VP->_43;
	D3DXPlaneNormalize(&FrustrumPlanes[1], &FrustrumPlanes[1]);

	// Calculate left plane of frustum.
	FrustrumPlanes[2].a = VP->_14 + VP->_11; 
	FrustrumPlanes[2].b = VP->_24 + VP->_21;
	FrustrumPlanes[2].c = VP->_34 + VP->_31;
	FrustrumPlanes[2].d = VP->_44 + VP->_41;
	D3DXPlaneNormalize(&FrustrumPlanes[2], &FrustrumPlanes[2]);

	// Calculate right plane of frustum.
	FrustrumPlanes[3].a = VP->_14 - VP->_11; 
	FrustrumPlanes[3].b = VP->_24 - VP->_21;
	FrustrumPlanes[3].c = VP->_34 - VP->_31;
	FrustrumPlanes[3].d = VP->_44 - VP->_41;
	D3DXPlaneNormalize(&FrustrumPlanes[3], &FrustrumPlanes[3]);

	// Calculate top plane of frustum.
	FrustrumPlanes[4].a = VP->_14 - VP->_12; 
	FrustrumPlanes[4].b = VP->_24 - VP->_22;
	FrustrumPlanes[4].c = VP->_34 - VP->_32;
	FrustrumPlanes[4].d = VP->_44 - VP->_42;
	D3DXPlaneNormalize(&FrustrumPlanes[4], &FrustrumPlanes[4]);

	// Calculate bottom plane of frustum.
	FrustrumPlanes[5].a = VP->_14 + VP->_12;
	FrustrumPlanes[5].b = VP->_24 + VP->_22;
	FrustrumPlanes[5].c = VP->_34 + VP->_32;
	FrustrumPlanes[5].d = VP->_44 + VP->_42;
	D3DXPlaneNormalize(&FrustrumPlanes[5], &FrustrumPlanes[5]);
	
	//Terrain
	for(unsigned int i = 0; i < this->terrains.size(); i++)
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
	for(unsigned int i = 0; i < this->objects.size(); i++)
	{
		StaticMesh* ms = this->objects.get(i);
		if ( ms->GetMeshStripsResourcePointer() != NULL)
		{
			MaloW::Array<MeshStrip*>* strips = ms->GetStrips();
			for(unsigned int u = 0; u < strips->size(); u++)
			{
				MeshStrip* s = strips->get(u);
				float scale = max(ms->GetScalingD3D().x, max(ms->GetScalingD3D().y, ms->GetScalingD3D().z));
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
	for(unsigned int i = 0; i < this->animations.size(); i++)
	{
		AnimatedMesh* animatedMesh = this->animations.get(i);
		if ( animatedMesh->GetKeyFrames()->get(0)->meshStripsResource != NULL)
		{
			if ( !animatedMesh->GetKeyFrames()->get(0)->meshStripsResource->GetMeshStripsPointer()->isEmpty() )
			{
				float scale = max(animatedMesh->GetScalingD3D().x, max(animatedMesh->GetScalingD3D().y, animatedMesh->GetScalingD3D().z));
				MaloW::Array<MeshStrip*>* strips = animatedMesh->GetKeyFrames()->get(0)->meshStripsResource->GetMeshStripsPointer();
				for(unsigned int u = 0; u < strips->size(); u++)
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

	// FBX meshes
	for(unsigned int i = 0; i < this->FBXMeshes.size(); i++)
	{
		FBXMesh* mesh = this->FBXMeshes.get(i);
		float scale = max(mesh->GetScalingD3D().x, max(mesh->GetScalingD3D().y, mesh->GetScalingD3D().z));

		if(pe.FrustrumVsSphere(this->FrustrumPlanes, mesh->GetBoundingSphere(), mesh->GetWorldMatrix(), scale))
		{
			mesh->SetCulled(false);
		}
		else
		{
			mesh->SetCulled(true);
		}
	}

	// Billboards
	for(unsigned int i = 0; i < this->billboards.size(); ++i)
	{
		Billboard* bb = this->billboards.get(i);
		D3DXVECTOR3 pos = bb->GetPositionD3DX();
		D3DXVECTOR3 offset = D3DXVECTOR3(bb->GetSizeD3DX().x, bb->GetSizeD3DX().y, bb->GetSizeD3DX().y) * 0.5f;

		BoundingSphere boundingSphere = BoundingSphere(pos - offset, pos + offset); 
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		world._14 = bb->GetPositionD3DX().x;
		world._24 = bb->GetPositionD3DX().y;
		world._34 = bb->GetPositionD3DX().z;
		float scale = 1.0f;

		if(this->pe.FrustrumVsSphere(this->FrustrumPlanes, boundingSphere, world, scale))
		{
			bb->SetIsCameraCulledFlag(false);
			//If the billboard is inside the frustum, it can cast a shadow
			bb->SetIsShadowCulledFlag(false);
		}
		else
		{
			bb->SetIsCameraCulledFlag(true);
		}
	}

	// Billboard collections
	for(unsigned int i = 0; i < this->billboardCollections.size(); ++i)
	{
		BillboardCollection* bbColl = this->billboardCollections.get(i);
		D3DXVECTOR3 pos = bbColl->CalcWorldPos();
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		world._14 = pos.x;
		world._24 = pos.y;
		world._34 = pos.z;
		float scale = 1.0f;

		if(this->pe.FrustrumVsSphere(this->FrustrumPlanes, BoundingSphere(bbColl->GetMinPos(), bbColl->GetMaxPos()), world, scale))
		{
			bbColl->SetIsCameraCulledFlag(false);
			//If the billboard collection is inside the frustum, it can cast a shadow
			bbColl->SetIsShadowCulledFlag(false);
		}
		else
		{
			bbColl->SetIsCameraCulledFlag(true);
		}
	}

	//SHADOW CULLING - determine if an object is inside a cascade.
	if(this->csm != NULL && this->useShadow) 
	{
		//Calculate frustums - the frustum in this case i an OBB (the cascade). 
		this->csm->CalcCascadePlanes();

		//Terrain
		for(unsigned int i = 0; i < this->terrains.size(); i++)
		{
			Terrain* terrain = this->terrains.get(i);
			float scale = max(terrain->GetScale().x, max(terrain->GetScale().y, terrain->GetScale().z));

			//Terrain already in the cameras view frustum does not need to be checked,
			//so only check the ones that are outside of it. (The ones that have already been culled)
			if(terrain->IsCulled())
			{
				//See if the terrain is inside or intersects the bounding boxes(cascades).
				bool notDone = true;
				for(unsigned int k = 0; k < this->csm->GetNrOfCascadeLevels() && notDone; k++)
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
		for(unsigned int i = 0; i < this->objects.size(); i++)
		{
			StaticMesh* staticMesh = this->objects.get(i);
			float scale = max(staticMesh->GetScalingD3D().x, max(staticMesh->GetScalingD3D().y, staticMesh->GetScalingD3D().z));

			MaloW::Array<MeshStrip*>* strips = staticMesh->GetStrips();
			for(unsigned int j = 0; j < strips->size(); j++)
			{
				MeshStrip* strip = strips->get(j);

				//Objects already in the cameras view frustum does not need to be checked,
				//so only check the ones that are outside of it. (The ones that have already been culled)
				if(staticMesh->IsStripCulled(j))
				{
					//See if the strip is inside the bounding boxes(cascades) or intersects.
					bool notDone = true;
					for(unsigned int k = 0; k < this->csm->GetNrOfCascadeLevels() && notDone; k++)
					{
						if(pe.FrustrumVsSphere(csm->GetCascadePlanes(k), strip->GetBoundingSphere(), staticMesh->GetWorldMatrix(), scale))
						{
							//As long as the strip is inside ONE of the cascades, it needs to be drawn to the shadow map.
							staticMesh->SetStripShadowCulledFlag(j, false); 
							notDone = false;
						}
						else
						{
							staticMesh->SetStripShadowCulledFlag(j, true); 
						}
					}
				}
			}
		}


		//Animated meshes
		for(unsigned int i = 0; i < this->animations.size(); i++)
		{
			AnimatedMesh* animatedMesh = this->animations.get(i);

			if ( !animatedMesh->GetKeyFrames()->get(0)->meshStripsResource->GetMeshStripsPointer()->isEmpty() )
			{
				float scale = max(animatedMesh->GetScalingD3D().x, max(animatedMesh->GetScalingD3D().y, animatedMesh->GetScalingD3D().z));
				
				MaloW::Array<MeshStrip*>* strips = animatedMesh->GetStrips();
				for(unsigned int j = 0; j < strips->size(); j++)
				{
					MeshStrip* strip = strips->get(j);
				
					//Animations already in the cameras view frustum does not need to be checked,
					//so only check the ones that are outside of it. (The ones that have already been culled)
					if(animatedMesh->IsStripCulled(j))
					{
						//See if the strip is inside the bounding boxes(cascades) or intersects.
						bool notDone = true;
						for(unsigned int k = 0; k < this->csm->GetNrOfCascadeLevels() && notDone; k++)
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

		// Billboards
		for(unsigned int i = 0; i < this->billboards.size(); ++i)
		{
			Billboard* bb = this->billboards.get(i);
			D3DXVECTOR3 pos = bb->GetPositionD3DX();
			D3DXVECTOR3 offset = D3DXVECTOR3(bb->GetSizeD3DX().x, bb->GetSizeD3DX().y, bb->GetSizeD3DX().y) * 0.5f;

			BoundingSphere boundingSphere = BoundingSphere(pos - offset, pos + offset); 
			D3DXMATRIX world;
			D3DXMatrixIdentity(&world);
			world._14 = bb->GetPositionD3DX().x;
			world._24 = bb->GetPositionD3DX().y;
			world._34 = bb->GetPositionD3DX().z;
			float scale = 1.0f;
			
			//Billboards already in the cameras view frustum does not need to be checked,
			//so only check the ones that are outside of it. (The ones that have already been culled)
			if(bb->IsCameraCulled())
			{
				//See if the Billboard is inside the bounding boxes(cascades) or intersects.
				bool notDone = true;
				for(unsigned int k = 0; k < this->csm->GetNrOfCascadeLevels() && notDone; k++)
				{
					if(pe.FrustrumVsSphere(csm->GetCascadePlanes(k), boundingSphere, world, scale))
					{
						//As long as the billboard is inside ONE of the cascades, it needs to be drawn to the shadow map.
						bb->SetIsShadowCulledFlag(false); 
						notDone = false;
					}
					else
					{
						bb->SetIsShadowCulledFlag(true); 
					}
				}
			}
		}
		// Billboard collections
		for(unsigned int i = 0; i < this->billboardCollections.size(); ++i)
		{
			BillboardCollection* bbColl = this->billboardCollections.get(i);
			D3DXVECTOR3 pos = bbColl->CalcWorldPos();
			D3DXMATRIX world;
			D3DXMatrixIdentity(&world);
			world._14 = pos.x;
			world._24 = pos.y;
			world._34 = pos.z;
			float scale = 1.0f;
			
			//Billboard collections already in the cameras view frustum does not need to be checked,
			//so only check the ones that are outside of it. (The ones that have already been culled)
			if(bbColl->IsCameraCulled())
			{
				//See if the Billboard is inside the bounding boxes(cascades) or intersects.
				bool notDone = true;
				for(unsigned int k = 0; k < this->csm->GetNrOfCascadeLevels() && notDone; k++)
				{
					if(pe.FrustrumVsSphere(csm->GetCascadePlanes(k), BoundingSphere(bbColl->GetMinPos(), bbColl->GetMaxPos()), world, scale))
					{
						//As long as the billboard collection is inside ONE of the cascades, it needs to be drawn to the shadow map.
						bbColl->SetIsShadowCulledFlag(false); 
						notDone = false;
					}
					else
					{
						bbColl->SetIsShadowCulledFlag(true); 
					}
				}
			}
		}
	}
}

void DxManager::RenderEnclosingFog()
{
	// only draw fog if there should be fog drawn.
	Vector3 camToFogCenter = this->camera->GetPosition() - this->fogCenter;
	float distanceToFogCenter = camToFogCenter.GetLength() + this->params.FarClip;
	if(distanceToFogCenter > this->fogRadius)
	{
		distanceToFogCenter = camToFogCenter.GetLength();
		float overallFogFactor = 0.0f;
		float maxFog = this->fogRadius * this->fogFadeFactor;
		if(distanceToFogCenter > this->fogRadius + maxFog)
			overallFogFactor = 1.0f;
		else
		{
			float curFog = distanceToFogCenter - this->fogRadius;

			overallFogFactor = curFog / maxFog;

			overallFogFactor += overallFogFactor - (overallFogFactor * overallFogFactor);	
			// Exponential fog, making it intense quickly at first and slow at the end.
		}
		if(overallFogFactor < 0.0f)
			overallFogFactor = 0.0f;


		this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
		this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
		this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		this->Shader_FogEnclosement->SetResource("Position", this->Dx_GbufferSRVs[2]);
		this->Shader_FogEnclosement->SetFloat("fogRadius", this->fogRadius);
		this->Shader_FogEnclosement->SetFloat("fogFadeFactor", this->fogFadeFactor);
		this->Shader_FogEnclosement->SetFloat3("center", D3DXVECTOR3(this->fogCenter.x, this->fogCenter.y, this->fogCenter.z));
		this->Shader_FogEnclosement->SetFloat("overallFogFactor", overallFogFactor);
		this->Shader_FogEnclosement->Apply(0);

		this->Dx_DeviceContext->Draw(1, 0);

		this->Shader_FogEnclosement->SetResource("Position", NULL);
		this->Shader_FogEnclosement->Apply(0);
	}

}

void DxManager::Render()
{
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - PreOverhead", 2);
#endif
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
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - PreOverhead", 2);
#endif


#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - CalculateCulling", 2);
#endif
	this->CalculateCulling();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - CalculateCulling", 2);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - PreRender", 2);
#endif
	this->PreRender();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - PreRender", 2);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Shadows", 2);
#endif
	this->RenderShadowMap();
	this->RenderCascadedShadowMap();
	this->RenderCascadedShadowMapInstanced(); //Must be after RenderCascadedShadowMap()
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Shadows", 2);
#endif

	//this->RenderForward();

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo", 2);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Terrains", 3);
#endif
	this->RenderDeferredGeoTerrains();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo Terrains", 3);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Decals", 3);
#endif
	this->RenderDecals();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo Decals", 3);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Objects", 3);
#endif
	this->RenderDeferredGeoObjects();
	this->RenderDeferredGeometryInstanced(); //Must be after RenderDeferredGeometry()
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo Objects", 3);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo FBX", 3);
#endif
	this->RenderFBXMeshes();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo FBX", 3);
#endif

#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo", 2);
#endif


#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Billboards", 2);
#endif
	this->RenderBillboardsInstanced(); //Must be after RenderDeferredGeometry()
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Billboards", 2);
#endif
	
	//this->RenderQuadDeferred();
	//this->RenderDeferredTexture();
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Per Pixel", 2);
#endif
	this->RenderDeferredPerPixel();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Per Pixel", 2);
#endif


#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Translucent", 2);
#endif
	this->RenderDeferredGeoTranslucent();
	this->RenderDeferredPerPixelTranslucent();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Translucent", 2);
#endif
	
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Skybox", 2);
#endif
	this->RenderDeferredSkybox();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Skybox", 2);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Enclosing Fog", 2);
#endif
	if(this->useEnclosingFog)
		this->RenderEnclosingFog();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Enclosing Fog", 2);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Invisible Geo", 2);
#endif
	if(this->invisibleGeometry)
		this->RenderInvisibilityEffect(); 
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Invisible Geo", 2);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Images", 2);
#endif
	this->RenderImages();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Images", 2);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Text", 2);
#endif
	this->RenderText();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Text", 2);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render FXAA", 2);
#endif
	this->RenderAntiAliasing();
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render FXAA", 2);
#endif

	this->PostRender();
	

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
		//DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_Image, this->csm->GetShadowMapSRV(q), q); 
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - SwapChain Present", 2);
#endif
	this->Dx_SwapChain->Present( 0, 0 );
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - SwapChain Present", 2);
#endif
}
