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

void DxManager::SetCamera(SetCameraEvent* ev)
{
	
	Camera* cam = ev->GetCamera();
	Camera* oldCam = this->camera;
	this->camera = cam;
	delete oldCam;
	oldCam = NULL;
	this->DelayGettingCamera = false;
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
		Image* img = new Image(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->params.windowWidth, (float)this->params.windowHeight));
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
				if(!this->objects[i]->IsUsingInvisibility())
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
						else
						{
							Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
						}
					}
				}
			}
		
			//Animated meshes
			for(int i = 0; i < this->animations.size(); i++)
			{
				if(!this->animations[i]->IsUsingInvisibility())
				{
					KeyFrame* one = NULL;
					KeyFrame* two = NULL;
					float t = 0.0f;
					this->animations[i]->SetCurrentTime(this->Timer * 1000.0f); //Timer is in seconds.
					this->animations[i]->GetCurrentKeyFrames(&one, &two, t);
					MaloW::Array<MeshStrip*>* stripsOne = one->strips;
					MaloW::Array<MeshStrip*>* stripsTwo = two->strips;

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
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat4(l, "lights", "LightPosition", D3DXVECTOR4(this->lights[l]->GetPosition(), 1));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsFloat4(l, "lights", "LightColor", D3DXVECTOR4(this->lights[l]->GetColor(), 1));
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
		this->Shader_BillBoard->SetFloat("posx", (img->GetPosition().x / this->params.windowWidth) * 2 - 1);
		this->Shader_BillBoard->SetFloat("posy", 2 - (img->GetPosition().y / this->params.windowHeight) * 2 - 1);
		this->Shader_BillBoard->SetFloat("dimx", (img->GetDimensions().x / this->params.windowWidth) * 2);
		this->Shader_BillBoard->SetFloat("dimy", -(img->GetDimensions().y / this->params.windowHeight) * 2);
		this->Shader_BillBoard->SetFloat("opacity", img->GetOpacity());
		
		/*// if -1 to 1
		this->Shader_BillBoard->SetFloat("posx", img->GetPosition().x);
		this->Shader_BillBoard->SetFloat("posy", img->GetPosition().y);
		this->Shader_BillBoard->SetFloat("dimx", img->GetDimensions().x);
		this->Shader_BillBoard->SetFloat("dimy", img->GetDimensions().y);
		*/
		if(img->GetTexture() != NULL)
		{
			this->Shader_BillBoard->SetResource("tex2D", img->GetTexture()->GetSRVPointer());
		}
		this->Shader_BillBoard->Apply(0);
		this->Dx_DeviceContext->Draw(1, 0);
	}
	this->Shader_BillBoard->SetResource("tex2D", NULL);
	this->Shader_BillBoard->Apply(0);
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
		this->Shader_Text->SetFloat("posx", (txt->GetPosition().x / this->params.windowWidth) * 2 - 1);
		this->Shader_Text->SetFloat("posy", 2 - (txt->GetPosition().y / this->params.windowHeight) * 2 - 1);

		this->Shader_Text->SetFloat("size", txt->GetSize());
		this->Shader_Text->SetFloat("windowWidth", (float)this->params.windowWidth);
		this->Shader_Text->SetFloat("windowHeight", (float)this->params.windowHeight);
		
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

	if(this->useSun && this->useShadow) //* TILLMAN - sun ska också inte ligga här?*
	{
		// Set sun-settings
		this->Shader_DeferredLightning->SetStructMemberAsFloat4("sun", "Direction", D3DXVECTOR4(this->sun.direction, 0.0f));
		this->Shader_DeferredLightning->SetStructMemberAsFloat4("sun", "LightColor", D3DXVECTOR4(this->sun.lightColor, 0.0f));
		this->Shader_DeferredLightning->SetStructMemberAsFloat("sun", "LightIntensity", this->sun.intensity);

		D3DXMATRIX wvp;
		D3DXMatrixIdentity(&wvp);

		for (int l = 0; l < this->csm->GetNrOfCascadeLevels(); l++)
		{
			this->Dx_DeviceContext->OMSetRenderTargets(0, 0, this->csm->GetShadowMapDSV(l));
			D3D11_VIEWPORT wp = this->csm->GetShadowMapViewPort(l);
			this->Dx_DeviceContext->RSSetViewports(1, &wp);
			this->Dx_DeviceContext->ClearDepthStencilView(this->csm->GetShadowMapDSV(l), D3D11_CLEAR_DEPTH, 1.0f, 0);
			
			//Terrain
			for(int i = 0; i < this->terrains.size(); i++)
			{
				//Matrices
				wvp = this->terrains[i]->GetWorldMatrix() * this->csm->GetViewProjMatrix(l);
				this->Shader_ShadowMap->SetMatrix("lightWVP", wvp);
			
				//Input Assembler
				this->Dx_DeviceContext->IASetPrimitiveTopology(this->terrains[i]->GetTopology());

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

				//**TILLMAN EV TODO: (TEXTURE)TRANSPARANCY.

				//Apply Shader
				this->Shader_ShadowMap->Apply(0);

				//Draw
				if(inds)
				{
					this->Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
				}
				else
				{
					this->Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
				}
			}

			//Static meshes
			for(int i = 0; i < this->objects.size(); i++)
			{
				if(!this->objects[i]->IsUsingInvisibility())
				{
					MaloW::Array<MeshStrip*>* strips = this->objects[i]->GetStrips();
					wvp = this->objects[i]->GetWorldMatrix() * this->csm->GetViewProjMatrix(l);
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

						//Apply Shader
						this->Shader_ShadowMap->Apply(0);

						//Draw
						if(inds)
						{
							Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
						}
						else
						{
							Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
						}
					}
				}
			}
			//Unbind shader resources
			this->Shader_ShadowMap->SetResource("diffuseMap", NULL);
			
			//Animated meshes
			for(int i = 0; i < this->animations.size(); i++)
			{
				if(!this->animations[i]->IsUsingInvisibility())
				{
					KeyFrame* one = NULL;
					KeyFrame* two = NULL;
					float t = 0.0f;
					this->animations[i]->SetCurrentTime(this->Timer * 1000.0f); //Timer is in seconds.
					this->animations[i]->GetCurrentKeyFrames(&one, &two, t);
					MaloW::Array<MeshStrip*>* stripsOne = one->strips;
					MaloW::Array<MeshStrip*>* stripsTwo = two->strips;

					//Set shader data (per object)
					this->Shader_ShadowMapAnimated->SetFloat("t", t);
					D3DXMATRIX wvp = this->animations[i]->GetWorldMatrix() * this->csm->GetViewProjMatrix(l);
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
						UINT strides [] = {sizeof(Vertex), sizeof(Vertex)};
						UINT offsets [] = {0, 0};
						this->Dx_DeviceContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);

						//Textures
						if(objOne->GetTextureResource() != NULL && objTwo->GetTextureResource() != NULL)
						{
							if(objOne->GetTextureResource()->GetSRVPointer() != NULL && objTwo->GetTextureResource()->GetSRVPointer() != NULL)
							{
								this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", objOne->GetTextureResource()->GetSRVPointer());
								this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", objTwo->GetTextureResource()->GetSRVPointer());
								this->Shader_ShadowMapAnimated->SetBool("textured", true);
							}
							else
							{
								this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", NULL);
								this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", NULL);
								this->Shader_ShadowMapAnimated->SetBool("textured", false);
							}
						}
						else
						{
							this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", NULL);
							this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", NULL);
							this->Shader_ShadowMapAnimated->SetBool("textured", false);
						}

						//Apply
						this->Shader_ShadowMapAnimated->Apply(0);

						//Draw
						//this->Dx_DeviceContext->Draw(vertsOne->GetElementCount(), 0);
					}
				}
			}
		

			//Unbind shader resources
			this->Shader_ShadowMapAnimated->SetResource("diffuseMap0", NULL);
			this->Shader_ShadowMapAnimated->SetResource("diffuseMap1", NULL);

		
			D3DXMATRIX lvp = this->csm->GetViewProjMatrix(l);

			// For deferred:
			this->Shader_DeferredLightning->SetResourceAtIndex(l, "CascadedShadowMap", this->csm->GetShadowMapSRV(l));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsMatrix(l, "cascades", "viewProj", lvp);
		}
	
			
		float PCF_SIZE = (float)this->params.ShadowMapSettings + 1;
		float PCF_SQUARED = 1 / (PCF_SIZE * PCF_SIZE);

		this->Shader_DeferredLightning->SetFloat("SMAP_DX", 1.0f / (256.0f * pow(2.0f, this->params.ShadowMapSettings / 2.0f)));
		
		this->Shader_DeferredLightning->SetFloat("PCF_SIZE", PCF_SIZE);
		this->Shader_DeferredLightning->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);
		
		this->Shader_DeferredLightning->SetBool("blendCascades", true); //** TILLMAN CSM VARIABLE**
		this->Shader_DeferredLightning->SetFloat("blendDistance", this->csm->GetBlendDistance()); 
		this->Shader_DeferredLightning->SetFloat("blendStrength", 0.0f); //** TILLMAN CSM VARIABLE**

		this->Shader_DeferredLightning->SetInt("nrOfCascades", this->csm->GetNrOfCascadeLevels());
		D3DXVECTOR4 cascadeFarPlanes = D3DXVECTOR4(-1.0f, -1.0f, -1.0f, -1.0f);
		for(int i = 0; i < this->csm->GetNrOfCascadeLevels(); i++)
		{
			cascadeFarPlanes[i] = this->csm->GetSplitDepth(i + 1);
		}
		this->Shader_DeferredLightning->SetFloat4("cascadeFarPlanes", cascadeFarPlanes);
		this->Shader_DeferredLightning->SetMatrix("cameraViewMatrix", this->camera->GetViewMatrix()); //Send camera view matrix to determine what frustum slice to use.
	}
	else
	{
		this->Shader_ShadowMapAnimated->Apply(0); //**TILLMAN - SKA INTE BEHÖVAS, MEN MÅSTE. 
		//** min gissning är att någon variabel (som ligger i stdafx.fx) sätts av denna shader
		//och används en en ANNAN shader och måste därmed applyas. **
	}

	//Always tell the shader whether to use shadows or not.
	this->Shader_DeferredLightning->SetBool("useShadow", this->useShadow);
}

void DxManager::CalculateCulling()
{
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
		}
		else
		{
			terr->SetCulled(true);
		}
	}

	//Static meshes
	for(int i = 0; i < this->objects.size(); i++)
	{
		StaticMesh* ms = this->objects.get(i);
		MaloW::Array<MeshStrip*>* strips = ms->GetStrips();
		for(int u = 0; u < strips->size(); u++)
		{
			MeshStrip* s = strips->get(u);
			float scale = max(ms->GetScaling().x, max(ms->GetScaling().y, ms->GetScaling().z));
			if(pe.FrustrumVsSphere(this->FrustrumPlanes, s->GetBoundingSphere(), ms->GetWorldMatrix(), scale))
			{
				s->SetCulled(false);
			}
			else
			{
				s->SetCulled(true);
			}
		}
	}

	//Animated meshes
	for(int i = 0; i < this->animations.size(); i++)
	{
		AnimatedMesh* ms = this->animations.get(i);
		if ( !ms->GetKeyFrames()->get(0)->strips->isEmpty() )
		{
			MeshStrip* s = ms->GetKeyFrames()->get(0)->strips->get(0);

			float scale = max(ms->GetScaling().x, max(ms->GetScaling().y, ms->GetScaling().z));
			if(pe.FrustrumVsSphere(this->FrustrumPlanes, s->GetBoundingSphere(), ms->GetWorldMatrix(), scale))
			{
				s->SetCulled(false);
			}
			else
			{
				s->SetCulled(true);
			}
		}
	}
}

void DxManager::RenderWaterPlanes()
{

}

HRESULT DxManager::Render()
{
	if(this->RendererSleep > 0)
		Sleep((DWORD)this->RendererSleep);

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
	
	//this->RenderForward();


	this->RenderDeferredGeometry();

	
	//this->RenderQuadDeferred();
	//this->RenderDeferredTexture();

	this->RenderDeferredPerPixel();
	
	if(this->invisibleGeometry)
		this->RenderInvisibilityEffect(); 
	
	this->RenderDeferredSkybox();

	this->RenderParticles();

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
		DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_BillBoard, this->Dx_GbufferSRVs[q], q); 
	*/
	
	
	// Render shadowmap pictures:
	//for(int q = 0; q < this->lights.size(); q++)
		//DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_BillBoard, this->lights[q]->GetShadowMapSRV(), q); 
	//for(int q = 0; q < this->csm->GetNrOfCascadeLevels(); q++)
	//	DrawScreenSpaceBillboardDebug(this->Dx_DeviceContext, this->Shader_BillBoard, this->csm->GetShadowMapSRV(q), q); 

	
	
	if(FAILED(Dx_SwapChain->Present( 0, 0 )))
		return E_FAIL;

	return S_OK;
}
