#include "DxManager.h"

DxManager::DxManager(HWND g_hWnd, GraphicsEngineParams params, Camera* cam)
{
#ifdef INCLUDE_NDYGFX
	MaloW::Debug("(DEBUG): NDYGFX: vld.h included.");
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	MaloW::Debug("(DEBUG): NDYGFX: Debug flag set to: _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF). ");
#endif
	this->StartRender = false;
	this->params = params;
	this->hWnd = g_hWnd;

	this->helperThread = NULL;

	this->Dx_DeviceContext = NULL;
	this->Dx_DepthStencilView = NULL;
	this->Dx_DepthStencil = NULL;
	this->Dx_RenderTargetView = NULL;
	this->Shader_ForwardRendering = NULL;
	this->Dx_SwapChain = NULL;
	this->Dx_Device = NULL;

	this->Shader_ShadowMapFBX = NULL;

	this->DelayGettingCamera = false;

	this->Shader_ShadowMap = NULL;
	this->Shader_ShadowMapInstanced = NULL;
	this->Shader_ShadowMapBillboardInstanced = NULL;
	this->Shader_ShadowMapAnimated = NULL;
	this->Shader_ShadowMapAnimatedInstanced = NULL;

	this->Shader_FBX = NULL;

	this->RenderedMeshes = 0;
	this->RenderedTerrains = 0;
	this->renderedMeshShadows = 0;
	this->renderedTerrainShadows = 0;
	this->renderedFBX = 0;
	this->NrOfDrawnVertices = 0;
	this->NrOfDrawCalls = 0;

	this->Shader_Text = NULL;
	this->Shader_Image = NULL;
	this->Shader_Decal = NULL;

	for(int i = 0; i < NrOfRenderTargets; i++)
	{
		this->Dx_GbufferTextures[i] = NULL;
		this->Dx_GbufferRTs[i] = NULL;
		this->Dx_GbufferSRVs[i] = NULL;
	}
	
	this->useShadow = params.ShadowMapSettings > 0;
	this->csm = NULL;

	this->Shader_DeferredGeometry = NULL;
	this->Shader_DeferredGeometryInstanced = NULL;
	this->Shader_DeferredAnimatedGeometry = NULL;
	this->Shader_DeferredAnimatedGeometryInstanced = NULL;

	this->Shader_TerrainEditor = NULL;
	this->Shader_DeferredLightning = NULL;
	this->Shader_InvisibilityEffect = NULL;

	this->Shader_DeferredGeoTranslucent = NULL;
	this->Shader_DeferredPerPixelTranslucent = NULL;

	this->Shader_DeferredQuad = NULL;
	this->Shader_DeferredTexture = NULL;
	this->Shader_FogEnclosement = NULL;
	this->fbx = NULL;

	this->Dx_DeferredTexture = NULL;
	this->Dx_DeferredQuadRT = NULL;
	this->Dx_DeferredSRV = NULL;
	this->skybox = NULL;
	this->Shader_Skybox = NULL;

	this->ssao = NULL;
	this->fxaa = NULL;

	this->useEnclosingFog = false;
	this->fogCenter = Vector3(0, 0, 0);
	this->fogRadius = 0.0f;
	this->fogFadeFactor = 0.0f;


	this->instancingHelper = NULL;

	this->framecount = 0;
	this->TriangleCount = 0;
	this->RendererSleep = 0;
	this->LastCamUpdate = 0;
	this->LastFBXUpdate = 0;

	this->useSun = false;
	this->sceneAmbientLight = D3DXVECTOR3(0.2f, 0.2f, 0.2f);

	this->camera = cam;

	this->specialCircleParams = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

	if(FAILED(this->Init()))
		MaloW::Debug("Failed to init DxManager");

	this->Timer = 0.0f;
	LARGE_INTEGER li;
	if(!QueryPerformanceFrequency(&li))
		MaloW::Debug("QueryPerformanceFrequency Failed!, High resolution performance counter not available?");

	this->PCFreq = float(li.QuadPart)/1000.0f;
	QueryPerformanceCounter(&li);
	this->prevTimeStamp = li.QuadPart;
}

DxManager::~DxManager()
{
	if(this->helperThread)
	{
		this->helperThread->Close();
		this->helperThread->WaitUntillDone();
		delete this->helperThread;
		this->helperThread = NULL;
	}

	if(this->camera)
		delete this->camera;

	if(this->Shader_ForwardRendering)
		delete this->Shader_ForwardRendering;

	if(this->Shader_ShadowMap)
		delete this->Shader_ShadowMap;
	if(this->Shader_ShadowMapInstanced)
	{
		delete this->Shader_ShadowMapInstanced;
		this->Shader_ShadowMapInstanced = NULL;
	}

	if(this->Shader_ShadowMapBillboardInstanced) 
	{
		delete this->Shader_ShadowMapBillboardInstanced, this->Shader_ShadowMapBillboardInstanced = NULL;
	}

	if ( this->Shader_ShadowMapAnimated ) delete this->Shader_ShadowMapAnimated, this->Shader_ShadowMapAnimated=0;
	
	if(this->Shader_ShadowMapAnimatedInstanced)
	{
		delete this->Shader_ShadowMapAnimatedInstanced;
		this->Shader_ShadowMapAnimatedInstanced = NULL;
	}

	if(this->Shader_Image)
		delete this->Shader_Image;
	
	if(this->instancingHelper) 
	{
		delete this->instancingHelper; 
		this->instancingHelper = NULL;
	}

	if(this->Shader_BillboardInstanced)
	{
		delete this->Shader_BillboardInstanced;
		this->Shader_BillboardInstanced = NULL;
	}

	if(this->Shader_DeferredGeometry)
		delete this->Shader_DeferredGeometry;

	if(this->Shader_DeferredGeometryInstanced)
		delete this->Shader_DeferredGeometryInstanced;

	if(this->Shader_DeferredAnimatedGeometry)
		delete this->Shader_DeferredAnimatedGeometry;

	if(this->Shader_DeferredAnimatedGeometryInstanced)
		delete this->Shader_DeferredAnimatedGeometryInstanced;
	
	if(this->Shader_TerrainEditor)
	{
		delete this->Shader_TerrainEditor;
		this->Shader_TerrainEditor = NULL;
	}

	if(this->Shader_DeferredLightning)
		delete this->Shader_DeferredLightning;

	if(this->Shader_DeferredGeoTranslucent)
		delete this->Shader_DeferredGeoTranslucent;

	if(this->Shader_ShadowMapFBX)
		delete this->Shader_ShadowMapFBX;

	if(this->Shader_Decal)
		delete this->Shader_Decal;

	if(this->Shader_FogEnclosement)
		delete this->Shader_FogEnclosement;

	if(this->Shader_DeferredPerPixelTranslucent)
		delete this->Shader_DeferredPerPixelTranslucent;

	if ( this->Shader_InvisibilityEffect ) delete this->Shader_InvisibilityEffect, this->Shader_InvisibilityEffect=0;

	if(this->Shader_DeferredQuad)
		delete this->Shader_DeferredQuad;

	if(this->Shader_DeferredTexture)
		delete this->Shader_DeferredTexture;

	

	if(this->Shader_FBX)
		delete this->Shader_FBX;

	/*
	if(this->Dx_DeferredTexture)
		this->Dx_DeferredTexture->Release();
	if(this->Dx_DeferredQuadRT)
		this->Dx_DeferredQuadRT->Release();
	if(this->Dx_DeferredSRV)
		this->Dx_DeferredSRV->Release();
	*/

	if ( this->skybox ) delete this->skybox, this->skybox=0;

	if(this->Shader_Skybox)
		delete this->Shader_Skybox;

	if(this->Shader_Text)
		delete this->Shader_Text;

	if ( this->ssao ) delete this->ssao, this->ssao=0;
	if ( this->fxaa ) delete this->fxaa, this->fxaa=0;
	if ( this->Shader_Fxaa ) delete this->Shader_Fxaa, this->Shader_Fxaa=0;
	if(this->csm)
	{
		delete this->csm;
		this->csm = NULL;
	}

	if(this->Dx_DeviceContext)
		this->Dx_DeviceContext->Release();
	if(this->Dx_DepthStencilView)
		this->Dx_DepthStencilView->Release();
	if(this->Dx_DepthStencil)
		this->Dx_DepthStencil->Release();
	if(this->Dx_RenderTargetView)
		this->Dx_RenderTargetView->Release();
	if(this->Dx_SwapChain)
		this->Dx_SwapChain->Release();
	if(this->Dx_Device)
		this->Dx_Device->Release();

	for(int i = 0; i < NrOfRenderTargets; i++)
	{
		if(this->Dx_GbufferTextures[i])
			this->Dx_GbufferTextures[i]->Release();
		if(this->Dx_GbufferRTs[i])
			this->Dx_GbufferRTs[i]->Release();
		if(this->Dx_GbufferSRVs[i])
			this->Dx_GbufferSRVs[i]->Release();
	}


	while(0 < this->images.size())
		delete this->images.getAndRemove(0);

	while(0 < this->billboards.size()) 
		delete this->billboards.getAndRemove(0);
	
	while(0 < this->billboardCollections.size()) 
		delete this->billboardCollections.getAndRemove(0);

	while(0 < this-> terrains.size())
		delete this-> terrains.getAndRemove(0);

	while(0 < this->objects.size())
		delete this->objects.getAndRemove(0);

	while(0 < this->animations.size())
		delete this->animations.getAndRemove(0);

	while(0 < this->lights.size())
		delete this->lights.getAndRemove(0);

	while(0 < this->texts.size())
		delete this->texts.getAndRemove(0);

	while(0 < this->waterplanes.size())
		delete this->waterplanes.getAndRemove(0);

	while(0 < this->FBXMeshes.size())
		delete this->FBXMeshes.getAndRemove(0);

	while(0 < this->decals.size())
		delete this->decals.getAndRemove(0);

	// Empty event queue 
	while(MaloW::ProcessEvent* ev = this->PeekEvent())
	{
		delete ev;
	}

	FreeResourceManager();
}

void DxManager::CreateTerrain(Terrain* terrain)
{
	//OBS! Do not use resource manager to create buffers since they are (most likely) unique for every terrain.


	//Create vertex buffer
	BUFFER_INIT_DESC vertexBufferDesc;
	vertexBufferDesc.ElementSize = sizeof(Vertex);
	vertexBufferDesc.InitData = terrain->GetVerticesPointer(); 
	vertexBufferDesc.NumElements = terrain->GetNrOfVertices();
	vertexBufferDesc.Type = VERTEX_BUFFER;
	vertexBufferDesc.Usage = BUFFER_CPU_WRITE_DISCARD;

	Buffer* vertexBuffer = new Buffer();
	HRESULT hr;
	hr = vertexBuffer->Init(this->Dx_Device, this->Dx_DeviceContext, vertexBufferDesc);
	if(FAILED(hr))
	{
		delete vertexBuffer;
		vertexBuffer = NULL;

		MaloW::Debug("ERROR: Could not create vertex buffer. REASON: DxManager: CreateTerrain(Terrain* terrain)."
		+ string("ERROR code: '") 
		+ MaloW::GetHRESULTErrorCodeString(hr));
	}
	terrain->SetVertexBuffer(vertexBuffer);

	//Create index buffer
	Buffer* indexBuffer = NULL;
	if(terrain->GetIndicesPointer()) //Check if indices are used
	{
		BUFFER_INIT_DESC indexBufferDesc;
		indexBufferDesc.ElementSize = sizeof(int);
		indexBufferDesc.InitData = terrain->GetIndicesPointer();
		indexBufferDesc.NumElements = terrain->GetNrOfIndices();
		indexBufferDesc.Type = INDEX_BUFFER;
		indexBufferDesc.Usage = BUFFER_DEFAULT;

		indexBuffer = new Buffer();
		//**tmp testing: creating index buffer sometimes fails and generates slenda error**
		
		hr = indexBuffer->Init(this->Dx_Device, this->Dx_DeviceContext, indexBufferDesc);
		if(FAILED(hr))
		{
			delete indexBuffer; 
			indexBuffer = NULL;

			MaloW::Debug("ERROR: Could not create index buffer. REASON: DxManager: CreateTerrain(Terrain* terrain)."
				+ string("ERROR code: '") 
				+ MaloW::GetHRESULTErrorCodeString(hr));
		}
		terrain->SetIndexBuffer(indexBuffer);
	}

	//Texture(s) are not set here since they are set afterwards.

	//terrain->Init(this->Dx_Device, this->Dx_DeviceContext);

	//Create & put this event
	TerrainEvent* re = new TerrainEvent(true, terrain);
	this->PutEvent(re);
}

void DxManager::CreateStaticMesh(StaticMesh* mesh)
{
	//Per Strip data
	MaloW::Array<MeshStrip*>* strips = mesh->GetStrips();
	if(strips->size() > 0)
	{
		//Check if a previous mesh already has set data.
		if(strips->get(0)->GetRenderObject() == NULL)
		{
			for(int i = 0; i < strips->size(); i++)
			{
				MeshStrip* strip = strips->get(i);

				BUFFER_INIT_DESC bufferDesc;
				bufferDesc.ElementSize = sizeof(VertexNormalMap);
				bufferDesc.InitData = strip->getVerts();
		
		
				// Last face black, should +1 this to solve it.
				bufferDesc.NumElements = strip->getNrOfVerts();

				bufferDesc.Type = VERTEX_BUFFER;
				bufferDesc.Usage = BUFFER_DEFAULT;
	
				string resourceNameVertices = mesh->GetFilePath() + string("Strip") + MaloW::convertNrToString(i) + string("Vertices");
				BufferResource* verts = GetResourceManager()->CreateBufferResource(resourceNameVertices.c_str(), bufferDesc);

				BufferResource* inds = NULL; 
				if(strip->getIndicies())
				{
					BUFFER_INIT_DESC bufferInds;
					bufferInds.ElementSize = sizeof(int);
					bufferInds.InitData = strip->getIndicies();
					bufferInds.NumElements = strip->getNrOfIndicies();
					bufferInds.Type = INDEX_BUFFER;
					bufferInds.Usage = BUFFER_DEFAULT;
	

					string resourceNameIndices = mesh->GetFilePath() + string("Strip") + MaloW::convertNrToString(i) + string("Indices");
					inds = GetResourceManager()->CreateBufferResource(resourceNameIndices.c_str(), bufferInds);
				}

				TextureResource* texture = NULL;
				TextureResource* normalMap = NULL;
				string texturePath = strip->GetTexturePath();
				if(texturePath != "")
				{
					texture = GetResourceManager()->CreateTextureResourceFromFile(texturePath.c_str(), true);
					string ending = texturePath.substr(texturePath.length()-4);
					string first = texturePath.substr(0, texturePath.length()-4);
					string normalTexturePath = first + "_n" + ending;
					normalMap = GetResourceManager()->CreateTextureResourceFromFile(normalTexturePath.c_str(), true);
				}

				Object3D* obj = new Object3D(verts, inds, texture, normalMap, mesh->GetTopology()); 
				strip->SetRenderObject(obj);
			}
		}
	}
	else
	{
		MaloW::Debug("WARNING: DxManager::CreateStaticMesh(): Strips of mesh has not been set for: '" + string(mesh->GetFilePath()));
	}

	//Per Mesh data
	TextureResource* billboardTexture = NULL;
	if(mesh->GetBillboardFilePath() != "")
	{
		billboardTexture = GetResourceManager()->CreateTextureResourceFromFile(mesh->GetBillboardFilePath().c_str(), true);
	}
	mesh->GetBillboardGFX()->SetTextureResource(billboardTexture);


	mesh->RecreateWorldMatrix(); 
	
	StaticMeshEvent* re = new StaticMeshEvent(true, mesh);
	this->PutEvent(re);
}


void DxManager::CreateAnimatedMesh(AnimatedMesh* mesh)
{
	MaloW::Array<KeyFrame*>* kfs = mesh->GetKeyFrames();
	
	for(int j = 0; j < kfs->size(); j++)
	{
		MaloW::Array<MeshStrip*>* strips = kfs->get(j)->meshStripsResource->GetMeshStripsPointer();

		//Check if a previous mesh already has set data.
		//Per strip data
		if(strips->get(0)->GetRenderObject() == NULL)
		{
			for(int i = 0; i < strips->size(); i++)
			{
				MeshStrip* strip = strips->get(i);

				BUFFER_INIT_DESC bufferDesc;
				bufferDesc.ElementSize = sizeof(VertexNormalMap);
				bufferDesc.InitData = strip->getVerts();
		
		
				// Last face black, should +1 this to solve it.
				bufferDesc.NumElements = strip->getNrOfVerts();
				bufferDesc.Type = VERTEX_BUFFER;
				bufferDesc.Usage = BUFFER_DEFAULT;
			
				string resourceNameVertices = mesh->GetFilePath() + string("Keyframe") + MaloW::convertNrToString(j) + string("Strip") + MaloW::convertNrToString(i) + string("Vertices");
				BufferResource* verts = GetResourceManager()->CreateBufferResource(resourceNameVertices.c_str(), bufferDesc);

				BufferResource* inds = NULL;
				if(strip->getIndicies())
				{
					BUFFER_INIT_DESC bufferInds;
					bufferInds.ElementSize = sizeof(int);
					bufferInds.InitData = strip->getIndicies();
					bufferInds.NumElements = strip->getNrOfIndicies();
					bufferInds.Type = INDEX_BUFFER;
					bufferInds.Usage = BUFFER_DEFAULT;
				
					string resourceNameIndices = mesh->GetFilePath() + string("Keyframe") + MaloW::convertNrToString(j) + string("Strip") + MaloW::convertNrToString(i) + string("Indices");

					inds = GetResourceManager()->CreateBufferResource(resourceNameIndices.c_str(), bufferInds);
				}

				TextureResource* texture = NULL;
				TextureResource* normalMap = NULL;
				string texturePath = strip->GetTexturePath();
				if(texturePath != "")
				{
					texture = GetResourceManager()->CreateTextureResourceFromFile(texturePath.c_str(), true);
					string ending = texturePath.substr(texturePath.length()-4);
					string first = texturePath.substr(0, texturePath.length()-4);
					string normalTexturePath = first + "_n" + ending;
					normalMap = GetResourceManager()->CreateTextureResourceFromFile(normalTexturePath.c_str(), true);
				}

				Object3D* obj = new Object3D(verts, inds, texture, normalMap, mesh->GetTopology()); 
				strip->SetRenderObject(obj);
			}
		}
	}

	//Per mesh data
	TextureResource* billboardTexture = NULL;
	if(mesh->GetBillboardFilePath() != "")
	{
		billboardTexture = GetResourceManager()->CreateTextureResourceFromFile(mesh->GetBillboardFilePath().c_str(), true);
	}
	mesh->GetBillboardGFX()->SetTextureResource(billboardTexture);

	mesh->RecreateWorldMatrix(); 
	
	
	AnimatedMeshEvent* re = new AnimatedMeshEvent(true, mesh);
	this->PutEvent(re);
}


Object3D* DxManager::createParticleObject(ParticleMesh* mesh)
{
	/*BUFFER_INIT_DESC bufferDesc;
	bufferDesc.ElementSize = sizeof(ParticleVertex);
	bufferDesc.InitData = mesh->getVerts();

	
	// Last face black, should +1 this to solve it.
	bufferDesc.NumElements = mesh->getNrOfVerts();

	bufferDesc.Type = VERTEX_BUFFER;
	bufferDesc.Usage = BUFFER_DEFAULT;
	
	/*Buffer* verts = new Buffer();
	if(FAILED(verts->Init(Dx_Device, Dx_DeviceContext, bufferDesc)))
		MaloW::Debug("Initiate Buffer Failed in DxManager");
	
	BufferResource* verts = GetResourceManager()->CreateBufferResource(mesh->GetFilePath().c_str(), bufferDesc);

	BufferResource* inds = NULL;

	TextureResource* texture = NULL;
	if(mesh->GetTexturePath() != "")
	{
		texture = GetResourceManager()->CreateTextureResourceFromFile(mesh->GetTexturePath().c_str());
	}

	Object3D* obj = new Object3D(verts, inds, texture, mesh->GetTopology());
	mesh->RecreateWorldMatrix(); 
	if(mesh->GetRenderObject())
		delete mesh->GetRenderObject();
	mesh->SetRenderObject(obj);
	
	return obj;*/
	return NULL;
}

void DxManager::CreateSmokeEffect()
{
	RendererEvent* re = new RendererEvent("Create SmokeEffect");
	this->PutEvent(re);
}

void DxManager::DeleteTerrain(Terrain* terrain)
{
	TerrainEvent* re = new TerrainEvent(false, terrain);
	this->PutEvent(re);
}

void DxManager::DeleteStaticMesh(StaticMesh* mesh)
{
	StaticMeshEvent* re = new StaticMeshEvent(false, mesh);
	this->PutEvent(re);
}

void DxManager::DeleteAnimatedMesh(AnimatedMesh* mesh)
{
	AnimatedMeshEvent* re = new AnimatedMeshEvent(false, mesh);
	this->PutEvent(re);
}

void DxManager::DeleteLight(Light* light)
{
	LightEvent* re = new LightEvent(false, light, false);
	this->PutEvent(re);
}

Light* DxManager::CreateLight(D3DXVECTOR3 pos, bool UseShadowMap)
{
	Light* light = new Light(pos);

	if(UseShadowMap)
	{
		LightEvent* re = new LightEvent(true, light, true);
		this->PutEvent(re);
	}
	else
	{
		LightEvent* re = new LightEvent(true, light, false);
		this->PutEvent(re);
	}

	return light;
}

void DxManager::CreateImage(Image* image, string texture)
{
	TextureResource* tex = NULL;
	if(texture != "")
	{
		tex = GetResourceManager()->CreateTextureResourceFromFile(texture.c_str());
	}

	image->SetTexture(tex);
	
	ImageEvent* re = new ImageEvent(true, image);
	this->PutEvent(re);
}

void DxManager::DeleteImage(Image* image)
{
	ImageEvent* re = new ImageEvent(false, image);
	this->PutEvent(re);
}


void DxManager::CreateBillboard(Billboard* billboard, string texture)
{
	TextureResource* tex = NULL;
	if(texture != "")
	{
		tex = GetResourceManager()->CreateTextureResourceFromFile(texture.c_str(), true);
	}

	billboard->SetTextureResource(tex);

	BillboardEvent* re = new BillboardEvent(true, billboard);
	this->PutEvent(re);
}

void DxManager::DeleteBillboard(Billboard* billboard)
{
	BillboardEvent* re = new BillboardEvent(false, billboard);
	this->PutEvent(re);
}

void DxManager::CreateBillboardCollection(BillboardCollection* billboardCollection, string texture)
{
	TextureResource* tex = NULL;
	if(texture != "")
	{
		tex = GetResourceManager()->CreateTextureResourceFromFile(texture.c_str(), true);
	}

	billboardCollection->SetTextureResource(tex);

	BillboardCollectionEvent* re = new BillboardCollectionEvent(true, billboardCollection);
	this->PutEvent(re);
}

void DxManager::DeleteBillboardCollection(BillboardCollection* billboardCollection)
{
	BillboardCollectionEvent* re = new BillboardCollectionEvent(false, billboardCollection);
	this->PutEvent(re);
}

void DxManager::DeleteText(Text* text)
{
	TextEvent* te = new TextEvent(false, text);
	this->PutEvent(te);
}

void DxManager::CreateText(Text* text, string font)
{
	TextureResource* tex = NULL;
	if(font != "")
	{
		tex = GetResourceManager()->CreateTextureResourceFromFile((font + ".png").c_str());
	}

	Font* newFont = text->GetFont();
	newFont->textureResource = tex;

	/* Font .txt structure:
	char in int
	chartex
	charwidth

	char in int
	chartex
	charwidth

	example:
	91
	55
	10

	92
	65
	8

	*/


	font += ".txt";
	ifstream file;
	file.open(font);
	while(!file.eof())
	{
		string line = "";

		getline(file, line);
		int character = atoi(line.c_str());

		getline(file, line);
		int texpos = atoi(line.c_str());

		getline(file, line);
		int width = atoi(line.c_str());
		width = width - texpos;

		newFont->charTexCoords[character] = texpos;
		newFont->charWidth[character] = width;
		getline(file, line);
	}
	file.close();

	TextEvent* te = new TextEvent(true, text);
	this->PutEvent(te);
}

void DxManager::CreateSkyBox(string texture)
{
	if(this->skybox)
		delete this->skybox;
		
	SkyBox* sb = new SkyBox(this->camera->GetOldPos(), 10, 10);
	MeshStrip* strip = sb->GetStrip();

	// Create the desc for the buffer
	BUFFER_INIT_DESC BufferDesc;
	BufferDesc.ElementSize = sizeof(VertexNormalMap);
	BufferDesc.InitData = strip->getVerts();
	BufferDesc.NumElements = strip->getNrOfVerts();
	BufferDesc.Type = VERTEX_BUFFER;
	BufferDesc.Usage = BUFFER_DEFAULT;
	
	// Create the buffer
	BufferResource* VertexBuffer = GetResourceManager()->CreateBufferResource("SkyBoxDefaultVertex", BufferDesc);


	BUFFER_INIT_DESC indiceBufferDesc;
	indiceBufferDesc.ElementSize = sizeof(int);
	indiceBufferDesc.InitData = strip->getIndicies();
	indiceBufferDesc.NumElements = strip->getNrOfIndicies();
	indiceBufferDesc.Type = INDEX_BUFFER;
	indiceBufferDesc.Usage = BUFFER_DEFAULT;

	BufferResource* IndexBuffer = GetResourceManager()->CreateBufferResource("SkyBoxDefaultIndex", indiceBufferDesc);

	TextureResource* tex = NULL;
	if(texture != "")
	{
		tex = GetResourceManager()->CreateCubeTextureResourceFromFile(texture.c_str());
	}

	Object3D* ro = new Object3D(VertexBuffer, IndexBuffer, tex, NULL, sb->GetTopology());
	strip->SetRenderObject(ro);

	this->skybox = sb;
}

void DxManager::UseShadow(bool useShadow)
{
	if(this->params.ShadowMapSettings > 0)
	{
		this->useShadow = useShadow;
	}
}

void DxManager::SetGrassFilePath(const char* filePath)
{
	this->instancingHelper->SetGrassFilePath(filePath);
}

void DxManager::RenderGrass(bool flag)
{
	this->instancingHelper->SetRenderGrassFlag(flag);
}
bool DxManager::GetRenderGrassFlag() const
{
	return this->instancingHelper->GetRenderGrassFlag();
}

void DxManager::SetSpecialCircle(float innerRadius, float outerRadius, Vector2& targetPos)
{
	this->specialCircleParams.x = innerRadius;
	this->specialCircleParams.y = outerRadius;
	this->specialCircleParams.z = targetPos.x; //x
	this->specialCircleParams.w = targetPos.y; //z
}

void DxManager::SetSunLightProperties( Vector3 direction, Vector3 lightColor, float intensity )
{
	direction.Normalize();
	this->sun.direction = D3DXVECTOR3(direction.x, direction.y, direction.z);
	this->sun.lightColor = D3DXVECTOR3(lightColor.x, lightColor.y, lightColor.z);
	this->sun.intensity = intensity;
	this->useSun = true;
}

void DxManager::ResizeRenderer(ResizeEvent* ev)
{
	unsigned int width = ev->GetWidth();
	unsigned int height = ev->GetHeight();

	this->params.WindowWidth = width;
	this->params.WindowHeight = height;

	this->camera->RecreateProjectionMatrix();



	if(this->Dx_SwapChain)
	{
		this->Dx_DeviceContext->OMSetRenderTargets(0, 0, 0);

		// Release all outstanding references to the swap chain's buffers.
		if(this->Dx_RenderTargetView)
			this->Dx_RenderTargetView->Release();

		// Preserve the existing buffer count and format.
		// Automatically choose the width and height to match the client rect for HWNDs.
		if(FAILED(this->Dx_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
			MaloW::Debug("Failed to call ResizeRenderer on DxManager: ResizeBuffers");

	
		// Get buffer and create a render-target-view.
		ID3D11Texture2D* pBuffer;
		if(FAILED(this->Dx_SwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D), (void**) &pBuffer)))
			MaloW::Debug("Failed to call ResizeRenderer on DxManager: GetBuffer");
		

		if(FAILED(this->Dx_Device->CreateRenderTargetView(pBuffer, NULL, &this->Dx_RenderTargetView)))
			MaloW::Debug("Failed to call ResizeRenderer on DxManager: CreateRenderTargetView");
		
		pBuffer->Release();


		if(this->Dx_DepthStencilView)
			this->Dx_DepthStencilView->Release();
		if(this->Dx_DepthStencil)
			this->Dx_DepthStencil->Release();

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		if(FAILED(Dx_Device->CreateTexture2D( &descDepth, NULL, &Dx_DepthStencil )))
			MaloW::Debug("Failed to call ResizeRenderer on DxManager: CreateTexture2D");
			

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		if(FAILED(Dx_Device->CreateDepthStencilView(Dx_DepthStencil, &descDSV, &Dx_DepthStencilView)))
			MaloW::Debug("Failed to call ResizeRenderer on DxManager: CreateDepthStencilView");


		this->Dx_DeviceContext->OMSetRenderTargets(1, &this->Dx_RenderTargetView, Dx_DepthStencilView);

		// Set up the viewport.
		Dx_Viewport.Width = (FLOAT)width;
		Dx_Viewport.Height = (FLOAT)height;
		Dx_Viewport.MinDepth = 0.0f;
		Dx_Viewport.MaxDepth = 1.0f;
		Dx_Viewport.TopLeftX = 0;
		Dx_Viewport.TopLeftY = 0;
		this->Dx_DeviceContext->RSSetViewports( 1, &Dx_Viewport );


		// MRT's
		for(int i = 0; i < NrOfRenderTargets; i++)
		{
			if(this->Dx_GbufferTextures[i])
				this->Dx_GbufferTextures[i]->Release();
			if(this->Dx_GbufferRTs[i])
				this->Dx_GbufferRTs[i]->Release();
			if(this->Dx_GbufferSRVs[i])
				this->Dx_GbufferSRVs[i]->Release();
		}


		D3D11_TEXTURE2D_DESC GBufferTextureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC DescRT;
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
		for(int i = 0; i < this->NrOfRenderTargets; i++)
		{
			ZeroMemory(&GBufferTextureDesc, sizeof(GBufferTextureDesc));
			GBufferTextureDesc.Width = width;
			GBufferTextureDesc.Height = height;	
			GBufferTextureDesc.MipLevels = 1;
			GBufferTextureDesc.ArraySize = 1;
			GBufferTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			GBufferTextureDesc.SampleDesc.Count = 1;
			GBufferTextureDesc.SampleDesc.Quality = 0;
			GBufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
			GBufferTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			GBufferTextureDesc.CPUAccessFlags = 0;
			GBufferTextureDesc.MiscFlags = 0;

			if(FAILED(this->Dx_Device->CreateTexture2D(&GBufferTextureDesc, NULL, &this->Dx_GbufferTextures[i])))
				MaloW::Debug("Failed to call ResizeRenderer on DxManager: Failed to initiate GbufferTexture");


			ZeroMemory(&DescRT, sizeof(DescRT));
			DescRT.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			DescRT.Texture2DArray.ArraySize = 1;
			DescRT.Texture2DArray.MipSlice = 0;

			//if(FAILED(this->Dx_Device->CreateRenderTargetView(this->Dx_GbufferTextures[i], NULL, &this->Dx_GbufferRTs[i])))
			if(FAILED(this->Dx_Device->CreateRenderTargetView(this->Dx_GbufferTextures[i], &DescRT, &this->Dx_GbufferRTs[i])))
				MaloW::Debug("Failed to call ResizeRenderer on DxManager: Failed to initiate Gbuffer RT");


			ZeroMemory(&srDesc, sizeof(srDesc));
			srDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srDesc.Texture2D.MostDetailedMip = 0;
			srDesc.Texture2D.MipLevels = 1;

			if(FAILED(this->Dx_Device->CreateShaderResourceView(this->Dx_GbufferTextures[i], &srDesc, &this->Dx_GbufferSRVs[i])))
				MaloW::Debug("Failed to call ResizeRenderer on DxManager: Failed to initiate Gbuffer SRV");
		}
	}
}

void DxManager::ResizeEngine(unsigned int width, unsigned int height)
{
	ResizeEvent* te = new ResizeEvent("Resize", width, height);
	this->PutEvent(te);
}

void DxManager::SetSunLightDisabled()
{
	this->useSun = false;
}

int DxManager::GetRenderedMeshCount() const
{
	return this->RenderedMeshes;
}
int DxManager::GetRenderedFBXCount() const
{
	return this->renderedFBX;
}
int DxManager::GetRenderedTerrainCount() const
{
	return this->RenderedTerrains;
}
int DxManager::GetRenderedTerrainShadowCount() const
{
	return this->renderedTerrainShadows;
}
int DxManager::GetRenderedMeshShadowCount() const
{
	return this->renderedMeshShadows;
}
int DxManager::GetNrOfDrawnVerticesCount() const
{
	return this->NrOfDrawnVertices;
}
int DxManager::GetNrOfDrawCallsCount() const
{
	return this->NrOfDrawCalls;
}

void DxManager::SetCamera( Camera* cam )
{
	if(!this->camera)	// Needed for the first set of a camera.
		this->camera = cam;
	else
	{
		this->DelayGettingCamera = true;
		SetCameraEvent* ev = new SetCameraEvent("SetCam", cam);
		this->PutEvent(ev);
	}
}

Camera* DxManager::GetCamera() const
{
	while(this->DelayGettingCamera)
		Sleep(1);

	return this->camera;
}

void DxManager::CreateWaterPlane( WaterPlane* wp, string texture )
{
	TextureResource* tex = NULL;
	if(texture != "")
	{
		tex = GetResourceManager()->CreateTextureResourceFromFile(texture.c_str(), true);
	}
	wp->SetTexture(tex);

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

		MaloW::Debug("ERROR: Could not create vertex buffer. REASON: CreateWaterPlane."
			+ string("ERROR code: '") 
			+ MaloW::GetHRESULTErrorCodeString(hr));
	}

	wp->SetVertexBuffer(vertexBuffer);

	
	WaterPlaneEvent* re = new WaterPlaneEvent(true, wp);
	this->PutEvent(re);
}

void DxManager::DeleteWaterPlane( WaterPlane* wp )
{
	WaterPlaneEvent* re = new WaterPlaneEvent(false, wp);
	this->PutEvent(re);
}

void DxManager::CreateFBXMesh( FBXMesh* mesh )
{
	FBXEvent* re = new FBXEvent(true, mesh);
	this->PutEvent(re);
}

void DxManager::DeleteFBXMesh( FBXMesh* mesh )
{
	FBXEvent* re = new FBXEvent(false, mesh);
	this->PutEvent(re);
}

void DxManager::SetMaxFPS( float maxFPS )
{
	if(maxFPS == 0)
		this->RendererSleep = 0.0f;
	else
		this->RendererSleep = 1000.0f / maxFPS;
}

float DxManager::GetMaxFPS() const
{
	return 1000.0f / this->RendererSleep;
}

void DxManager::SetRendererSleep( float sleep )
{
	this->RendererSleep = sleep;
}

float DxManager::GetRendererSleep() const
{
	return this->RendererSleep;
}

void DxManager::ChangeShadowQuality( int newQual )
{
	if(newQual > 0)
	{
		ChangeShadowQualityEvent* re = new ChangeShadowQualityEvent("New Qual", newQual);
		this->PutEvent(re);
	}
	else
	{
		this->useShadow = false;
	}

}

void DxManager::ReloadShaders(int shader)
{
	ReloadShaderEvent* re = new ReloadShaderEvent("Reload Shaders", shader);
	this->PutEvent(re);
}

void DxManager::CreateDecal( Decal* decal, string texture )
{
	TextureResource* tex = NULL;
	if(texture != "")
	{
		tex = GetResourceManager()->CreateTextureResourceFromFile(texture.c_str(), true);
	}
	
	// create matrix for it.
	Vector3 pos = decal->GetPosition() - (decal->GetDirection() * 1.01f) * decal->GetSize() * 0.5f;
	float scale = decal->GetSize();
	Vector3 zAxis = decal->GetDirection().Normalize();
	Vector3 yAxis = decal->GetUp().Normalize();
	Vector3 xAxis = yAxis.GetCrossProduct(zAxis);
	D3DXMATRIX scaleMat = D3DXMATRIX(scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, 1);
	D3DXMATRIX worldMat = D3DXMATRIX(xAxis.x, xAxis.y, xAxis.z, 0, yAxis.x, yAxis.y, yAxis.z, 0, zAxis.x, zAxis.y, zAxis.z, 0,
		pos.x, pos.y, pos.z, 1);
	worldMat = scaleMat * worldMat;
	decal->SetWorldMatrix(worldMat);
	D3DXMATRIX lookAtMat = D3DXMATRIX(xAxis.x, yAxis.x, zAxis.x, 0, xAxis.y, yAxis.y, zAxis.y, 0, xAxis.z, yAxis.z, zAxis.z, 0,
		-xAxis.GetDotProduct(pos), -yAxis.GetDotProduct(pos), -zAxis.GetDotProduct(pos), 1);
	D3DXMATRIX projMat = D3DXMATRIX(2.0f / scale, 0, 0, 0, 0, 2.0f / scale, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	D3DXMATRIX viewProjMat = lookAtMat * projMat;
	decal->SetMatrix(viewProjMat);



	MeshStrip* strip = decal->GetStrip();

	// Create the desc for the buffer
	BUFFER_INIT_DESC BufferDesc;
	BufferDesc.ElementSize = sizeof(VertexNormalMap);
	BufferDesc.InitData = strip->getVerts();
	BufferDesc.NumElements = strip->getNrOfVerts();
	BufferDesc.Type = VERTEX_BUFFER;
	BufferDesc.Usage = BUFFER_DEFAULT;

	// Create the buffer
	BufferResource* VertexBuffer = GetResourceManager()->CreateBufferResource("DecalDefault", BufferDesc);


	BUFFER_INIT_DESC indiceBufferDesc;
	indiceBufferDesc.ElementSize = sizeof(int);
	indiceBufferDesc.InitData = strip->getIndicies();
	indiceBufferDesc.NumElements = strip->getNrOfIndicies();
	indiceBufferDesc.Type = INDEX_BUFFER;
	indiceBufferDesc.Usage = BUFFER_DEFAULT;

	BufferResource* IndexBuffer = GetResourceManager()->CreateBufferResource("DecalDefaultIndex", indiceBufferDesc);


	Object3D* ro = new Object3D(VertexBuffer, IndexBuffer, tex, NULL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	strip->SetRenderObject(ro);


	DecalEvent* re = new DecalEvent(true, decal);
	this->PutEvent(re);
}

void DxManager::DeleteDecal( Decal* decal )
{
	DecalEvent* re = new DecalEvent(false, decal);
	this->PutEvent(re);
}

void DxManager::SetEnclosingFog( Vector3 center, float radius, float fadeFactor )
{
	if(radius > 0.0f)
	{
		this->useEnclosingFog = true;
		this->fogCenter = center;
		this->fogRadius = radius;
		this->fogFadeFactor = fadeFactor;
	}
	else
	{
		this->useEnclosingFog = false;
	}
}

void DxManager::StartRendering()
{
	this->StartRender = true;
}

void DxManager::ResetPerfLogging()
{
#ifdef MALOWTESTPERF
	this->perf.ResetAll();
#endif
}

void DxManager::PrintPerfLogging()
{
#ifdef MALOWTESTPERF
	this->perf.GenerateReport(this->params);
#endif
}

