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

	this->Dx_DeviceContext = NULL;
	this->Dx_DepthStencilView = NULL;
	this->Dx_DepthStencil = NULL;
	this->Dx_RenderTargetView = NULL;
	this->Shader_ForwardRendering = NULL;
	this->Dx_SwapChain = NULL;
	this->Dx_Device = NULL;

	this->DelayGettingCamera = false;

	this->Shader_ShadowMap = NULL;
	this->Shader_Text = NULL;
	this->Shader_ShadowMapAnimated = NULL;

	this->Shader_BillBoard = NULL;

	for(int i = 0; i < NrOfRenderTargets; i++)
	{
		this->Dx_GbufferTextures[i] = NULL;
		this->Dx_GbufferRTs[i] = NULL;
		this->Dx_GbufferSRVs[i] = NULL;
	}

	this->csm = NULL;

	this->Shader_DeferredGeometry = NULL;
	this->Shader_DeferredGeometryBlendMap = NULL;
	this->Shader_DeferredLightning = NULL;
	this->Shader_InvisibilityEffect = NULL;

	this->Shader_DeferredQuad = NULL;
	this->Shader_DeferredTexture = NULL;
	this->Shader_DeferredAnimatedGeometry = NULL;

	this->Dx_DeferredTexture = NULL;
	this->Dx_DeferredQuadRT = NULL;
	this->Dx_DeferredSRV = NULL;
	this->skybox = NULL;
	this->Shader_Skybox = NULL;

	this->ssao = NULL;
	this->fxaa = NULL;

	this->framecount = 0;
	this->TriangleCount = 0;
	this->RendererSleep = 0;
	this->LastCamUpdate = 0;

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
	if(this->camera)
		delete this->camera;

	if(this->Shader_ForwardRendering)
		delete this->Shader_ForwardRendering;

	if(this->Shader_ShadowMap)
		delete this->Shader_ShadowMap;

	if ( this->Shader_ShadowMapAnimated ) delete this->Shader_ShadowMapAnimated, this->Shader_ShadowMapAnimated=0;

	if(this->Shader_BillBoard)
		delete this->Shader_BillBoard;

	if(this->Shader_DeferredGeometry)
		delete this->Shader_DeferredGeometry;

	if(this->Shader_DeferredGeometryBlendMap)
		delete this->Shader_DeferredGeometryBlendMap;

	if(this->Shader_DeferredLightning)
		delete this->Shader_DeferredLightning;

	if ( this->Shader_InvisibilityEffect ) delete this->Shader_InvisibilityEffect, this->Shader_InvisibilityEffect=0;

	if(this->Shader_DeferredQuad)
		delete this->Shader_DeferredQuad;

	if(this->Shader_DeferredTexture)
		delete this->Shader_DeferredTexture;

	if(this->Shader_DeferredAnimatedGeometry)
		delete this->Shader_DeferredAnimatedGeometry;

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

	// Empty event queue 
	while(MaloW::ProcessEvent* ev = this->PeekEvent())
	{
		delete ev;
	}

	FreeResourceManager();
}

void DxManager::CreateTerrain(Terrain* terrain)
{
	//Create vertex buffer
	BUFFER_INIT_DESC vertexBufferDesc;
	vertexBufferDesc.ElementSize = sizeof(Vertex);
	vertexBufferDesc.InitData = terrain->GetVerticesPointer(); 
	vertexBufferDesc.NumElements = terrain->GetNrOfVertices();
	vertexBufferDesc.Type = VERTEX_BUFFER;
	vertexBufferDesc.Usage = BUFFER_DEFAULT; //BUFFER_CPU_WRITE***

	Buffer* vertexBuffer = new Buffer();
	if(FAILED(vertexBuffer->Init(this->Dx_Device, this->Dx_DeviceContext, vertexBufferDesc)))
		MaloW::Debug("ERROR: Could not create vertex buffer. REASON: CreateTerrain(Terrain* terrain)");
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
		if(FAILED(indexBuffer->Init(this->Dx_Device, this->Dx_DeviceContext, indexBufferDesc)))
			MaloW::Debug("ERROR: Could not create index buffer. REASON: CreateTerrain(Terrain* terrain)");
		terrain->SetIndexBuffer(indexBuffer);
	}

	//Texture(s) are not set here since they are set afterwards.

	//Create & put this event
	TerrainEvent* re = new TerrainEvent("Add Terrain", terrain);
	this->PutEvent(re);
}

void DxManager::CreateStaticMesh(StaticMesh* mesh)
{
	MaloW::Array<MeshStrip*>* strips = mesh->GetStrips();

	for(int i = 0; i < strips->size(); i++)
	{
		MeshStrip* strip = strips->get(i);

		BUFFER_INIT_DESC bufferDesc;
		bufferDesc.ElementSize = sizeof(Vertex);
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
		if(strip->GetTexturePath() != "")
		{
			texture = GetResourceManager()->CreateTextureResourceFromFile(strip->GetTexturePath().c_str());
		}

		Object3D* obj = new Object3D(verts, inds, texture, mesh->GetTopology()); 
		strip->SetRenderObject(obj);
	}

	mesh->RecreateWorldMatrix(); 
	
	MeshEvent* re = new MeshEvent("Add Mesh", mesh, NULL);
	this->PutEvent(re);
}


void DxManager::CreateAnimatedMesh(AnimatedMesh* mesh)
{
	MaloW::Array<KeyFrame*>* kfs = mesh->GetKeyFrames();
	
	for(int j = 0; j < kfs->size(); j++)
	{
		MaloW::Array<MeshStrip*>* strips = kfs->get(j)->strips;

		for(int i = 0; i < strips->size(); i++)
		{
			MeshStrip* strip = strips->get(i);

			BUFFER_INIT_DESC bufferDesc;
			bufferDesc.ElementSize = sizeof(Vertex);
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
			if(strip->GetTexturePath() != "")
			{
				texture = GetResourceManager()->CreateTextureResourceFromFile(strip->GetTexturePath().c_str());
			}
			else
			{
				texture = NULL; 
			}

			Object3D* obj = new Object3D(verts, inds, texture, mesh->GetTopology()); 
			strip->SetRenderObject(obj);
		}
	}

	mesh->RecreateWorldMatrix(); 
	
	
	MeshEvent* re = new MeshEvent("Add AniMesh", NULL, mesh);
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
	//**TILLMAN resource manager
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
	TerrainEvent* re = new TerrainEvent("Delete Terrain", terrain);
	this->PutEvent(re);
}

void DxManager::DeleteStaticMesh(StaticMesh* mesh)
{
	MeshEvent* re = new MeshEvent("Delete Mesh", mesh, NULL);
	this->PutEvent(re);
}

void DxManager::DeleteAnimatedMesh(AnimatedMesh* mesh)
{
	MeshEvent* re = new MeshEvent("Delete AniMesh", NULL, mesh);
	this->PutEvent(re);
}

void DxManager::DeleteLight(Light* light)
{
	LightEvent* re = new LightEvent("Delete Light", light);
	this->PutEvent(re);
}

Light* DxManager::CreateLight(D3DXVECTOR3 pos, bool UseShadowMap)
{
	Light* light = new Light(pos);

	if(UseShadowMap)
	{
		LightEvent* re = new LightEvent("Add Light with shadows", light);
		this->PutEvent(re);
	}
	else
	{
		LightEvent* re = new LightEvent("Add Light", light);
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
	
	ImageEvent* re = new ImageEvent("Add Image", image);
	this->PutEvent(re);
}

void DxManager::DeleteImage(Image* image)
{
	ImageEvent* re = new ImageEvent("Delete Image", image);
	this->PutEvent(re);
}

void DxManager::DeleteText(Text* text)
{
	TextEvent* te = new TextEvent("Delete Text", text);
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

	TextEvent* te = new TextEvent("Add Text", text);
	this->PutEvent(te);
}

void DxManager::CreateSkyBox(string texture)
{
	if(this->skybox)
		delete this->skybox;
		
	SkyBox* sb = new SkyBox(this->camera->GetPositionD3DX(), 10, 10);
	MeshStrip* strip = sb->GetStrips()->get(0);

	// Create the desc for the buffer
	BUFFER_INIT_DESC BufferDesc;
	BufferDesc.ElementSize = sizeof(Vertex);
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

	Object3D* ro = new Object3D(VertexBuffer, IndexBuffer, tex, sb->GetTopology());
	strip->SetRenderObject(ro);

	this->skybox = sb;
}

void DxManager::SetSpecialCircle(float innerRadius, float outerRadius, Vector2& targetPos)
{
	this->specialCircleParams.x = innerRadius;
	this->specialCircleParams.y = outerRadius;
	this->specialCircleParams.z = targetPos.x; //x
	this->specialCircleParams.w = targetPos.y; //z
}

void DxManager::SetFPSMAX( float maxFPS )
{
	if(maxFPS == 0)
		this->RendererSleep = 0.0f;
	else
		this->RendererSleep = 1000.0f / maxFPS;
}

void DxManager::SetSunLightProperties( Vector3 direction, Vector3 lightColor, float intensity )
{
	this->sun.direction = direction;
	this->sun.lightColor = lightColor;
	this->sun.intensity = intensity;
	this->useSun = true;
}

void DxManager::ResizeRenderer(ResizeEvent* ev)
{
	unsigned int width = ev->GetWidth();
	unsigned int height = ev->GetHeight();

	this->params.windowWidth = width;
	this->params.windowHeight = height;

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

		for(int i = 0; i < this->NrOfRenderTargets; i++)
		{
			D3D11_TEXTURE2D_DESC GBufferTextureDesc;
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


			D3D11_RENDER_TARGET_VIEW_DESC DescRT;
			ZeroMemory(&DescRT, sizeof(DescRT));
			DescRT.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			DescRT.Texture2DArray.ArraySize = 1;
			DescRT.Texture2DArray.MipSlice = 0;

			//if(FAILED(this->Dx_Device->CreateRenderTargetView(this->Dx_GbufferTextures[i], NULL, &this->Dx_GbufferRTs[i])))
			if(FAILED(this->Dx_Device->CreateRenderTargetView(this->Dx_GbufferTextures[i], &DescRT, &this->Dx_GbufferRTs[i])))
				MaloW::Debug("Failed to call ResizeRenderer on DxManager: Failed to initiate Gbuffer RT");


			D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
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

int DxManager::GetRenderedMeshCount()
{
	int nrOfRendered = 0;
	for(int i = 0; i < this->objects.size(); i++)
	{
		StaticMesh* ms = this->objects.get(i);
		MaloW::Array<MeshStrip*>* strips = ms->GetStrips();
		for(int u = 0; u < strips->size(); u++)
		{
			MeshStrip* s = strips->get(u);
			if(!s->GetCulled())
			{
				nrOfRendered++;
				u = strips->size();
			}
		}
	}
	return nrOfRendered;
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

