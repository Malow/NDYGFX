#include "DxManager.h"

void DxManager::PreRender()
{
	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	//Clear render targets
	float ClearColor1[4] = {0.5f, 0.71f, 1.0f, 1};
	float ClearColor2[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[0], ClearColor1);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[1], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[2], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[3], ClearColor2);


	if(this->useSun && this->useShadow)	
	{
		this->csm->PreRender(this->sun.direction, this->camera, this->params.ShadowFit);
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


void DxManager::RenderDeferredGeometry()
{
	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
	//Clear render targets
	float ClearColor1[4] = {0.5f, 0.71f, 1.0f, 1};
	float ClearColor2[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[0], ClearColor1);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[1], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[2], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[3], ClearColor2);

	int CurrentRenderedMeshes = 0;
	int CurrentRenderedTerrains = 0;
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
			CurrentRenderedTerrains++;
			//Set topology
			this->Dx_DeviceContext->IASetPrimitiveTopology(terrPtr->GetTopology());

			//Calculate matrices & set them
			world = terrPtr->GetWorldMatrix();
			wvp = world * view * proj;
			D3DXMatrixInverse(&worldInverseTranspose, NULL, &world); //worldInverseTranspose needs to be an identity matrix.
			D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose); //Used for calculating right normal
			this->Shader_TerrainEditor->SetMatrix("WVP", wvp);
			this->Shader_TerrainEditor->SetMatrix("worldMatrix", world);
			this->Shader_TerrainEditor->SetMatrix("worldMatrixInverseTranspose", worldInverseTranspose);

			//Update vertex buffer if y-value for vertices (height map) have changed
			if(terrPtr->HasHeightMapChanged() || terrPtr->HaveNormalsChanged()) //**TILLMAN LOADTHREAD**
			{
				//**OPT(OBS! Ev. only for editor): should be replaced with an update function ** TILLMAN
					//this->Dx_DeviceContext->UpdateSubresource()
				/*int srcRP = sizeof(float) * terrPtr->GetSize();
				int srcDP = srcRP * terrPtr->GetSize(); 
				this->Dx_DeviceContext->UpdateSubresource(
					terrPtr->GetVertexBufferPointer()->GetBufferPointer(),
					D3D10CalcSubresource(0, 0, 1),
					NULL,
					terrPtr->GetVerticesPointer(),
					srcRP,
					srcDP);
				*/
				//Save pointer to buffer
				Buffer* oldBuffer = terrPtr->GetVertexBufferPointer();

				//Create new vertex buffer
				BUFFER_INIT_DESC vertexBufferDesc;
				vertexBufferDesc.ElementSize = sizeof(Vertex);
				vertexBufferDesc.InitData = terrPtr->GetVerticesPointer(); 
				vertexBufferDesc.NumElements = terrPtr->GetNrOfVertices();
				vertexBufferDesc.Type = VERTEX_BUFFER;
				vertexBufferDesc.Usage = BUFFER_DEFAULT;

				Buffer* newBuffer = new Buffer();
				if(FAILED(newBuffer->Init(this->Dx_Device, this->Dx_DeviceContext, vertexBufferDesc)))
					MaloW::Debug("ERROR: Could not create new vertex buffer for terrain.");
				//Set it
				terrPtr->SetVertexBuffer(newBuffer);
				//Delete old buffer
				if(oldBuffer) delete oldBuffer;

				//Set that the height map shall not be changed anymore.
				terrPtr->HeightMapHasChanged(false);
				//Same for normals
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
					terrPtr->SetTexture(j, GetResourceManager()->CreateTextureResourceFromFile(terrPtr->GetTextureResourceToLoadFileName(j).c_str()));
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
					if(bmPtr0->HasChanged)
					{
						//Release old shader resource view //**TILLMAN LOAD-THREAD**
						if(bmPtr0->SRV) bmPtr0->SRV->Release();

						//Create texture
						int widthOrHeight = bmPtr0->Size;
						D3D11_TEXTURE2D_DESC texDesc;
						texDesc.Width = widthOrHeight;
						texDesc.Height = widthOrHeight;
						texDesc.MipLevels = 1;
						texDesc.ArraySize = 1;
						texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
						texDesc.SampleDesc.Count = 1;
						texDesc.SampleDesc.Quality = 0;
						texDesc.Usage = D3D11_USAGE_IMMUTABLE;
						texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
						texDesc.CPUAccessFlags = 0;
						texDesc.MiscFlags = 0;

						D3D11_SUBRESOURCE_DATA initData = {0};
						initData.SysMemPitch = widthOrHeight * sizeof(float) * 4;
						initData.pSysMem = bmPtr0->Data; 

						ID3D11Texture2D* tex = NULL;
						if(FAILED(this->Dx_Device->CreateTexture2D(&texDesc, &initData, &tex)))
						{
							MaloW::Debug("ERROR: Failed to create texture with blend map data.");
						}
						//Create shader resource view
						if(FAILED(this->Dx_Device->CreateShaderResourceView(tex, 0, &bmPtr0->SRV)))
						{
							MaloW::Debug("ERROR: Failed to create Shader resource view with blend map texture.");
						}
						//Set that the blend map shall not be changed anymore.
						bmPtr0->HasChanged = false;
					}

					BlendMap* bmPtr1 = terrPtr->GetBlendMapPointer(1);  //**tillman ändra**
					if(bmPtr1 != NULL)
					{
						if(bmPtr1->HasChanged)
						{
							//Release old shader resource view //**TILLMAN LOAD-THREAD**
							if(bmPtr1->SRV) bmPtr1->SRV->Release();

							//Create texture
							int widthOrHeight = bmPtr1->Size;
							D3D11_TEXTURE2D_DESC texDesc;
							texDesc.Width = widthOrHeight;
							texDesc.Height = widthOrHeight;
							texDesc.MipLevels = 1;
							texDesc.ArraySize = 1;
							texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
							texDesc.SampleDesc.Count = 1;
							texDesc.SampleDesc.Quality = 0;
							texDesc.Usage = D3D11_USAGE_IMMUTABLE;
							texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
							texDesc.CPUAccessFlags = 0;
							texDesc.MiscFlags = 0;

							D3D11_SUBRESOURCE_DATA initData = {0};
							initData.SysMemPitch = widthOrHeight * sizeof(float) * 4;
							initData.pSysMem = bmPtr1->Data; 

							ID3D11Texture2D* tex = NULL;
							if(FAILED(this->Dx_Device->CreateTexture2D(&texDesc, &initData, &tex)))
							{
								MaloW::Debug("ERROR: Failed to create texture with blend map data.");
							}
							//Create shader resource view
							if(FAILED(this->Dx_Device->CreateShaderResourceView(tex, 0, &bmPtr1->SRV)))
							{
								MaloW::Debug("ERROR: Failed to create Shader resource view with blend map texture.");
							}
							//Set that the blend map shall not be changed anymore.
							bmPtr1->HasChanged = false;
						}

						//Set blend map variables
						this->Shader_TerrainEditor->SetResource("blendMap1", bmPtr1->SRV);
					}

					//Set blend map variables
					this->Shader_TerrainEditor->SetBool("blendMapped", true);
					this->Shader_TerrainEditor->SetInt("nrOfBlendMaps", terrPtr->GetNrOfBlendMaps());
					this->Shader_TerrainEditor->SetResource("blendMap0", bmPtr0->SRV);
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
			this->Shader_TerrainEditor->SetFloat("specularPower", terrPtr->GetMaterial()->SpecularPower);
			this->Shader_TerrainEditor->SetFloat3("specularColor", terrPtr->GetMaterial()->SpecularColor);
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
			}
			else
			{
				this->Dx_DeviceContext->Draw(vertices->GetElementCount(), 0);
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



	//Static meshes
	this->Shader_DeferredGeometry->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	this->Shader_DeferredGeometry->SetFloat("NearClip", this->params.NearClip);
	this->Shader_DeferredGeometry->SetFloat("FarClip", this->params.FarClip);

	for(int i = 0; i < this->objects.size(); i++)
	{
		StaticMesh* staticMesh = this->objects[i];
		if(!staticMesh->IsUsingInvisibility())
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
					if(!strips->get(u)->GetCulled())
					{
						if(!hasBeenCounted)
						{
							CurrentRenderedMeshes++;
							hasBeenCounted = true;
						}

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
							}
							else //else set texture variables to not be used
							{
								this->Shader_DeferredGeometry->SetBool("textured", false);
								this->Shader_DeferredGeometry->SetResource("tex2D", NULL);
							}
						}
						else //else set texture variables to not be used
						{
							this->Shader_DeferredGeometry->SetBool("textured", false);
							this->Shader_DeferredGeometry->SetResource("tex2D", NULL);
						}
						Buffer* inds = obj->GetIndsBuff();
						if(inds)
							inds->Apply();
			
						this->Shader_DeferredGeometry->Apply(0);

						// draw
						if(inds)
							this->Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
						else
							this->Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
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
				MaloW::Array<MeshStrip*>* strips = this->objects[i]->GetStrips();
				//Just check the first strip if it is culled. 
				if(!strips->get(0)->GetCulled())
				{
					if(!hasBeenCounted)
					{
						CurrentRenderedMeshes++;
						hasBeenCounted = true;
					}

					//Render billboards
					this->RenderBillboard(this->objects[i]->GetBillboardGFX());
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
	// Unbind resources static geometry:
	this->Shader_DeferredGeometry->SetResource("tex2D", NULL);
	this->Shader_DeferredGeometry->Apply(0);



	// Normal Animated meshes
	this->Shader_DeferredAnimatedGeometry->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetPositionD3DX(), 1));
	this->Shader_DeferredAnimatedGeometry->SetFloat("NearClip", this->params.NearClip);
	this->Shader_DeferredAnimatedGeometry->SetFloat("FarClip", this->params.FarClip);
	for(int i = 0; i < this->animations.size(); i++)
	{
		AnimatedMesh* animatedMesh = this->animations[i];
		if(!animatedMesh->IsUsingInvisibility())
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

				if(!animatedMesh->GetKeyFrames()->get(0)->strips->get(0)->GetCulled())
				{
					CurrentRenderedMeshes++;			

					KeyFrame* one = NULL;
					KeyFrame* two = NULL;
					float t = 0.0f;
					animatedMesh->SetCurrentTime(this->Timer * 1000.0f); //Timer is in seconds.
					animatedMesh->GetCurrentKeyFrames(&one, &two, t);

					MaloW::Array<MeshStrip*>* stripsOne = one->strips;
					MaloW::Array<MeshStrip*>* stripsTwo = two->strips;
		
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
						this->Shader_DeferredAnimatedGeometry->SetFloat4("SpecularColor", D3DXVECTOR4(stripsOne->get(u)->GetMaterial()->SpecularColor, 1));
						this->Shader_DeferredAnimatedGeometry->SetFloat("SpecularPower", stripsOne->get(u)->GetMaterial()->SpecularPower);
						this->Shader_DeferredAnimatedGeometry->SetFloat4("AmbientLight", D3DXVECTOR4(stripsOne->get(u)->GetMaterial()->AmbientColor, 1));
						this->Shader_DeferredAnimatedGeometry->SetFloat4("DiffuseColor", D3DXVECTOR4(stripsOne->get(u)->GetMaterial()->DiffuseColor, 1));

						Buffer* vertsOne = objOne->GetVertBuff();
						Buffer* vertsTwo = objTwo->GetVertBuff();

						ID3D11Buffer* vertexBuffers [] = {vertsOne->GetBufferPointer(), vertsTwo->GetBufferPointer()};
						UINT strides [] = {sizeof(Vertex), sizeof(Vertex)};
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
							}
							else //else set texture variables to not be used
							{
								this->Shader_DeferredAnimatedGeometry->SetBool("textured", false);
								this->Shader_DeferredAnimatedGeometry->SetResource("tex2D", NULL);
							}
						}
						else //else set texture variables to not be used
						{
							this->Shader_DeferredAnimatedGeometry->SetBool("textured", false);
							this->Shader_DeferredAnimatedGeometry->SetResource("tex2D", NULL);
						}

						this->Shader_DeferredAnimatedGeometry->Apply(0);

						// draw
						this->Dx_DeviceContext->Draw(vertsOne->GetElementCount(), 0);
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
				//Just check the first strip if it is culled. 
				if(!animatedMesh->GetKeyFrames()->get(0)->strips->get(0)->GetCulled())
				{
					if(!hasBeenCounted)
					{
						CurrentRenderedMeshes++;
						hasBeenCounted = true;
					}

					//Render billboards
					this->RenderBillboard(animatedMesh->GetBillboardGFX());
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
}


void DxManager::RenderDeferredSkybox()
{
	if(!this->skybox)
		return;

		// Set matrixes
	D3DXMATRIX world, wvp, view, proj;
	view = this->camera->GetViewMatrix();
	proj = this->camera->GetProjectionMatrix();
	world = this->skybox->GetSkyboxWorldMatrix(this->camera->GetPositionD3DX());
	wvp = world * view * proj;
	
	this->Shader_Skybox->SetMatrix("gWVP", wvp);
	this->Shader_Skybox->SetMatrix("world", world);
	this->Shader_Skybox->SetFloat("FogHeight", this->params.FarClip * 2.0f);
	this->Shader_Skybox->SetFloat("CamY", this->camera->GetPosition().y + this->params.FarClip * 0.5f);

	MeshStrip* strip = this->skybox->GetStrips()->get(0);

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
	this->ssao->PreRender(this->Shader_DeferredLightning, this->params, this->camera);

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
		if(this->objects[i]->IsUsingInvisibility())
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
				if(ID3D11ShaderResourceView* texture = obj->GetTextureResource()->GetSRVPointer())//**TILLMAN
				{
					this->Shader_InvisibilityEffect->SetResource((char*)strips->get(u)->GetTexturePath().c_str(), texture);
					this->Shader_InvisibilityEffect->SetBool("textured", true);
				}
				else
				{
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