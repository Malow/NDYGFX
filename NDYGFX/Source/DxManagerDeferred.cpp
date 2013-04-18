#include "DxManager.h"

void DxManager::PreRender()
{
	this->CurrentRenderedMeshes = 0;
	this->CurrentRenderedTerrains = 0;
	this->CurrentRenderedFBX = 0;
	this->CurrentRenderedNrOfVertices = 0;
	this->CurrentNrOfDrawCalls = 0;

	//clear and set render target/depth
	//this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->ClearDepthStencilView(this->Dx_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);
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

	// Optimizing: send terrain textures once to the GPU.
	/*static unsigned int nrOfTexturesSet = 0;
	static bool texSet[7];
	static const TextureResource* texRes = NULL;
	if(nrOfTexturesSet < 7)
	{
		//0
		if(!texSet[0])
		{
			texRes = GetResourceManager()->GetTextureResourcePointer("Media/Textures/01_v02-Moss.png");
			if(texRes)
			{
				this->Shader_TerrainEditor->SetResource("tex0", texRes->GetSRVPointer());
				nrOfTexturesSet++;
				texSet[0] = true;
			}
		}
		//1
		if(!texSet[1])
		{
			texRes = GetResourceManager()->GetTextureResourcePointer("Media/Textures/02_v03-Leaf.png");
			if(texRes)
			{
				this->Shader_TerrainEditor->SetResource("tex1", texRes->GetSRVPointer());
				nrOfTexturesSet++;
				texSet[1] = true;
			}
		}
		//2
		if(!texSet[2])
		{
			texRes = GetResourceManager()->GetTextureResourcePointer("Media/Textures/03_v02-Mix.png");
			if(texRes)
			{
				this->Shader_TerrainEditor->SetResource("tex2", texRes->GetSRVPointer());
				nrOfTexturesSet++;
				texSet[2] = true;
			}
		}
		//3
		if(!texSet[3])
		{
			texRes = GetResourceManager()->GetTextureResourcePointer("Media/Textures/04_v02-Gravel.png");
			if(texRes)
			{
				this->Shader_TerrainEditor->SetResource("tex3", texRes->GetSRVPointer());
				nrOfTexturesSet++;
				texSet[3] = true;
			}
		}
		//4
		if(!texSet[4])
		{
			texRes = GetResourceManager()->GetTextureResourcePointer("Media/Textures/05_v01-Sandpng.png");
			if(texRes)
			{
				this->Shader_TerrainEditor->SetResource("tex4", texRes->GetSRVPointer());
				nrOfTexturesSet++;
				texSet[4] = true;
			}
		}
		//5
		if(!texSet[5])
		{
			texRes = GetResourceManager()->GetTextureResourcePointer("Media/Textures/06_v01-MossDark.png");
			if(texRes)
			{
				this->Shader_TerrainEditor->SetResource("tex5", texRes->GetSRVPointer());
				nrOfTexturesSet++;
				texSet[5] = true;
			}
		}
		//6
		if(!texSet[6])
		{
			texRes = GetResourceManager()->GetTextureResourcePointer("Media/Textures/07_v01-MossLight.png");
			if(texRes)
			{
				this->Shader_TerrainEditor->SetResource("tex6", texRes->GetSRVPointer());
				nrOfTexturesSet++;
				texSet[6] = true;
			}
		}
		//7 (unused)
		/*const TextureResource* texRes = GetResourceManager()->GetTextureResourcePointer("Media/Textures/01_v02-Moss.png");
		if(texRes)
		{
			this->Shader_TerrainEditor->SetResource("tex7", texRes->GetSRVPointer());
			nrOfTexturesSet++;
		}*/
	//}
}


void DxManager::RenderDeferredGeoTerrains()
{
	//clear and set render target/depth
	this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);
	this->Dx_DeviceContext->RSSetViewports(1, &this->Dx_Viewport);

	//Matrices
	D3DXMATRIX world, view, proj, wvp, worldInverseTranspose;
	D3DXMatrixIdentity(&worldInverseTranspose); //Needed to calculate correct world inverse matrix
	view = this->camera->GetViewMatrix();
	proj = this->camera->GetProjectionMatrix();


	//Terrain 
	this->Shader_TerrainEditor->SetFloat3("g_CamPos", this->camera->GetOldPos());
	this->Shader_TerrainEditor->SetFloat("g_FarClip", this->params.FarClip);

	//Per terrain:
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
			if(terrPtr->HasHeightMapChanged() && terrPtr->HaveNormalsChanged()) 
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
			if(terrPtr->GetTexture(0) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex0", terrPtr->GetTexture(0)->GetSRVPointer());
			}
			if(terrPtr->GetTexture(1) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex1", terrPtr->GetTexture(1)->GetSRVPointer());
			}
			if(terrPtr->GetTexture(2) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex2", terrPtr->GetTexture(2)->GetSRVPointer());
			}
			if(terrPtr->GetTexture(3) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex3", terrPtr->GetTexture(3)->GetSRVPointer());
			}
			if(terrPtr->GetTexture(4) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex4", terrPtr->GetTexture(4)->GetSRVPointer());
			}
			if(terrPtr->GetTexture(5) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex5", terrPtr->GetTexture(5)->GetSRVPointer());
			}
			if(terrPtr->GetTexture(6) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex6", terrPtr->GetTexture(6)->GetSRVPointer());
			}
			if(terrPtr->GetTexture(7) != NULL)
			{
				this->Shader_TerrainEditor->SetResource("tex7", terrPtr->GetTexture(7)->GetSRVPointer());
			}

			if(terrPtr->GetNrOfTextures() > 0) 
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

					BlendMap* bmPtr1 = terrPtr->GetBlendMapPointer(1); 
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
				//Release old shader resource view 
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
				this->Shader_TerrainEditor->SetInt("nodesPerSide", terrPtr->GetNrOfAINodesPerSide());
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
				this->CurrentNrOfDrawCalls++;
			}
			else if(vertices)
			{
				this->Dx_DeviceContext->Draw(vertices->GetElementCount(), 0);
				//Count(debug)
				this->CurrentNrOfDrawCalls++;
			}
			else
			{
				MaloW::Debug("WARNING: DxManagerDeferred: RenderDeferredGeometry(): Both index and vertex buffers were NULL for the terrain.");
			}
		}
	}
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

	for(unsigned int i = 0; i < this->decals.size(); i++)
	{
		// Test Texture
		if ( !this->decals[i]->GetStrip()->GetRenderObject()->GetTextureResource() )
			continue;

		// Shader Settings
		this->Shader_Decal->SetMatrix("World", this->decals[i]->GetWorldMatrix());
		this->Shader_Decal->SetMatrix("WorldViewProj", this->decals[i]->GetWorldMatrix() * viewProj);
		this->Shader_Decal->SetResource("Decal", this->decals[i]->GetStrip()->GetRenderObject()->GetTextureResource()->GetSRVPointer());
		this->Shader_Decal->SetMatrix("ScreenToLocal", this->decals[i]->GetMatrix());
		this->Shader_Decal->SetFloat("opacity", this->decals[i]->GetOpacity());
		this->Shader_Decal->SetFloat("size", this->decals[i]->GetSize());

		Buffer* verts = this->decals[i]->GetStrip()->GetRenderObject()->GetVertBuff();
		verts->Apply();
		Buffer* inds = this->decals[i]->GetStrip()->GetRenderObject()->GetIndsBuff();
		inds->Apply();
		this->Shader_Decal->Apply(0);

		this->Dx_DeviceContext->DrawIndexed(inds->GetElementCount(), 0, 0);
	}

	//this->Shader_Decal->SetResource("Depth", NULL);
	//this->Shader_Decal->SetResource("Decal", NULL);
	//this->Shader_Decal->Apply(0);
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
	for(int i = 0; i < this->objects.size(); i++)
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
				//As long as one strip has not been culled, add whole mesh for instancing
				MaloW::Array<MeshStrip*>* strips = staticMesh->GetStrips();
				unsigned int index = 0;
				bool oneStripIsNotCulled = false;
				while(!oneStripIsNotCulled && index < strips->size())
				{
					if(!staticMesh->IsStripCulled(index++))
					{
						oneStripIsNotCulled = true;
						CurrentRenderedMeshes++;

						//Add mesh info to instance helper
						this->instancingHelper->AddStaticMesh(staticMesh);
					}
				}
			}
			else
			{
				//As long as one strip has not been culled, add whole mesh for instancing
				MaloW::Array<MeshStrip*>* strips = staticMesh->GetStrips();
				unsigned int index = 0;
				bool oneStripIsNotCulled = false;
				while(!oneStripIsNotCulled && index < strips->size())
				{
					if(!staticMesh->IsStripCulled(index++))
					{
						oneStripIsNotCulled = true;
						CurrentRenderedMeshes++;

						//Add billboard info
						this->instancingHelper->AddBillboard(staticMesh);
					}
				}
			}
		}
		else
		{
			this->invisibleGeometry = true;
		}
	}

#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo Objects Static", 4);
#endif
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Objects Animated", 4);
#endif
	
	// Normal Animated meshes
	for(int i = 0; i < this->animations.size(); i++)
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
				//As long as one strip has not been culled, add whole mesh for instancing
				MaloW::Array<MeshStrip*>* strips = animatedMesh->GetStrips();
				unsigned int index = 0;
				bool oneStripIsNotCulled = false;
				while(!oneStripIsNotCulled && index < strips->size())
				{
					if(!animatedMesh->IsStripCulled(index++))
					{
						oneStripIsNotCulled = true;
						CurrentRenderedMeshes++;
						//Add mesh
						this->instancingHelper->AddAnimatedMesh(animatedMesh, this->Timer);
					}
				}
			}
			else
			{
				//As long as one strip has not been culled, add whole mesh for instancing
				MaloW::Array<MeshStrip*>* strips = animatedMesh->GetStrips();
				unsigned int index = 0;
				bool oneStripIsNotCulled = false;
				while(!oneStripIsNotCulled && index < strips->size())
				{
					if(!animatedMesh->IsStripCulled(index++))
					{
						oneStripIsNotCulled = true;
						CurrentRenderedMeshes++;
						//Add billboard info
						this->instancingHelper->AddBillboard(animatedMesh);
					}
				}
			}
		}
		else
		{
			this->invisibleGeometry = true;
		}
	}

#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo Objects Animated", 4);
#endif


}



void DxManager::RenderDeferredGeometryInstanced()
{
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Objects Instanced", 4);
#endif
#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Objects Static Instanced", 5);
#endif

	//STATIC MESHES(strips)
	if(this->instancingHelper->GetNrOfStrips() > 0)
	{
		//Sort, create instance groups and update buffer before rendering
		this->instancingHelper->PreRenderStrips();
		
		// Set global variables per frame
		this->Shader_DeferredGeometryInstanced->SetFloat("g_FarClip", this->params.FarClip);
		this->Shader_DeferredGeometryInstanced->SetFloat3("g_CamPos", this->camera->GetOldPos());
		this->Shader_DeferredGeometryInstanced->SetMatrix("g_CamViewProj", this->camera->GetViewProjMatrix());
		
		//Set instance buffer
		ID3D11Buffer* bufferPointers[2];
		unsigned int strides[2] = {sizeof(VertexNormalMap), sizeof(StripData::InstancedDataStruct)};
		unsigned int offsets[2] = {0, 0};
		bufferPointers[1] = this->instancingHelper->GetStripInstanceBuffer();	

		//Per Strip group
		for(unsigned int i = 0; i < this->instancingHelper->GetNrOfStripGroups(); ++i)
		{
			StripGroup stripGroup = this->instancingHelper->GetStripGroup(i);
			MeshStrip* strip = stripGroup.s_MeshStrip;

			Object3D* renderObject = strip->GetRenderObject();
			
			//Set topology
			this->Dx_DeviceContext->IASetPrimitiveTopology(renderObject->GetTopology());

			//Set material variables
			this->Shader_DeferredGeometryInstanced->SetFloat4("g_DiffuseColor", D3DXVECTOR4(strip->GetMaterial()->DiffuseColor, 1));
			this->Shader_DeferredGeometryInstanced->SetFloat3("g_SpecularColor", D3DXVECTOR3(strip->GetMaterial()->SpecularColor));
			this->Shader_DeferredGeometryInstanced->SetFloat("g_SpecularPower", strip->GetMaterial()->SpecularPower);
				
			//Set textures
			if(renderObject->GetTextureResource() != NULL)
			{
				if(renderObject->GetTextureResource()->GetSRVPointer() != NULL)
				{
					this->Shader_DeferredGeometryInstanced->SetResource("g_DiffuseMap", renderObject->GetTextureResource()->GetSRVPointer());
					this->Shader_DeferredGeometryInstanced->SetBool("g_Textured", true);
					
					//Normal map
					if(renderObject->GetNormalMapResource() != NULL)
					{
						this->Shader_DeferredGeometryInstanced->SetResource("g_NormalMap", renderObject->GetNormalMapResource()->GetSRVPointer());
						this->Shader_DeferredGeometryInstanced->SetBool("g_UseNormalMap", true);
					}
					else
					{
						this->Shader_DeferredGeometryInstanced->SetResource("g_NormalMap", NULL);
						this->Shader_DeferredGeometryInstanced->SetBool("g_UseNormalMap", false);
					}
				}
				else
				{
					this->Shader_DeferredGeometryInstanced->SetResource("g_DiffuseMap", NULL);
					this->Shader_DeferredGeometryInstanced->SetResource("g_NormalMap", NULL);
					this->Shader_DeferredGeometryInstanced->SetBool("g_Textured", false);
					this->Shader_DeferredGeometryInstanced->SetBool("g_UseNormalMap", false);
				}
			}
			else
			{
				this->Shader_DeferredGeometryInstanced->SetResource("g_DiffuseMap", NULL);
				this->Shader_DeferredGeometryInstanced->SetResource("g_NormalMap", NULL);
				this->Shader_DeferredGeometryInstanced->SetBool("g_Textured", false);
				this->Shader_DeferredGeometryInstanced->SetBool("g_UseNormalMap", false);
			}

			//Change vertex buffer and set it and the instance buffer.
			bufferPointers[0] = renderObject->GetVertexBufferResource()->GetBufferPointer()->GetBufferPointer();
			this->Dx_DeviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
			
			//Apply pass and input layout
			this->Shader_DeferredGeometryInstanced->Apply(0);
				
			//Draw
			unsigned int vertexCount = strip->getNrOfVerts();
			int instanceCount = this->instancingHelper->GetStripGroup(i).s_Size;
			int startLoc = this->instancingHelper->GetStripGroup(i).s_StartLocation;
			this->Dx_DeviceContext->DrawInstanced(vertexCount, instanceCount, 0, startLoc); 
			
			//Debug data
			this->CurrentNrOfDrawCalls++;
			this->CurrentRenderedNrOfVertices += vertexCount;
		}
		
		//Reset data for next frame. 
		this->instancingHelper->PostRenderStrips();
	}
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo Objects Static Instanced", 5);
#endif

#ifdef MALOWTESTPERF
	this->perf.PreMeasure("Renderer - Render Deferred Geo Objects Animated Instanced", 5);
#endif
	//ANIMATED MESHES(strips)
	if(this->instancingHelper->GetNrOfAnimatedStrips() > 0)
	{
		//Sort, create instance groups and update buffer before rendering
		this->instancingHelper->PreRenderAnimatedStrips();

		//Draw animated mesh(strips) groups (instanced)
		//Set variables per frame:
		this->Shader_DeferredAnimatedGeometryInstanced->SetFloat("g_FarClip", this->params.FarClip);
		this->Shader_DeferredAnimatedGeometryInstanced->SetFloat3("g_CamPos", this->camera->GetOldPos());
		this->Shader_DeferredAnimatedGeometryInstanced->SetMatrix("g_CamViewProj", this->camera->GetViewProjMatrix());
		
		//"Set" instance buffer
		ID3D11Buffer* bufferPointers[3];
		unsigned int strides[3] = { sizeof(VertexNormalMap), 
									sizeof(VertexNormalMap), 
									sizeof(AnimatedStripData::AnimatedInstancedDataStruct)};
		unsigned int offsets[3] = {0, 0, 0};
		bufferPointers[2] = this->instancingHelper->GetAnimatedStripInstanceBuffer();	

		//Set variables per animated strip group:
		for(unsigned int i = 0; i < this->instancingHelper->GetNrOfAnimatedStripGroups(); ++i)
		{
			AnimatedStripGroup stripGroup = this->instancingHelper->GetAnimatedStripGroup(i);
			MeshStrip* stripOne = stripGroup.s_MeshStripOne;
			MeshStrip* stripTwo = stripGroup.s_MeshStripTwo;
			Object3D* renderObjectOne = stripOne->GetRenderObject();
			Object3D* renderObjectTwo = stripTwo->GetRenderObject();

			//Set topology (both use same topology)
			this->Dx_DeviceContext->IASetPrimitiveTopology(renderObjectOne->GetTopology());

			//Set material variables
			this->Shader_DeferredAnimatedGeometryInstanced->SetFloat4("g_DiffuseColor", D3DXVECTOR4(stripOne->GetMaterial()->DiffuseColor, 1.0f));
			this->Shader_DeferredAnimatedGeometryInstanced->SetFloat3("g_SpecularColor", D3DXVECTOR3(stripOne->GetMaterial()->SpecularColor));
			this->Shader_DeferredAnimatedGeometryInstanced->SetFloat("g_SpecularPower", stripOne->GetMaterial()->SpecularPower);
			
			//Set textures (different texture between render objects is not supported)
			if(renderObjectOne->GetTextureResource() != NULL)
			{
				if(renderObjectOne->GetTextureResource()->GetSRVPointer() != NULL)
				{
					this->Shader_DeferredAnimatedGeometryInstanced->SetResource("g_DiffuseMap", renderObjectOne->GetTextureResource()->GetSRVPointer());
					this->Shader_DeferredAnimatedGeometryInstanced->SetBool("g_Textured", true);

					//Normal map
					if(renderObjectOne->GetNormalMapResource() != NULL)
					{
						this->Shader_DeferredAnimatedGeometryInstanced->SetResource("g_NormalMap", renderObjectOne->GetNormalMapResource()->GetSRVPointer());
						this->Shader_DeferredAnimatedGeometryInstanced->SetBool("g_UseNormalMap", true);

					}
					else
					{
						this->Shader_DeferredAnimatedGeometryInstanced->SetResource("g_NormalMap", NULL);
						this->Shader_DeferredAnimatedGeometryInstanced->SetBool("g_UseNormalMap", false);
					}
				}
				else
				{
					this->Shader_DeferredAnimatedGeometryInstanced->SetResource("g_DiffuseMap", NULL);
					this->Shader_DeferredAnimatedGeometryInstanced->SetResource("g_NormalMap", NULL);
					this->Shader_DeferredAnimatedGeometryInstanced->SetBool("g_Textured", false);
				}
			}
			else
			{
				this->Shader_DeferredAnimatedGeometryInstanced->SetResource("g_DiffuseMap", NULL);
				this->Shader_DeferredAnimatedGeometryInstanced->SetResource("g_NormalMap", NULL);
				this->Shader_DeferredAnimatedGeometryInstanced->SetBool("g_Textured", false);
			}

			//Change vertex buffers and set them and the instance buffer.
			bufferPointers[0] = renderObjectOne->GetVertexBufferResource()->GetBufferPointer()->GetBufferPointer();
			bufferPointers[1] = renderObjectTwo->GetVertexBufferResource()->GetBufferPointer()->GetBufferPointer();
			this->Dx_DeviceContext->IASetVertexBuffers(0, 3, bufferPointers, strides, offsets);

			//Apply pass and input layout
			this->Shader_DeferredAnimatedGeometryInstanced->Apply(0);

			//Draw
			unsigned int vertexCount = stripOne->getNrOfVerts(); //Both strips contain the same amount of vertices.
			int instanceCount = this->instancingHelper->GetAnimatedStripGroup(i).s_Size;
			int startLoc = this->instancingHelper->GetAnimatedStripGroup(i).s_StartLocation;
			this->Dx_DeviceContext->DrawInstanced(vertexCount, instanceCount, 0, startLoc); 

			//Debug data
			this->CurrentNrOfDrawCalls++;
			this->CurrentRenderedNrOfVertices+= vertexCount * 2;
		}

		//Reset data for next frame.
		this->instancingHelper->PostRenderAnimatedStrips();
	}
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo Objects Animated Instanced", 5);
#endif
#ifdef MALOWTESTPERF
	this->Dx_DeviceContext->Flush();
	this->perf.PostMeasure("Renderer - Render Deferred Geo Objects Instanced", 4);
#endif
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

	this->Shader_Skybox->SetFloat3("fogColor", this->fogColor);

	//MeshStrip* strip = this->skybox->GetStrips()->get(0);
	MeshStrip* strip = this->skybox->GetStrip();

	Object3D* obj = strip->GetRenderObject();
	this->Dx_DeviceContext->IASetPrimitiveTopology(obj->GetTopology());

	obj->GetVertBuff()->Apply();
	obj->GetIndsBuff()->Apply();
	if(obj->GetTextureResource() != NULL)
	{
		this->Shader_Skybox->SetResource("SkyMap", obj->GetTextureResource()->GetSRVPointer());
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
	// If special circle is used
	if(this->specialCircleParams.x) //if inner radius > 0, then send/set data
	{
		this->Shader_DeferredLightning->SetFloat4("dataPPHA", this->specialCircleParams);
	}
	//Always tell the shader whether to use shadows or not.
	this->Shader_DeferredLightning->SetBool("useShadow", this->useShadow);
	if ( useShadow )
	{
		float PCF_SIZE = (float)this->params.ShadowMapSettings + 1;
		float PCF_SQUARED = 1 / (PCF_SIZE * PCF_SIZE);

		this->Shader_DeferredLightning->SetFloat("SMAP_DX", 1.0f / (256.0f * powf(2.0f, (float)this->params.ShadowMapSettings * 0.5f)));

		this->Shader_DeferredLightning->SetFloat("PCF_SIZE", PCF_SIZE);
		this->Shader_DeferredLightning->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);

		this->Shader_DeferredLightning->SetBool("blendCascades", true); //Todo: CSM-variable
		this->Shader_DeferredLightning->SetFloat("blendDistance", this->csm->GetBlendDistance()); 
		this->Shader_DeferredLightning->SetFloat("blendStrength", 0.0f); //Todo: CSM-variable

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
			this->Shader_DeferredLightning->SetStructMemberAtIndexAsMatrix(l, "gCascades", "viewProj", lvp);

		}
	}

	// Set sun-settings
	if(this->useSun) 
	{
		this->Shader_DeferredLightning->SetStructMemberAsFloat4("gSun", "Direction", D3DXVECTOR4(this->sun.direction, 0.0f));
		this->Shader_DeferredLightning->SetStructMemberAsFloat4("gSun", "LightColor", D3DXVECTOR4(this->sun.lightColor, 0.0f));
		this->Shader_DeferredLightning->SetStructMemberAsFloat("gSun", "LightIntensity", this->sun.intensity);
	}
	//Always tell the shader whether to use sun or not.
	this->Shader_DeferredLightning->SetBool("gUseSun", this->useSun);

	//DeferredLightning.fx:
	this->Shader_DeferredLightning->SetResource("Texture", this->Dx_GbufferSRVs[0]);
	this->Shader_DeferredLightning->SetResource("NormalAndDepth", this->Dx_GbufferSRVs[1]);
	this->Shader_DeferredLightning->SetResource("Position", this->Dx_GbufferSRVs[2]);
	this->Shader_DeferredLightning->SetResource("Specular", this->Dx_GbufferSRVs[3]);
	D3DXMATRIX vp = this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix();
	
	this->Shader_DeferredLightning->SetMatrix("gCameraVP", vp);
	this->Shader_DeferredLightning->SetFloat3("gCameraPosition", this->camera->GetOldPos());
	this->Shader_DeferredLightning->SetFloat("gNrOfLights", (float)this->lights.size());
	this->Shader_DeferredLightning->SetFloat3("gSceneAmbientLight", this->sceneAmbientLight);

	this->Shader_DeferredLightning->SetFloat3("fogColor", this->fogColor);
	

	//ssao.fx:
	// this->ssao->PreRender(this->Shader_DeferredLightning, this->params, this->camera);

	this->Shader_DeferredLightning->Apply(0);
	
	this->Dx_DeviceContext->Draw(1, 0);
	
	// Unbind resources:
	this->Shader_DeferredLightning->SetResource("Texture", NULL);
	this->Shader_DeferredLightning->SetResource("NormalAndDepth", NULL);
	this->Shader_DeferredLightning->SetResource("Position", NULL);
	this->Shader_DeferredLightning->SetResource("Specular", NULL);
	for(int i = 0; i < this->lights.size(); i++)
	{
		this->Shader_DeferredLightning->SetResourceAtIndex(i, "ShadowMap", NULL);
	}
	for(int i = 0; i < this->csm->GetNrOfCascadeLevels(); i++)
		this->Shader_DeferredLightning->SetResourceAtIndex(i, "CascadedShadowMap", NULL);

	// Unbind SSAO
	//this->ssao->PostRender(this->Shader_DeferredLightning);

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
	this->Shader_DeferredQuad->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetOldPos(), 1));
	
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
	// need to not clear this for fog effect to work.
	//this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[2], ClearColor2);
	this->Dx_DeviceContext->ClearRenderTargetView(this->Dx_GbufferRTs[3], ClearColor2);

	//Static meshes// If special circle is used
	if(this->specialCircleParams.x) //if inner radius > 0, then send/set data
	{
		this->Shader_DeferredLightning->SetFloat4("dataPPHA", this->specialCircleParams);
	}
	this->Shader_DeferredGeoTranslucent->SetFloat4("gCameraPosition", D3DXVECTOR4(this->camera->GetOldPos(), 1));
	this->Shader_DeferredGeoTranslucent->SetFloat("gFarClip", this->params.FarClip);
	this->Shader_DeferredGeoTranslucent->SetFloat("gTimerMillis", this->Timer);
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

			if(wp->GetTextureResource() != NULL)
			{
				if(ID3D11ShaderResourceView* texture = wp->GetTextureResource()->GetSRVPointer())
				{
					this->Shader_DeferredGeoTranslucent->SetBool("textured", true);
					this->Shader_DeferredGeoTranslucent->SetResource("tex2D", texture);					
				}
				else
				{
					this->Shader_DeferredGeoTranslucent->SetBool("textured", false);
					this->Shader_DeferredGeoTranslucent->SetResource("tex2D", NULL);
				}
			}
			else
			{
				this->Shader_DeferredGeoTranslucent->SetBool("textured", false);
				this->Shader_DeferredGeoTranslucent->SetResource("tex2D", NULL);
			}
			

			this->Shader_DeferredGeoTranslucent->Apply(0);
			this->Dx_DeviceContext->Draw(verts->GetElementCount(), 0);
		}
	}
	// Unbind resources static geometry:
	this->Shader_DeferredGeoTranslucent->SetResource("tex2D", NULL);
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

		this->Shader_DeferredPerPixelTranslucent->SetFloat("SMAP_DX", 1.0f / (256.0f * powf(2.0f, (float)this->params.ShadowMapSettings * 0.5f)));

		this->Shader_DeferredPerPixelTranslucent->SetFloat("PCF_SIZE", PCF_SIZE);
		this->Shader_DeferredPerPixelTranslucent->SetFloat("PCF_SIZE_SQUARED", PCF_SQUARED);

		this->Shader_DeferredPerPixelTranslucent->SetBool("blendCascades", true); //Todo: CSM-variable
		this->Shader_DeferredPerPixelTranslucent->SetFloat("blendDistance", this->csm->GetBlendDistance()); 
		this->Shader_DeferredPerPixelTranslucent->SetFloat("blendStrength", 0.0f); //Todo: CSM-variable

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
			this->Shader_DeferredPerPixelTranslucent->SetStructMemberAtIndexAsMatrix(l, "gCascades", "viewProj", lvp);
		}

	}


	// Set sun-settings
	if(this->useSun) 
	{
		this->Shader_DeferredPerPixelTranslucent->SetStructMemberAsFloat4("gSun", "Direction", D3DXVECTOR4(this->sun.direction, 0.0f));
		this->Shader_DeferredPerPixelTranslucent->SetStructMemberAsFloat4("gSun", "LightColor", D3DXVECTOR4(this->sun.lightColor, 0.0f));
		this->Shader_DeferredPerPixelTranslucent->SetStructMemberAsFloat("gSun", "LightIntensity", this->sun.intensity);
	}
	//Always tell the shader whether to use sun or not.
	this->Shader_DeferredPerPixelTranslucent->SetBool("gUseSun", this->useSun);

	//DeferredLightning.fx:
	this->Shader_DeferredPerPixelTranslucent->SetResource("Texture", this->Dx_GbufferSRVs[0]);
	this->Shader_DeferredPerPixelTranslucent->SetResource("NormalAndDepth", this->Dx_GbufferSRVs[1]);
	this->Shader_DeferredPerPixelTranslucent->SetResource("Position", this->Dx_GbufferSRVs[2]);
	this->Shader_DeferredPerPixelTranslucent->SetResource("Specular", this->Dx_GbufferSRVs[3]);
	D3DXMATRIX vp = this->camera->GetViewMatrix() * this->camera->GetProjectionMatrix();
	
	
	this->Shader_DeferredPerPixelTranslucent->SetMatrix("gCameraVP", vp);
	this->Shader_DeferredPerPixelTranslucent->SetFloat3("gCameraPosition", this->camera->GetOldPos());
	this->Shader_DeferredPerPixelTranslucent->SetFloat("gNrOfLights", (float)this->lights.size()); 
	this->Shader_DeferredPerPixelTranslucent->SetFloat3("gSceneAmbientLight", this->sceneAmbientLight);
	
	this->Shader_DeferredPerPixelTranslucent->SetFloat3("fogColor", this->fogColor);



	//ssao.fx:
	// this->ssao->PreRender(this->Shader_DeferredPerPixelTranslucent, this->params, this->camera);

	this->Shader_DeferredPerPixelTranslucent->Apply(0);
	
	this->Dx_DeviceContext->Draw(1, 0);

	
	// Unbind resources:
	this->Shader_DeferredPerPixelTranslucent->SetResource("Texture", NULL);
	this->Shader_DeferredPerPixelTranslucent->SetResource("NormalAndDepth", NULL);
	this->Shader_DeferredPerPixelTranslucent->SetResource("Position", NULL);
	this->Shader_DeferredPerPixelTranslucent->SetResource("Specular", NULL);
	for(int i = 0; i < this->lights.size(); i++)
	{
		this->Shader_DeferredPerPixelTranslucent->SetResourceAtIndex(i, "ShadowMap", NULL);
	}
	for(int i = 0; i < this->csm->GetNrOfCascadeLevels(); i++)
		this->Shader_DeferredPerPixelTranslucent->SetResourceAtIndex(i, "CascadedShadowMap", NULL);

	// Unbind SSAO
	//this->ssao->PostRender(this->Shader_DeferredPerPixelTranslucent);

	this->Shader_DeferredPerPixelTranslucent->Apply(0);
}

void DxManager::RenderFBXMeshes()
{
	this->Dx_DeviceContext->OMSetRenderTargets(this->NrOfRenderTargets, this->Dx_GbufferRTs, this->Dx_DepthStencilView);
	this->Shader_FBX->SetFloat4("CameraPosition", D3DXVECTOR4(this->camera->GetOldPos(), 1));
	this->Shader_FBX->SetFloat("NearClip", this->params.NearClip);
	this->Shader_FBX->SetFloat("FarClip", this->params.FarClip);


	// Should be per mesh, future....
	this->Shader_FBX->SetInt("specialColor", 0);
	this->Shader_FBX->SetFloat4("SpecularColor", D3DXVECTOR4(D3DXVECTOR3(0.05f, 0.05f, 0.05f), 1));
	this->Shader_FBX->SetFloat("SpecularPower", 30.0f);
	this->Shader_FBX->SetFloat4("AmbientLight", D3DXVECTOR4(D3DXVECTOR3(0.2f, 0.2f, 0.2f), 1));
	this->Shader_FBX->SetFloat4("DiffuseColor", D3DXVECTOR4(D3DXVECTOR3(0.6f, 0.6f, 0.6f), 1));
	//


	Shader_FBX->Apply(0);
	float dt = this->Timer - this->LastFBXUpdate;
	for(int i = 0; i < this->FBXMeshes.size(); i++)
	{
		if(!this->FBXMeshes[i]->IsCulled())
		{
			this->CurrentRenderedFBX++;
			this->FBXMeshes[i]->Update(dt);
			this->FBXMeshes[i]->Render(dt, this->camera->GetProjectionMatrix(), this->camera->GetViewMatrix(), this->camera->GetViewProjMatrix(), this->Shader_FBX, this->Dx_DeviceContext);
		}
	}

	this->LastFBXUpdate = this->Timer;
}
void DxManager::PostRender()
{
	//ev. todo: rendered vertices for shadows?
	this->RenderedMeshes = this->CurrentRenderedMeshes;
	this->RenderedTerrains = this->CurrentRenderedTerrains;
	this->renderedFBX = this->CurrentRenderedFBX;

	this->NrOfDrawnVertices = this->CurrentRenderedNrOfVertices;
	this->NrOfDrawCalls = this->CurrentNrOfDrawCalls;

	//this->renderedMeshShadows = 0;
	//this->renderedTerrainShadows = 0;
}
