#include "DxManager.h"

void DxManager::PreRender()
{
	this->CurrentRenderedMeshes = 0;
	this->CurrentRenderedTerrains = 0;
	this->CurrentRenderedFBX = 0;
	this->CurrentRenderedNrOfVertices = 0;
	this->CurrentNrOfDrawCalls = 0;

	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	//Clear render targets
	static float ClearColor1[4] = {0.5f, 0.71f, 1.0f, 1};
	static float ClearColor2[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[0], ClearColor1);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[1], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[2], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[3], ClearColor2);


	if(this->useSun && this->useShadow)	
	{
		this->csm->PreRender(this->sun.direction, this->camera);
	}



	/*
	if(this->Shader_ForwardRendering)
	{

	}
	else
	{
		Shader_DeferredLightning is always used, and is therefore used here.
		
		

		D3DXMATRIX vp = this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix();
		
		this->Shader_DeferredLightning->SetMatrix("CameraVP", vp);
		this->Shader_DeferredLightning->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
		

		this->Shader_DeferredLightning->SetFloat("NrOfLights", (float)this->lights.size()); //**tillman**
		this->Shader_DeferredLightning->SetFloat4("SceneAmbientLight", D3DXVECTOR4(this->sceneAmbientLight, 1.0f));
		
		//use sun
		//sunlight
		
		if(this->animations.size()) this->Shader_DeferredLightning->SetFloat("timerMillis", this->TimerAnimation);
		this->Shader_DeferredLightning->SetInt("windowWidth", this->params.windowWidth);
		this->Shader_DeferredLightning->SetInt("windowHeight", this->params.windowHeight);
		//float NearClip;
		//float FarClip;
	}*/
		
	
}


void DxManager::RenderDeferredGeoTerrains()
{
	//static bool once = false;
	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	//Matrices
	D3DXMATRIX world, view, proj, wvp, worldInverseTranspose;
	D3DXMatrixIdentity(&worldInverseTranspose); //Needed to calculate correct world inverse matrix
	view = this->camera->GetViewMatrix();
	proj = this->camera->GetProjectionMatrix();


	//Terrain - **TILLMAN TODO: MOVE TO LOAD: blendmap, vertexbuffer, textures, aitexture**
	this->Shader_TerrainEditor->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	this->Shader_TerrainEditor->SetFloat("NearClip", this->params.NearClip);
	this->Shader_TerrainEditor->SetFloat("FarClip", this->params.FarClip);

	for(int i = 0; i < this->terrains.size(); i++)
	{
		Terrain* terrPtr = this->terrains.get(i);
		//Check if the terrain has been culled.
		if(terrPtr->IsCulled() == false)
		{
			//Counting(debug)
			CurrentRenderedTerrains++;
			CurrentRenderedNrOfVertices += terrPtr->GetNrOfVertices();

			//Set topology
			this->Dx_DeviceContext->IASetPrimitiveTopology(terrPtr->GetTopology());

			//Calculate matrices & set them
			world = terrPtr->GetWorldMatrix();
			wvp = world * view * proj;

			// Set Shader Variables
			this->Shader_TerrainEditor->SetMatrix("WVP", wvp);
			this->Shader_TerrainEditor->SetMatrix("worldMatrix", world);

			//Update vertex buffer if y-value for vertices (height map) have changed
			if(terrPtr->HasHeightMapChanged() && terrPtr->HaveNormalsChanged()) //**TILLMAN LOADTHREAD**
			{
				void* mappedSubResourceData = NULL;
				//Map to access data in vertex buffer
				mappedSubResourceData = terrPtr->GetVertexBufferPointer()->Map();
				Vertex* data = reinterpret_cast<Vertex*>(mappedSubResourceData);
				//Update data in vertex buffer
				for(unsigned int i = 0; i < terrPtr->GetNrOfVertices(); ++i)
				{
					data[i] = terrPtr->GetVerticesPointer()[i];
				}
				//Unmap so the GPU can access
				terrPtr->GetVertexBufferPointer()->Unmap();
				
				//Set that the height map shall not be changed anymore.
				terrPtr->HeightMapHasChanged(false);
				//Same for normals.
				terrPtr->NormalsHaveChanged(false);
			}

			//Set Textures
			//Reset textures so that previous textures are not used if a texture is missing.
			this->Shader_TerrainEditor->SetResource("tex0", NULL);
			this->Shader_TerrainEditor->SetResource("tex1", NULL);
			this->Shader_TerrainEditor->SetResource("tex2", NULL);
			this->Shader_TerrainEditor->SetResource("tex3", NULL);
			this->Shader_TerrainEditor->SetResource("tex4", NULL);
			this->Shader_TerrainEditor->SetResource("tex5", NULL);
			this->Shader_TerrainEditor->SetResource("tex6", NULL);
			this->Shader_TerrainEditor->SetResource("tex7", NULL);
			//Check if texture(name/path) have changed, create new shader resource view if it has
			for(int j = 0; j < terrPtr->GetNrOfTextures(); j++)
			{
				if(terrPtr->GetTextureResourceToLoadFileName(j) != "")
				{
					//Check if any textures are already loaded.
					if(terrPtr->GetTexture(j) != NULL)
					{
						//Delete(Decrease the reference) current one.
						TextureResource* tmp = terrPtr->GetTexture(j);
						GetResourceManager()->DeleteTextureResource(tmp);
					}
					
					//Create the new one.
					terrPtr->SetTexture(j, GetResourceManager()->CreateTextureResourceFromFile(terrPtr->GetTextureResourceToLoadFileName(j).c_str(), true));
					//Set that the texture resource shall not be changed anymore.
					terrPtr->SetTextureResourceToLoadFileName(j, "");
				}
			}


			//OBS! Do not put this code in a for loop using malow::ConvertNrToString()-function. (Huge performance loss).
			bool hasTexture = false;
			if(terrPtr->GetTexture(0) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex0", terrPtr->GetTexture(0)->GetSRVPointer());
				hasTexture = true;
			}
			if(terrPtr->GetTexture(1) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex1", terrPtr->GetTexture(1)->GetSRVPointer());
				hasTexture = true;
			}
			if(terrPtr->GetTexture(2) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex2", terrPtr->GetTexture(2)->GetSRVPointer());
				hasTexture = true;
			}
			if(terrPtr->GetTexture(3) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex3", terrPtr->GetTexture(3)->GetSRVPointer());
				hasTexture = true;
			}
			if(terrPtr->GetTexture(4) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex4", terrPtr->GetTexture(4)->GetSRVPointer());
				hasTexture = true;
			}
			if(terrPtr->GetTexture(5) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex5", terrPtr->GetTexture(5)->GetSRVPointer());
				hasTexture = true;
			}
			if(terrPtr->GetTexture(6) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex6", terrPtr->GetTexture(6)->GetSRVPointer());
				hasTexture = true;
			}
			if(terrPtr->GetTexture(7) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex7", terrPtr->GetTexture(7)->GetSRVPointer());
				hasTexture = true;
			}


			if(hasTexture) 
			{
				//Check if blend map is used
				BlendMap* bmPtr0 = terrPtr->GetBlendMapPointer(0); 
				if(bmPtr0 != NULL)
				{
					if(bmPtr0->s_HasChanged)
					{
						//Check if blendmap texture has to be recreated.
						if(bmPtr0->s_RecreateTexture)
						{
							//Release old shader resource view 
							if(bmPtr0->s_SRV) bmPtr0->s_SRV->Release();

							//Create texture
							int widthOrHeight = bmPtr0->s_Size;
							D3D11_TEXTURE2D_DESC texDesc;
							texDesc.Width = widthOrHeight;
							texDesc.Height = widthOrHeight;
							texDesc.MipLevels = 1;
							texDesc.ArraySize = 1;
							texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
							texDesc.SampleDesc.Count = 1;
							texDesc.SampleDesc.Quality = 0;
							texDesc.Usage = D3D11_USAGE_DYNAMIC; //GPU READ, CPU WRITE
							texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
							texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU WRITE portion/all
							texDesc.MiscFlags = 0;

							D3D11_SUBRESOURCE_DATA initData = {0};
							initData.SysMemPitch = widthOrHeight * sizeof(float) * 4;
							initData.pSysMem = bmPtr0->s_Data; 

							if(FAILED(this->Dx_Device->CreateTexture2D(&texDesc, &initData, &bmPtr0->s_Texture)))
							{
								MaloW::Debug("ERROR: Failed to create texture with blend map data.");
							}
							//Set that the blend map's texture shall not be recreated.
							bmPtr0->s_RecreateTexture = false;
						}
						else //otherwise update texture
						{
							D3D11_MAPPED_SUBRESOURCE mappedSubResource;
							this->Dx_DeviceContext->Map(bmPtr0->s_Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
							float* data = reinterpret_cast<float*>(mappedSubResource.pData);
							unsigned int arraySize = bmPtr0->s_Size * bmPtr0->s_Size * 4; //Width * height * 4 floats
							for(unsigned int i = 0; i < arraySize; ++i)
							{
								data[i] = bmPtr0->s_Data[i];
							}
							//Unmap so the GPU can have access
							this->Dx_DeviceContext->Unmap(bmPtr0->s_Texture, 0);
						}

						//Create shader resource view (always, cannot be updated)
						if(FAILED(this->Dx_Device->CreateShaderResourceView(bmPtr0->s_Texture, 0, &bmPtr0->s_SRV)))
						{
							MaloW::Debug("ERROR: Failed to create Shader resource view with blend map texture.");
						}

						//Set that the blend map shall not be changed anymore.
						bmPtr0->s_HasChanged = false;
					}

					BlendMap* bmPtr1 = terrPtr->GetBlendMapPointer(1);  //**tillman ändra**
					if(bmPtr1 != NULL)
					{
						if(bmPtr1->s_HasChanged)
						{
							//Check if blendmap texture has to be recreated.
							if(bmPtr1->s_RecreateTexture)
							{
								//Release old shader resource view 
								if(bmPtr1->s_SRV) bmPtr1->s_SRV->Release();

								//Create texture
								int widthOrHeight = bmPtr1->s_Size;
								D3D11_TEXTURE2D_DESC texDesc;
								texDesc.Width = widthOrHeight;
								texDesc.Height = widthOrHeight;
								texDesc.MipLevels = 1;
								texDesc.ArraySize = 1;
								texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
								texDesc.SampleDesc.Count = 1;
								texDesc.SampleDesc.Quality = 0;
								texDesc.Usage = D3D11_USAGE_DYNAMIC; //GPU READ, CPU WRITE
								texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
								texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU WRITE portion/all
								texDesc.MiscFlags = 0;

								D3D11_SUBRESOURCE_DATA initData = {0};
								initData.SysMemPitch = widthOrHeight * sizeof(float) * 4;
								initData.pSysMem = bmPtr1->s_Data; 

								if(FAILED(this->Dx_Device->CreateTexture2D(&texDesc, &initData, &bmPtr1->s_Texture)))
								{
									MaloW::Debug("ERROR: Failed to create texture with blend map data.");
								}
								//Set that the blend map's texture shall not be recreated.
								bmPtr1->s_RecreateTexture = false;
							}
							else //otherwise update texture
							{
								D3D11_MAPPED_SUBRESOURCE mappedSubResource;
								this->Dx_DeviceContext->Map(bmPtr1->s_Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
								float* data = reinterpret_cast<float*>(mappedSubResource.pData);
								unsigned int arraySize = bmPtr1->s_Size * bmPtr1->s_Size * 4; //Width * height * 4 floats
								for(unsigned int i = 0; i < arraySize; ++i)
								{
									data[i] = bmPtr1->s_Data[i];
								}
								//Unmap so the GPU can have access
								this->Dx_DeviceContext->Unmap(bmPtr1->s_Texture, 0);
							}

							//Create shader resource view (always, cannot be updated)
							if(FAILED(this->Dx_Device->CreateShaderResourceView(bmPtr1->s_Texture, 0, &bmPtr1->s_SRV)))
							{
								MaloW::Debug("ERROR: Failed to create Shader resource view with blend map texture.");
							}

							//Set that the blend map shall not be changed anymore.
							bmPtr1->s_HasChanged = false;
						}

						//Set blend map variables
						this->Shader_TerrainEditor->SetResource("blendMap1", bmPtr1->s_SRV);
					}

					//Set blend map variables
					this->Shader_TerrainEditor->SetBool("blendMapped", true);
					this->Shader_TerrainEditor->SetInt("nrOfBlendMaps", terrPtr->GetNrOfBlendMaps());
					this->Shader_TerrainEditor->SetResource("blendMap0", bmPtr0->s_SRV);
				}
				else
				{
					//Set blend map  variables
					this->Shader_TerrainEditor->SetBool("blendMapped", false);
				}

				//Set that textures shall be used and its scale
				this->Shader_TerrainEditor->SetBool("textured", true);
				this->Shader_TerrainEditor->SetFloat("textureScale", terrPtr->GetTextureScale());
			}
			else
			{
				//Set texture variable
				this->Shader_TerrainEditor->SetBool("textured", false);
			}

			//Editor/AI
			if(terrPtr->HasAIGridChanged())
			{
				ID3D11ShaderResourceView*& aiMap = terrPtr->GetAIShaderResourceView();
				//Release old shader resource view //**TILLMAN LOAD-THREAD**
				if(aiMap != NULL) aiMap->Release();
				
				//Create new one.
				unsigned int widthOrHeight = terrPtr->GetNrOfAINodesPerSide();
				D3D11_TEXTURE2D_DESC texDesc;
				texDesc.Width = widthOrHeight;
				texDesc.Height = widthOrHeight;
				texDesc.MipLevels = 1;
				texDesc.ArraySize = 1;
				texDesc.Format = DXGI_FORMAT_R8_UNORM; //node values are stored as bits. (true or false - 1 or 0).
				texDesc.SampleDesc.Count = 1;
				texDesc.SampleDesc.Quality = 0;
				texDesc.Usage = D3D11_USAGE_IMMUTABLE;
				texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				texDesc.CPUAccessFlags = 0;
				texDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA initData = {0};
				initData.SysMemPitch = widthOrHeight * 1; 
				initData.pSysMem = terrPtr->GetAIData();  

				ID3D11Texture2D* tex = NULL;
				if(FAILED(this->Dx_Device->CreateTexture2D(&texDesc, &initData, &tex)))
				{
					MaloW::Debug("ERROR: Failed to create texture with AI map data.");
				}
				//Create shader resource view
				if(FAILED(this->Dx_Device->CreateShaderResourceView(tex, 0, &aiMap)))
				{
					MaloW::Debug("ERROR: Failed to create Shader resource view with AI map texture.");
				}

				//Set that the AI map shall not be changed anymore.
				terrPtr->AIGridHasChanged(false);
			}
			//Set AIMAP
			if(terrPtr->GetAIShaderResourceView() != NULL)
			{
				this->Shader_TerrainEditor->SetBool("useAIMap", terrPtr->UseAIMap());
				this->Shader_TerrainEditor->SetFloat("nodesPerSide", terrPtr->GetNrOfAINodesPerSide());
				this->Shader_TerrainEditor->SetResource("AIMap", terrPtr->GetAIShaderResourceView());
				this->Shader_TerrainEditor->SetFloat("AIGridThickness", terrPtr->GetAIGridThickness());
			}
			else
			{
				this->Shader_TerrainEditor->SetResource("AIMap", NULL);
				this->Shader_TerrainEditor->SetBool("useAIMap", false);
			}
			
			//Set lighting from material
			//Specular is not used for terrain
			this->Shader_TerrainEditor->SetFloat3("diffuseColor", terrPtr->GetMaterial()->DiffuseColor);

			//Apply vertices & indices & shader
			Buffer* vertices = terrPtr->GetVertexBufferPointer();
			if(vertices) 
			{
				if(FAILED(vertices->Apply()))
				{
					MaloW::Debug("ERROR: Could not apply vertices for terrain.");
				}
			}
			else
			{
				MaloW::Debug("ERROR: Could not apply vertices for terrain. REASON: vertex buffer has not been created.");
			}
			Buffer* indices = terrPtr->GetIndexBufferPointer();
			if(indices) 
			{
				if(FAILED(indices->Apply()))
				{
					MaloW::Debug("ERROR: Could not apply indices for terrain.");
				}
			}
			else
			{
				MaloW::Debug("ERROR: Could not apply indices for terrain.  REASON: index buffer has not been created");
			}
			if(FAILED(this->Shader_TerrainEditor->Apply(0)))
			{
				MaloW::Debug("ERROR: Could not apply shader for terrain.");
			}

			//Draw
			if(indices)
			{
				this->Dx_DeviceContext->DrawIndexed(indices->GetElementCount(), 0, 0);
				//Count(debug)
				CurrentNrOfDrawCalls++;
			}
			else if(vertices)
			{
				this->Dx_DeviceContext->Draw(vertices->GetElementCount(), 0);
				//Count(debug)
				CurrentNrOfDrawCalls++;
			}
			else
			{
				MaloW::Debug("WARNING: DxManagerDeferred: RenderDeferredGeometry(): Both index and vertex buffers were NULL for the terrain.");
			}
		}
	}

	//Unbind terrain resources //**TILLMAN, onödigt?**
	this->Shader_TerrainEditor->SetResource("tex0", NULL);
	this->Shader_TerrainEditor->SetResource("tex1", NULL);
	this->Shader_TerrainEditor->SetResource("tex2", NULL);
	this->Shader_TerrainEditor->SetResource("tex3", NULL);
	this->Shader_TerrainEditor->SetResource("tex4", NULL);
	this->Shader_TerrainEditor->SetResource("tex5", NULL);
	this->Shader_TerrainEditor->SetResource("tex6", NULL);
	this->Shader_TerrainEditor->SetResource("tex7", NULL);
	this->Shader_TerrainEditor->SetResource("blendMap0", NULL);
	this->Shader_TerrainEditor->SetResource("blendMap1", NULL);
	this->Shader_TerrainEditor->SetResource("AIMap", NULL);
	this->Shader_TerrainEditor->Apply(0);
}

void DxManager::RenderDecals()
{
	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_GbufferRTs[0], this->Dx_DepthStencilView);
	D3DXMATRIX proj = this->camera->GetProjectionMatrix();
	D3DXMATRIX view = this->camera->GetViewMatrix();
	D3DXMATRIX viewProj = view * proj;


	this->Shader_Decal->SetFloat2("PixelSize", D3DXVECTOR2(1.0f / this->params.WindowWidth, 1.0f / this->params.WindowHeight));

	this->Shader_Decal->SetResource("Depth", this->Dx_GbufferSRVs[1]);

	for(int i = 0; i < this->decals.size(); i++)
	{
		this->Shader_Decal->SetMatrix("World", this->decals[i]->GetWorldMatrix());
		this->Shader_Decal->SetMatrix("WorldViewProj", this->decals[i]->GetWorldMatrix() * viewProj);
		this->Shader_Decal->SetResource("Decal", this->decals[i]->GetTextureResource()->GetSRVPointer());
		this->Shader_Decal->SetMatrix("ScreenToLocal", this->decals[i]->GetMatrix());
		this->Shader_Decal->SetFloat("opacity", this->decals[i]->GetOpacity());

		Buffer* verts = this->decals[i]->GetStrip()->GetRenderObject()->GetVertBuff();
		verts->Apply();
		Buffer* inds = this->decals[i]->GetStrip()->GetRenderObject()->GetIndsBuff();
		inds->Apply();
		this->Shader_Decal->Apply(0);

		this->Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
	}

	this->Shader_Decal->SetResource("Depth", NULL);
	this->Shader_Decal->SetResource("Decal", NULL);
	this->Shader_Decal->Apply(0);
}

void DxManager::RenderDeferredGeoObjects()
{
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Objects Static", 4);
#endif

	this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);

	D3DXMATRIX world, view, proj, wvp, worldInverseTranspose;
	D3DXMatrixIdentity(&worldInverseTranspose); //Needed to calculate correct world inverse matrix
	view = this->camera->GetViewMatrix();
	proj = this->camera->GetProjectionMatrix();

	//Static meshes
	//**TILLMAN TODO: ta bort shader variabler
	this->Shader_DeferredGeometry->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	this->Shader_DeferredGeometry->SetFloat("NearClip", this->params.NearClip);
	this->Shader_DeferredGeometry->SetFloat("FarClip", this->params.FarClip);

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
				//**TILLMAN TODO: ta bort shader variabler
				MaloW::Array<MeshStrip*>* strips = staticMesh->GetStrips();
		
				// Per object
				this->Shader_DeferredGeometry->SetInt("specialColor", staticMesh->GetSpecialColor());

				// Set matrices
				world = this->objects[i]->GetWorldMatrix();
				wvp = world * view * proj;
				D3DXMatrixInverse(&worldInverseTranspose, NULL, &world);
				D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);

				this->Shader_DeferredGeometry->SetMatrix("WVP", wvp);
				this->Shader_DeferredGeometry->SetMatrix("worldMatrix", world);
				this->Shader_DeferredGeometry->SetMatrix("worldMatrixInverseTranspose", worldInverseTranspose);

				bool hasBeenCounted = false;
				
				
				for(int u = 0; u < strips->size(); u++)
				{
					if(!staticMesh->IsStripCulled(u))
					{
						if(!hasBeenCounted)
						{
							CurrentRenderedMeshes++;
							hasBeenCounted = true;
						}

						//Count vertices(debug) 
						CurrentRenderedNrOfVertices += strips->get(u)->getNrOfVerts();

						Object3D* obj = strips->get(u)->GetRenderObject();
						this->Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());

						// Setting lightning from material
						this->Shader_DeferredGeometry->SetFloat4("SpecularColor", D3DXVECTOR4(strips->get(u)->GetMaterial()->SpecularColor, 1));
						this->Shader_DeferredGeometry->SetFloat("SpecularPower", strips->get(u)->GetMaterial()->SpecularPower);
						this->Shader_DeferredGeometry->SetFloat4("AmbientLight", D3DXVECTOR4(strips->get(u)->GetMaterial()->AmbientColor, 1));
						this->Shader_DeferredGeometry->SetFloat4("DiffuseColor", D3DXVECTOR4(strips->get(u)->GetMaterial()->DiffuseColor, 1));

						Buffer* verts = obj->GetVertBuff();
						if(verts)
							verts->Apply();

						//Check if the mesh uses a texture resource.
						if(obj->GetTextureResource() != NULL)
						{
							//Then check if it actually has a shader resource view.
							if(ID3D11ShaderResourceView* texture = obj->GetTextureResource()->GetSRVPointer())///**TILLMAN
							{
								this->Shader_DeferredGeometry->SetBool("textured", true);
								this->Shader_DeferredGeometry->SetResource("tex2D", texture);

								if(obj->GetNormalMapResource())
								{
									if(ID3D11ShaderResourceView* normalMap = obj->GetNormalMapResource()->GetSRVPointer())
									{
										this->Shader_DeferredGeometry->SetBool("useNormalMap", true);
										this->Shader_DeferredGeometry->SetResource("normalMap", normalMap);
									}
									else
									{
										this->Shader_DeferredGeometry->SetBool("useNormalMap", false);
										this->Shader_DeferredGeometry->SetResource("normalMap", NULL);
									}
								}
								else
								{
									this->Shader_DeferredGeometry->SetBool("useNormalMap", false);
									this->Shader_DeferredGeometry->SetResource("normalMap", NULL);
								}
							}
							else //else set texture variables to not be used
							{
								this->Shader_DeferredGeometry->SetBool("textured", false);
								this->Shader_DeferredGeometry->SetResource("tex2D", NULL);
								this->Shader_DeferredGeometry->SetBool("useNormalMap", false);
								this->Shader_DeferredGeometry->SetResource("normalMap", NULL);
							}
						}
						else //else set texture variables to not be used
						{
							this->Shader_DeferredGeometry->SetBool("textured", false);
							this->Shader_DeferredGeometry->SetResource("tex2D", NULL);
							this->Shader_DeferredGeometry->SetBool("useNormalMap", false);
							this->Shader_DeferredGeometry->SetResource("normalMap", NULL);
						}
						Buffer* inds = obj->GetIndsBuff();
						if(inds)
							inds->Apply();
			
						this->Shader_DeferredGeometry->Apply(0);

						//Draw
						if(inds)
						{
							this->Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
							//Count(debug)
							CurrentNrOfDrawCalls++;
						}
						else if(verts)
						{
							this->Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
							//Count(debug)
							CurrentNrOfDrawCalls++;
						}
						else
						{
							MaloW::Debug("WARNING: DxManagerDeferred: RenderDeferredGeometry(): Both index and vertex buffers were NULL for static mesh. ");
						}
						


					}
				}
					
						
				//Just check the first strip if it is culled.  //**TILLMAN**
				/*if(!staticMesh->IsStripCulled(0))
				{
					CurrentRenderedMeshes++;

					//Add mesh info to instance helper
					this->instancingHelper->AddMesh(staticMesh);
				}*/
			}
			else
			{
				//Just check the first strip if it is culled.  //**TILLMAN**
				if(!staticMesh->IsStripCulled(0))
				{
					CurrentRenderedMeshes++;

					//Add billboard info
					this->instancingHelper->AddBillboard(staticMesh);
				}
			}
		}
		else
		{
			this->invisibleGeometry = true;
		}
	}
	//once = false;
	// Unbind resources static geometry:
	this->Shader_DeferredGeometry->SetResource("normalMap", NULL);
	this->Shader_DeferredGeometry->SetResource("tex2D", NULL);
	this->Shader_DeferredGeometry->Apply(0);

#ifdef MALOWTESTPERF
	this->perf.PostMeasure("Renderer - Render Deferred Geo Objects Static", 4);
#endif
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Objects Animated", 4);
#endif
	
	// Normal Animated meshes
	this->Shader_DeferredAnimatedGeometry->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	this->Shader_DeferredAnimatedGeometry->SetFloat("NearClip", this->params.NearClip);
	this->Shader_DeferredAnimatedGeometry->SetFloat("FarClip", this->params.FarClip);
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
				if(!animatedMesh->IsStripCulled(0))
				{
					//Count(debug)
					for(int o = 0; o < animatedMesh->GetKeyFrames()->get(0)->meshStripsResource->GetMeshStripsPointer()->size(); ++o)
					{
						CurrentRenderedNrOfVertices += animatedMesh->GetKeyFrames()->get(0)->meshStripsResource->GetMeshStripsPointer()->get(o)->getNrOfVerts();
					}
					CurrentRenderedMeshes++;			

					KeyFrame* one = NULL;
					KeyFrame* two = NULL;
					float t = 0.0f;
					animatedMesh->SetCurrentTime(this->Timer * 1000.0f); //Timer is in seconds.
					animatedMesh->GetCurrentKeyFrames(&one, &two, t);

					MaloW::Array<MeshStrip*>* stripsOne = one->meshStripsResource->GetMeshStripsPointer();
					MaloW::Array<MeshStrip*>* stripsTwo = two->meshStripsResource->GetMeshStripsPointer();
		
					// Set matrixes
					world = animatedMesh->GetWorldMatrix();
					wvp = world * view * proj;
					D3DXMatrixInverse(&worldInverseTranspose, NULL, &world);
					D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);
		
					this->Shader_DeferredAnimatedGeometry->SetMatrix("WVP", wvp);
					this->Shader_DeferredAnimatedGeometry->SetMatrix("worldMatrix", world);
					this->Shader_DeferredAnimatedGeometry->SetMatrix("worldMatrixInverseTranspose", worldInverseTranspose);
					this->Shader_DeferredAnimatedGeometry->SetFloat("t", t);

					this->Shader_DeferredAnimatedGeometry->SetInt("specialColor", animatedMesh->GetSpecialColor()); //*Tillman old: kraschar för att antalet animationer > antalet object

					for(int u = 0; u < stripsOne->size(); u++)
					{
						Object3D* objOne = stripsOne->get(u)->GetRenderObject();
						Object3D* objTwo = stripsTwo->get(u)->GetRenderObject();

						this->Dx_DeviceContext->IASetPrimitiveTopology(objOne->GetTopology());

						// Setting lightning from material
						this->Shader_DeferredAnimatedGeometry->SetFloat4("SpecularColor", D3DXVECTOR4(stripsOne->get(u)->GetMaterial()->SpecularColor, 1)); //MaloW Opt
						this->Shader_DeferredAnimatedGeometry->SetFloat("SpecularPower", stripsOne->get(u)->GetMaterial()->SpecularPower);
						//this->Shader_DeferredAnimatedGeometry->SetFloat4("AmbientLight", D3DXVECTOR4(stripsOne->get(u)->GetMaterial()->AmbientColor, 1)); //MaloW Opt
						this->Shader_DeferredAnimatedGeometry->SetFloat4("DiffuseColor", D3DXVECTOR4(stripsOne->get(u)->GetMaterial()->DiffuseColor, 1)); //MaloW Opt
							
						Buffer* vertsOne = objOne->GetVertBuff();
						Buffer* vertsTwo = objTwo->GetVertBuff();

						ID3D11Buffer* vertexBuffers [] = {vertsOne->GetBufferPointer(), vertsTwo->GetBufferPointer()};
						UINT strides [] = {sizeof(VertexNormalMap), sizeof(VertexNormalMap)};
						UINT offsets [] = {0, 0};

						this->Dx_DeviceContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
				
						//Check if the mesh(es) uses a texture resource.
						if(objOne->GetTextureResource() != NULL)
						{
							//Then check if it actually has a shader resource view.
							if(ID3D11ShaderResourceView* texture = objOne->GetTextureResource()->GetSRVPointer())//**TILLMAN
							{
								this->Shader_DeferredAnimatedGeometry->SetBool("textured", true);
								this->Shader_DeferredAnimatedGeometry->SetResource("tex2D", texture);

								if(objOne->GetNormalMapResource())
								{
									if(ID3D11ShaderResourceView* normalMap = objOne->GetNormalMapResource()->GetSRVPointer())
									{
										this->Shader_DeferredAnimatedGeometry->SetBool("useNormalMap", true);
										this->Shader_DeferredAnimatedGeometry->SetResource("normalMap", normalMap);
									}
									else
									{
										this->Shader_DeferredAnimatedGeometry->SetBool("useNormalMap", false);
										this->Shader_DeferredAnimatedGeometry->SetResource("normalMap", NULL);
									}
								}
								else
								{
									this->Shader_DeferredAnimatedGeometry->SetBool("useNormalMap", false);
									this->Shader_DeferredAnimatedGeometry->SetResource("normalMap", NULL);
								}
							}
							else //else set texture variables to not be used
							{
								this->Shader_DeferredAnimatedGeometry->SetBool("textured", false);
								this->Shader_DeferredAnimatedGeometry->SetResource("tex2D", NULL);
								this->Shader_DeferredAnimatedGeometry->SetBool("useNormalMap", false);
								this->Shader_DeferredAnimatedGeometry->SetResource("normalMap", NULL);
							}
						}
						else //else set texture variables to not be used
						{
							this->Shader_DeferredAnimatedGeometry->SetBool("textured", false);
							this->Shader_DeferredAnimatedGeometry->SetResource("tex2D", NULL);
							this->Shader_DeferredAnimatedGeometry->SetBool("useNormalMap", false);
							this->Shader_DeferredAnimatedGeometry->SetResource("normalMap", NULL);
						}

						this->Shader_DeferredAnimatedGeometry->Apply(0);
						//Draw
						this->Dx_DeviceContext->Draw(vertsOne->GetElementCount(), 0);

						//Count(debug)
						CurrentNrOfDrawCalls++;
					}
				}
			}
			else
			{
				//set variables used per frame
				this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
				this->Shader_Billboard->SetFloat3("g_CameraPos", this->camera->GetPositionD3DX());
				this->Shader_Billboard->SetMatrix("g_CamViewProj", this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());

				//**CULLING _ TILLMAN**
				bool hasBeenCounted = false;
				
				//Just check the first strip if it is culled. //**TILLMAN**
				if(!animatedMesh->IsStripCulled(0))
				{
					if(!hasBeenCounted)
					{
						CurrentRenderedMeshes++;
						hasBeenCounted = true;
					}

					//Add billboard info
					this->instancingHelper->AddBillboard(animatedMesh);
				}


				//Unbind resources ** TILLMAN
				this->Shader_Billboard->SetResource("g_bb_DiffuseMap", NULL);
				this->Shader_Billboard->Apply(0);
			}
		}
		else
		{
			this->invisibleGeometry = true;
		}
	}
	//Unbind resources animated geometry:
	this->Shader_DeferredAnimatedGeometry->SetResource("tex2D", NULL);
	this->Shader_DeferredAnimatedGeometry->Apply(0);

	this->RenderedMeshes = CurrentRenderedMeshes;
	this->RenderedTerrains = CurrentRenderedTerrains;

	this->NrOfDrawnVertices = CurrentRenderedNrOfVertices;
	this->NrOfDrawCalls = CurrentNrOfDrawCalls;

#ifdef MALOWTESTPERF
	this->perf.PostMeasure("Renderer - Render Deferred Geo Objects Animated", 4);
#endif
}
void DxManager::RenderDeferredGeometryInstanced()
{
	if(this->instancingHelper->GetNrOfStrips() > 0)
	{
		//Sort, create instance groups and update buffer before rendering
		//static bool once = false;
		//if(!once)
		{

		
			this->instancingHelper->PreRenderStrips();
			//once = true;
		//	this->instancingHelper->SetBoolTest(once);
		}


		// Set global variables per frame
		this->Shader_DeferredGeometryInstanced->SetFloat3("g_CamPos", this->camera->GetPositionD3DX());
		this->Shader_DeferredGeometryInstanced->SetMatrix("g_CamViewProj", this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix());
		this->Shader_DeferredGeometryInstanced->SetFloat("g_FarClip", this->params.FarClip);
		
		//Set instance buffer
		ID3D11Buffer* bufferPointers[2];
		unsigned int strides[2] = {sizeof(VertexNormalMap), sizeof(StripData::InstancedDataStruct)};
		unsigned int offsets[2] = {0, 0};
		bufferPointers[1] = this->instancingHelper->GetStripInstanceBuffer();	

		//for(unsigned int jj = 0; jj < 3; ++jj)
		//{


			//Per Strip group
		for(unsigned int i = 0; i < this->instancingHelper->GetNrOfStripGroups(); ++i)//** 1 tillman
		//for(unsigned int i = 0; i < 1; ++i)
		{
			StripGroup stripGroup = this->instancingHelper->GetStripGroup(i);
			MeshStrip* strip = stripGroup.s_MeshStrip;

			


			D3DXMATRIX worldTest;
			D3DXMatrixIdentity(&worldTest);
			//this->Shader_DeferredGeometryInstanced->SetMatrix("g_TestW", worldTest);
			D3D11_MAPPED_SUBRESOURCE mappedSubResource; 
			//Map to access data
			this->Dx_DeviceContext->Map(this->instancingHelper->GetStripInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
			StripData::InstancedDataStruct* dataView = reinterpret_cast<StripData::InstancedDataStruct*>(mappedSubResource.pData);
			//Copy over all instance data
			//for(UINT i = 0; i < this->instancingHelper->GetNrOfMeshes(); ++i)
			//{
			/*D3DXMATRIX testW = D3DXMATRIX(
				dataView[i].x.x, dataView[i].x.y, dataView[i].x.z, dataView[i].x.w, 
				dataView[i].y.x, dataView[i].y.y, dataView[i].y.z, dataView[i].y.w, 
				dataView[i].z.x, dataView[i].z.y, dataView[i].z.z, dataView[i].z.w, 
				dataView[i].w.x, dataView[i].w.y, dataView[i].w.z, dataView[i].w.w);
			
				this->Shader_DeferredGeometryInstanced->SetMatrix("g_TestW", testW);
				*/
				this->Shader_DeferredGeometryInstanced->SetMatrix("g_TestW", dataView[i].s_WorldMatrix);
			//}
			//Unmap so the GPU can have access
			this->Dx_DeviceContext->Unmap(this->instancingHelper->GetStripInstanceBuffer(), 0);

			//this->Shader_DeferredGeometryInstanced->SetMatrix("g_TestW", this->instancingHelper->GetMeshData(0).InstancedData.s_WorldMatrix);
			//this->Shader_DeferredGeometryInstanced->SetMatrix("g_TestWIT", this->instancingHelper->GetMeshData(0).InstancedData.s_WorldInverseTransposeMatrix);
			





			Object3D* renderObject = strip->GetRenderObject();

			//Set topology
			this->Dx_DeviceContext->IASetPrimitiveTopology(renderObject->GetTopology());

			// Setting lightning from material
			this->Shader_DeferredGeometryInstanced->SetFloat4("g_DiffuseColor", D3DXVECTOR4(strip->GetMaterial()->DiffuseColor, 1));
			this->Shader_DeferredGeometryInstanced->SetFloat4("g_SpecularColor", D3DXVECTOR4(strip->GetMaterial()->SpecularColor, 1));
			this->Shader_DeferredGeometryInstanced->SetFloat("g_SpecularPower", strip->GetMaterial()->SpecularPower);
				
			//Change vertex buffer and set it and the instance buffer.
			bufferPointers[0] = renderObject->GetVertexBufferResource()->GetBufferPointer()->GetBufferPointer();
			this->Dx_DeviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
				
			//Set texture
			if(renderObject->GetTextureResource() != NULL)
			{
				if(renderObject->GetTextureResource()->GetSRVPointer() != NULL)
				{
					this->Shader_DeferredGeometryInstanced->SetResource("g_DiffuseMap", renderObject->GetTextureResource()->GetSRVPointer());
					this->Shader_DeferredGeometryInstanced->SetBool("g_Textured", true);
				}
				else
				{
					this->Shader_DeferredGeometryInstanced->SetResource("g_DiffuseMap", NULL);
					this->Shader_DeferredGeometryInstanced->SetBool("g_Textured", false);
				}
			}
			else
			{
				this->Shader_DeferredGeometryInstanced->SetResource("g_DiffuseMap", NULL);
				this->Shader_DeferredGeometryInstanced->SetBool("g_Textured", false);
			}
			//**TILLMAN TODO: normalmap**

			//Apply pass and input layout
			this->Shader_DeferredGeometryInstanced->Apply(0);
				
			//Draw
			unsigned int vertexCount = strip->getNrOfVerts();
			int instanceCount = this->instancingHelper->GetStripGroup(i).s_Size;
			int startLoc = this->instancingHelper->GetStripGroup(i).s_StartLocation;
			this->Dx_DeviceContext->DrawInstanced(vertexCount, instanceCount, 0, startLoc); //**tillman
			
			//Debug data
			this->NrOfDrawCalls++;
			this->NrOfDrawnVertices += vertexCount;
		}
		//}



		//Reset data.
		this->instancingHelper->PostRenderStrips();
	}
}

void DxManager::RenderDeferredSkybox()
{
	if(!this->skybox)
		return;

		// Set matrixes
	D3DXMATRIX world, wvp, view, proj;
	view = this->camera->GetViewMatrix();
	proj = this->camera->GetProjectionMatrix();
	world = this->skybox->GetSkyboxWorldMatrix(this->camera->GetOldPos(), this->params.FarClip);
	wvp = world * view * proj;
	
	this->Shader_Skybox->SetMatrix("gWVP", wvp);
	this->Shader_Skybox->SetMatrix("world", world);
	this->Shader_Skybox->SetFloat("FogHeight", this->camera->GetOldPos().y + this->params.FarClip * 0.1f);
	this->Shader_Skybox->SetFloat("CamY", this->camera->GetOldPos().y);

	//MeshStrip* strip = this->skybox->GetStrips()->get(0);
	MeshStrip* strip = this->skybox->GetStrip();

	Object3D* obj = strip->GetRenderObject();
	this->Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());

	obj->GetVertBuff()->Apply();
	obj->GetIndsBuff()->Apply();
	if(obj->GetTextureResource() != NULL)
	{
		this->Shader_Skybox->SetResource("SkyMap", obj->GetTextureResource()->GetSRVPointer());//**TILLMAN
	}
	
	this->Shader_Skybox->Apply(0);

	this->Dx_DeviceContext->DrawIndexed(obj->GetIndsBuff()->GetElementCount(), 0, 0);

	// Unbind resources:
	this->Shader_Skybox->SetResource("SkyMap", NULL);
	this->Shader_Skybox->Apply(0);
}

void DxManager::RenderDeferredPerPixel()
{
	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	static float ClearColor[4] = {0.5f, 0.71f, 1.0f, 1.0f};
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_RenderTargetView, ClearColor);

	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//Unbind the vertex buffer since no vertex data is used.
	ID3D11Buffer* dummyBuffers [] = {NULL, NULL};
	UINT dummyStrides [] = {0, 0};
	UINT dummyOffsets [] = {0, 0};
	this->Dx_DeviceContext->IASetVertexBuffers(0, 2, dummyBuffers, dummyStrides, dummyOffsets);



	//Always tell the shader whether to use shadows or not.
	this->Shader_DeferredLightning->SetBool("useShadow", this->useShadow);
	if ( useShadow )
	{
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
	
		for (int l = 0; l < this->csm->GetNrOfCascadeLevels(); l++)
		{

			D3DXMATRIX lvp = this->csm->GetViewProjMatrix(l);
			this->Shader_DeferredLightning->SetResourceAtIndex(l, "CascadedShadowMap", this->csm->GetShadowMapSRV(l));
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsMatrix(l, "cascades", "viewProj", lvp);

		}

	}




	
	// Set sun-settings
	if(this->useSun) 
	{
		this->Shader_DeferredLightning->SetStructMemberAsFloat4("sun", "Direction", D3DXVECTOR4(this->sun.direction, 0.0f));
		this->Shader_DeferredLightning->SetStructMemberAsFloat4("sun", "LightColor", D3DXVECTOR4(this->sun.lightColor, 0.0f));
		this->Shader_DeferredLightning->SetStructMemberAsFloat("sun", "LightIntensity", this->sun.intensity);
	}
	//Always tell the shader whether to use sun or not.
	this->Shader_DeferredLightning->SetBool("UseSun", this->useSun);

	//DeferredLightning.fx:
	this->Shader_DeferredLightning->SetResource("Texture", this->Dx_GbufferSRVs[0]);
	this->Shader_DeferredLightning->SetResource("NormalAndDepth", this->Dx_GbufferSRVs[1]);
	this->Shader_DeferredLightning->SetResource("Position", this->Dx_GbufferSRVs[2]);
	this->Shader_DeferredLightning->SetResource("Specular", this->Dx_GbufferSRVs[3]);
	D3DXMATRIX vp = this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix();
	this->Shader_DeferredLightning->SetMatrix("CameraVP", vp);
	this->Shader_DeferredLightning->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	//stdafx.fx:
	this->Shader_DeferredLightning->SetFloat("NrOfLights", (float)this->lights.size()); //**tillman - omstrukturering**
	this->Shader_DeferredLightning->SetFloat4("SceneAmbientLight", D3DXVECTOR4(this->sceneAmbientLight, 1.0f));

	this->Shader_DeferredLightning->SetFloat("timerMillis", this->Timer);
	this->Shader_DeferredLightning->SetInt("windowWidth", this->params.WindowWidth);
	this->Shader_DeferredLightning->SetInt("windowHeight", this->params.WindowHeight);
		



	//ssao.fx:
	// this->ssao->PreRender(this->Shader_DeferredLightning, this->params, this->camera);

	this->Shader_DeferredLightning->Apply(0);
	
	this->Dx_DeviceContext->Draw(1, 0);

	
	// Unbind resources:
	this->Shader_DeferredLightning->SetResource("Texture", NULL);
	this->Shader_DeferredLightning->SetResource("NormalAndDepth", NULL);
	this->Shader_DeferredLightning->SetResource("Position", NULL);
	this->Shader_DeferredLightning->SetResource("Specular", NULL);
	this->Shader_DeferredLightning->SetResource("LavaTexture", NULL);
	for(int i = 0; i < this->lights.size(); i++)
	{
		this->Shader_DeferredLightning->SetResourceAtIndex(i, "ShadowMap", NULL);
	}
	for(int i = 0; i < this->csm->GetNrOfCascadeLevels(); i++)
		this->Shader_DeferredLightning->SetResourceAtIndex(i, "CascadedShadowMap", NULL);

	// Unbind SSAO
	this->ssao->PostRender(this->Shader_DeferredLightning);

	this->Shader_DeferredLightning->Apply(0);
}

void DxManager::RenderInvisibilityEffect() 
{
	HRESULT hr = S_OK;

	//get the surface/texture from the swap chain
	ID3D11Texture2D* backBufferTex = NULL;
	this->Dx_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTex);
	if(FAILED(hr))
	{
		MaloW::Debug("InvisibilityEffect: Failed to get buffer from swap chain");
		return;
	}

	//get texture description
	D3D11_TEXTURE2D_DESC texDesc;
	backBufferTex->GetDesc(&texDesc);
	
	//change bindflag for use as a shader resource
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 

	//create texture
	ID3D11Texture2D* sceneTex = NULL;
	hr = this->Dx_Device->CreateTexture2D(&texDesc, NULL, &sceneTex);	
	if(FAILED(hr))
	{
		MaloW::Debug("InvisibilityEffect: Failed to create texture");
		return;
	}
	
	//get resource from the render target view of the backbuffer
	ID3D11RenderTargetView* backBufferRTV = NULL;
	this->Dx_DeviceContext->OMGetRenderTargets(1, &backBufferRTV, NULL);
	ID3D11Resource* backBufferRTVResource = NULL;
	backBufferRTV->GetResource(&backBufferRTVResource);

	//copy data from the resource to the scene texture
	this->Dx_DeviceContext->CopyResource(sceneTex, backBufferRTVResource);

	//create shader resource view
	ID3D11ShaderResourceView* sceneSRV;
	hr = this->Dx_Device->CreateShaderResourceView(sceneTex, NULL, &sceneSRV);
	if(FAILED(hr))
	{
		MaloW::Debug("InvisibilityEffect: Failed to create shader resource view");
		return;
	}

	//set rendertarget & view port
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	//set shader variables
	this->Shader_InvisibilityEffect->SetResource("sceneTex", sceneSRV);

	//Per frame
	this->Shader_InvisibilityEffect->SetInt("width", this->params.WindowWidth); 
	this->Shader_InvisibilityEffect->SetInt("height", this->params.WindowHeight); 
	this->Shader_InvisibilityEffect->SetInt("blurSize", 5);

	//Invisible(effect) geometry
	D3DXMATRIX wvp;
	for(int i = 0; i < this->objects.size(); i++)
	{
		if(this->objects[i]->IsUsingInvisibility() && !this->objects[i]->GetDontRenderFlag())
		{
			MaloW::Array<MeshStrip*>* strips = this->objects[i]->GetStrips();
		
			//Per object/mesh
			wvp = this->objects[i]->GetWorldMatrix() * this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix();
			this->Shader_InvisibilityEffect->SetMatrix("WVP", wvp);

			//Per strip
			for(int u = 0; u < strips->size(); u++)
			{
				Object3D* obj = strips->get(u)->GetRenderObject();

				//Set texture
				if(obj->GetTextureResource() != NULL)
				{
					if(ID3D11ShaderResourceView* texture = obj->GetTextureResource()->GetSRVPointer())
					{
						this->Shader_InvisibilityEffect->SetResource("ballTex", texture);
						this->Shader_InvisibilityEffect->SetBool("textured", true);
					}
					else
					{
						this->Shader_InvisibilityEffect->SetResource("ballTex", NULL);
						this->Shader_InvisibilityEffect->SetBool("textured", false);
					}
				}
				else
				{
					this->Shader_InvisibilityEffect->SetResource("ballTex", NULL);
					this->Shader_InvisibilityEffect->SetBool("textured", false);
				}

				//Set topology
				this->Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());

				//Apply vertex buffer
				Buffer* verts = obj->GetVertBuff();
				if(verts)
					verts->Apply();

				//Apply index buffer
				Buffer* inds = obj->GetIndsBuff();
				if(inds)
					inds->Apply();
			
				//Apply shader
				this->Shader_InvisibilityEffect->Apply(0);

				//Draw
				if(inds)
					this->Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
				else
					this->Dx_DeviceContext->Draw(verts->GetElementCount(), 0);

				this->Shader_InvisibilityEffect->SetResource((char*)strips->get(u)->GetTexturePath().c_str(), NULL);
			}
		}
	}

	SAFE_RELEASE(backBufferTex);
	SAFE_RELEASE(sceneTex);
	SAFE_RELEASE(backBufferRTV);
	SAFE_RELEASE(backBufferRTVResource);
	SAFE_RELEASE(sceneSRV);

	this->Shader_InvisibilityEffect->SetResource("sceneTex", NULL);
}

void DxManager::RenderQuadDeferred()
{
	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_DeferredQuadRT, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	static float ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_DeferredQuadRT, ClearColor);

	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	this->Shader_DeferredQuad->SetResource("Texture", this->Dx_GbufferSRVs[0]);
	this->Shader_DeferredQuad->SetResource("NormalAndDepth", this->Dx_GbufferSRVs[1]);
	this->Shader_DeferredQuad->SetResource("Position", this->Dx_GbufferSRVs[2]);
	this->Shader_DeferredQuad->SetResource("Specular", this->Dx_GbufferSRVs[3]);
	D3DXMATRIX v = this->camera->GetViewMatrix();
	D3DXMATRIX p = this->camera->GetProjectionMatrix();
	D3DXMATRIX vp = v * p;
	this->Shader_DeferredQuad->SetMatrix("CameraVP", vp);
	this->Shader_DeferredQuad->SetMatrix("CameraView", v);
	this->Shader_DeferredQuad->SetMatrix("CameraProj", p);
	this->Shader_DeferredQuad->SetFloat("CameraFar", this->params.FarClip);
	this->Shader_DeferredQuad->SetFloat("CameraNear", this->params.NearClip);
	this->Shader_DeferredQuad->SetFloat("ScreenWidth", (float)this->params.WindowWidth);
	this->Shader_DeferredQuad->SetFloat("ScreenHeight", (float)this->params.WindowHeight);
	this->Shader_DeferredQuad->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	
	this->Shader_DeferredQuad->Apply(0);
	
	
	this->Dx_DeviceContext->Draw(this->lights.size(), 0);
	
	
	// Unbind resources:
	this->Shader_DeferredQuad->SetResource("Texture", NULL);
	this->Shader_DeferredQuad->SetResource("NormalAndDepth", NULL);
	this->Shader_DeferredQuad->SetResource("Position", NULL);
	this->Shader_DeferredQuad->SetResource("Specular", NULL);
	for(int i = 0; i < this->lights.size(); i++)
	{
		this->Shader_DeferredQuad->SetResourceAtIndex(i, "ShadowMap", NULL);
	}
	this->Shader_DeferredQuad->Apply(0);
}

void DxManager::RenderDeferredTexture()
{
	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	static float ClearColor[4] = {0.5f, 0.71f, 1.0f, 1};
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_RenderTargetView, ClearColor);

	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	this->Shader_DeferredTexture->SetResource("Texture", this->Dx_GbufferSRVs[0]);
	this->Shader_DeferredTexture->SetResource("NormalAndDepth", this->Dx_GbufferSRVs[1]);
	this->Shader_DeferredTexture->SetResource("Position", this->Dx_GbufferSRVs[2]);
	this->Shader_DeferredTexture->SetResource("Specular", this->Dx_GbufferSRVs[3]);
	this->Shader_DeferredTexture->SetResource("LightAccu", this->Dx_DeferredSRV);

	// Set SSAO settings
	//this->ssao->PreRender(this->Shader_DeferredTexture, this->params, this->camera);
	this->Shader_DeferredTexture->Apply(0);

	
	this->Dx_DeviceContext->Draw(1, 0);
	
	
	// Unbind resources:
	this->Shader_DeferredTexture->SetResource("Texture", NULL);
	this->Shader_DeferredTexture->SetResource("NormalAndDepth", NULL);
	this->Shader_DeferredTexture->SetResource("Position", NULL);
	this->Shader_DeferredTexture->SetResource("Specular", NULL);
	this->Shader_DeferredTexture->SetResource("LightAccu", NULL);

	// Unbind SSAO
	//this->ssao->PostRender(this->Shader_DeferredTexture);

	this->Shader_DeferredTexture->Apply(0);
}


void DxManager::RenderDeferredGeoTranslucent()
{
	this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	float ClearColor1[4] = {0.5f, 0.71f, 1.0f, 1};
	float ClearColor2[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[0], ClearColor1);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[1], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[2], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[3], ClearColor2);

	//Static meshes
	this->Shader_DeferredGeoTranslucent->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	this->Shader_DeferredGeoTranslucent->SetFloat("NearClip", this->params.NearClip);
	this->Shader_DeferredGeoTranslucent->SetFloat("FarClip", this->params.FarClip);
	this->Shader_DeferredGeoTranslucent->SetFloat("timerMillis", this->Timer);
	D3DXMATRIX world;
	D3DXMATRIX view = this->camera->GetViewMatrix();
	D3DXMATRIX proj = this->camera->GetProjectionMatrix();
	D3DXMATRIX wvp;
	D3DXMATRIX worldInverseTranspose;

	for(int i = 0; i < this->waterplanes.size(); i++)
	{
		WaterPlane* wp = this->waterplanes[i];
		if(wp->VertexDataHasChanged)
		{
			delete wp->GetVertexBuffer();

			// Rebuild vertex buffer
			//Create vertex buffer
			BUFFER_INIT_DESC vertexBufferDesc;
			vertexBufferDesc.ElementSize = sizeof(Vertex);
			vertexBufferDesc.InitData = wp->GetVerts(); 
			vertexBufferDesc.NumElements = wp->GetNrOfVerts();
			vertexBufferDesc.Type = VERTEX_BUFFER;
			vertexBufferDesc.Usage = BUFFER_DEFAULT;

			Buffer* vertexBuffer = new Buffer();
			HRESULT hr = vertexBuffer->Init(this->Dx_Device, this->Dx_DeviceContext, vertexBufferDesc);
			if(FAILED(hr))
			{
				delete vertexBuffer; 
				vertexBuffer = NULL;

				MaloW::Debug("ERROR: Could not create vertex buffer. REASON: RecreateWaterPlane."
					+ string("ERROR code: '") 
					+ MaloW::GetHRESULTErrorCodeString(hr));
			}
			wp->SetVertexBuffer(vertexBuffer);
			wp->VertexDataHasChanged = false;
		}

		if(!wp->GetDontRenderFlag())
		{
			// Per object
			this->Shader_DeferredGeoTranslucent->SetInt("specialColor", wp->GetSpecialColor());

			// Set matrices
			world = wp->GetWorldMatrix();
			wvp = world * view * proj;
			D3DXMatrixInverse(&worldInverseTranspose, NULL, &world);
			D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);

			this->Shader_DeferredGeoTranslucent->SetMatrix("WVP", wvp);
			this->Shader_DeferredGeoTranslucent->SetMatrix("worldMatrix", world);
			this->Shader_DeferredGeoTranslucent->SetMatrix("worldMatrixInverseTranspose", worldInverseTranspose);

			this->Dx_DeviceContext->IASetPrimitiveTopology(wp->GetTopology());

			// Setting lightning from material
			this->Shader_DeferredGeoTranslucent->SetFloat4("SpecularColor", D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f));
			this->Shader_DeferredGeoTranslucent->SetFloat("SpecularPower", 10.0f);
			this->Shader_DeferredGeoTranslucent->SetFloat4("AmbientLight", D3DXVECTOR4(this->sceneAmbientLight, 1.0f));
			this->Shader_DeferredGeoTranslucent->SetFloat4("DiffuseColor", D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f));

			Buffer* verts = wp->GetVertexBuffer();
			if(verts)
				verts->Apply();

			if(wp->GetTextureResource() != NULL && wp->GetTextureResource2() != NULL)
			{
				if(ID3D11ShaderResourceView* texture = wp->GetTextureResource()->GetSRVPointer())
				{
					if(ID3D11ShaderResourceView* texture2 = wp->GetTextureResource2()->GetSRVPointer())//**TILLMAN
					{
						this->Shader_DeferredGeoTranslucent->SetBool("textured", true);
						this->Shader_DeferredGeoTranslucent->SetResource("tex2D", texture);
						this->Shader_DeferredGeoTranslucent->SetResource("tex2D2", texture2);
					}
					else
					{
						this->Shader_DeferredGeoTranslucent->SetBool("textured", false);
						this->Shader_DeferredGeoTranslucent->SetResource("tex2D", NULL);
						this->Shader_DeferredGeoTranslucent->SetResource("tex2D2", NULL);
					}
				}
				else
				{
					this->Shader_DeferredGeoTranslucent->SetBool("textured", false);
					this->Shader_DeferredGeoTranslucent->SetResource("tex2D", NULL);
					this->Shader_DeferredGeoTranslucent->SetResource("tex2D2", NULL);
				}
			}
			else
			{
				this->Shader_DeferredGeoTranslucent->SetBool("textured", false);
				this->Shader_DeferredGeoTranslucent->SetResource("tex2D", NULL);
				this->Shader_DeferredGeoTranslucent->SetResource("tex2D2", NULL);
			}
			

			this->Shader_DeferredGeoTranslucent->Apply(0);
			this->Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
		}
	}
	// Unbind resources static geometry:
	this->Shader_DeferredGeoTranslucent->SetResource("tex2D", NULL);
	this->Shader_DeferredGeoTranslucent->SetResource("tex2D2", NULL);
	this->Shader_DeferredGeoTranslucent->Apply(0);
}

void DxManager::RenderDeferredPerPixelTranslucent()
{
	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	this->Dx_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);



	//Always tell the shader whether to use shadows or not.
	this->Shader_DeferredPerPixelTranslucent->SetBool("useShadow", this->useShadow);
	if ( useShadow )
	{
		float PCF_SIZE = (float)this->params.ShadowMapSettings + 1;
		float PCF_SQUARED = 1 / (PCF_SIZE * PCF_SIZE);

		this->Shader_DeferredPerPixelTranslucent->SetFloat("SMAP_DX", 1.0f / (256.0f * pow(2.0f, this->params.ShadowMapSettings / 2.0f)));

		this->Shader_DeferredPerPixelTranslucent->SetFloat("PCF_SIZE", PCF_SIZE);
		this->Shader_DeferredPerPixelTranslucent->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);

		this->Shader_DeferredPerPixelTranslucent->SetBool("blendCascades", true); //** TILLMAN CSM VARIABLE**
		this->Shader_DeferredPerPixelTranslucent->SetFloat("blendDistance", this->csm->GetBlendDistance()); 
		this->Shader_DeferredPerPixelTranslucent->SetFloat("blendStrength", 0.0f); //** TILLMAN CSM VARIABLE**

		this->Shader_DeferredPerPixelTranslucent->SetInt("nrOfCascades", this->csm->GetNrOfCascadeLevels());
		D3DXVECTOR4 cascadeFarPlanes = D3DXVECTOR4(-1.0f, -1.0f, -1.0f, -1.0f);
		for(int i = 0; i < this->csm->GetNrOfCascadeLevels(); i++)
		{
			cascadeFarPlanes[i] = this->csm->GetSplitDepth(i + 1);
		}
		this->Shader_DeferredPerPixelTranslucent->SetFloat4("cascadeFarPlanes", cascadeFarPlanes);
		this->Shader_DeferredPerPixelTranslucent->SetMatrix("cameraViewMatrix", this->camera->GetViewMatrix()); //Send camera view matrix to determine what frustum slice to use.
	
		for (int l = 0; l < this->csm->GetNrOfCascadeLevels(); l++)
		{
			D3DXMATRIX lvp = this->csm->GetViewProjMatrix(l);
			this->Shader_DeferredPerPixelTranslucent->SetResourceAtIndex(l, "CascadedShadowMap", this->csm->GetShadowMapSRV(l));
			this->Shader_DeferredPerPixelTranslucent->SetStructMemberAtIndexAsMatrix(l, "cascades", "viewProj", lvp);
		}

	}




	
	// Set sun-settings
	if(this->useSun) 
	{
		this->Shader_DeferredPerPixelTranslucent->SetStructMemberAsFloat4("sun", "Direction", D3DXVECTOR4(this->sun.direction, 0.0f));
		this->Shader_DeferredPerPixelTranslucent->SetStructMemberAsFloat4("sun", "LightColor", D3DXVECTOR4(this->sun.lightColor, 0.0f));
		this->Shader_DeferredPerPixelTranslucent->SetStructMemberAsFloat("sun", "LightIntensity", this->sun.intensity);
	}
	//Always tell the shader whether to use sun or not.
	this->Shader_DeferredPerPixelTranslucent->SetBool("UseSun", this->useSun);

	//DeferredLightning.fx:
	this->Shader_DeferredPerPixelTranslucent->SetResource("Texture", this->Dx_GbufferSRVs[0]);
	this->Shader_DeferredPerPixelTranslucent->SetResource("NormalAndDepth", this->Dx_GbufferSRVs[1]);
	this->Shader_DeferredPerPixelTranslucent->SetResource("Position", this->Dx_GbufferSRVs[2]);
	this->Shader_DeferredPerPixelTranslucent->SetResource("Specular", this->Dx_GbufferSRVs[3]);
	D3DXMATRIX vp = this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix();
	this->Shader_DeferredPerPixelTranslucent->SetMatrix("CameraVP", vp);
	this->Shader_DeferredPerPixelTranslucent->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	//stdafx.fx:
	this->Shader_DeferredPerPixelTranslucent->SetFloat("NrOfLights", (float)this->lights.size()); //**tillman - omstrukturering**
	this->Shader_DeferredPerPixelTranslucent->SetFloat4("SceneAmbientLight", D3DXVECTOR4(this->sceneAmbientLight, 1.0f));

	this->Shader_DeferredPerPixelTranslucent->SetFloat("timerMillis", this->Timer);
	this->Shader_DeferredPerPixelTranslucent->SetInt("windowWidth", this->params.WindowWidth);
	this->Shader_DeferredPerPixelTranslucent->SetInt("windowHeight", this->params.WindowHeight);
		



	//ssao.fx:
	// this->ssao->PreRender(this->Shader_DeferredPerPixelTranslucent, this->params, this->camera);

	this->Shader_DeferredPerPixelTranslucent->Apply(0);
	
	this->Dx_DeviceContext->Draw(1, 0);

	
	// Unbind resources:
	this->Shader_DeferredPerPixelTranslucent->SetResource("Texture", NULL);
	this->Shader_DeferredPerPixelTranslucent->SetResource("NormalAndDepth", NULL);
	this->Shader_DeferredPerPixelTranslucent->SetResource("Position", NULL);
	this->Shader_DeferredPerPixelTranslucent->SetResource("Specular", NULL);
	this->Shader_DeferredPerPixelTranslucent->SetResource("LavaTexture", NULL);
	for(int i = 0; i < this->lights.size(); i++)
	{
		this->Shader_DeferredPerPixelTranslucent->SetResourceAtIndex(i, "ShadowMap", NULL);
	}
	for(int i = 0; i < this->csm->GetNrOfCascadeLevels(); i++)
		this->Shader_DeferredPerPixelTranslucent->SetResourceAtIndex(i, "CascadedShadowMap", NULL);

	// Unbind SSAO
	this->ssao->PostRender(this->Shader_DeferredPerPixelTranslucent);

	this->Shader_DeferredPerPixelTranslucent->Apply(0);
}
