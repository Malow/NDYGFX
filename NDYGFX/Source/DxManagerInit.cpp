#include "DxManager.h"

char* FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel)
{
	if(featureLevel == D3D_FEATURE_LEVEL_11_0)
		return "11.0";
	if(featureLevel == D3D_FEATURE_LEVEL_10_1)
		return "10.1";
	if(featureLevel == D3D_FEATURE_LEVEL_10_0)
		return "10.0";

	return "Unknown";
}

HRESULT DxManager::Init()
{
	HRESULT hr = S_OK;;

	RECT rc;
	GetClientRect(hWnd, &rc);
	int screenWidth = rc.right - rc.left;;
	int screenHeight = rc.bottom - rc.top;

	MaloW::Debug("Initing Engine with width: " + MaloW::convertNrToString((float)screenWidth) +
		" and height: " + MaloW::convertNrToString((float)screenHeight));

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverType;

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = screenWidth;
	sd.BufferDesc.Height = screenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = this->params.RefreshRate;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&Dx_SwapChain,
			&Dx_Device,
			&initiatedFeatureLevel,
			&Dx_DeviceContext);

		if( SUCCEEDED( hr ) )
		{
			char title[256];
			sprintf_s(
				title,
				sizeof(title),
				"MaloWEngine - Direct3D 11.0 | Direct3D 11.0 device initiated with Direct3D %s feature level",
				FeatureLevelToString(initiatedFeatureLevel)
			);
			SetWindowText(hWnd, title);

			break;
		}
	}
	if( FAILED(hr) )
		return hr;
	
	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = Dx_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	hr = Dx_Device->CreateRenderTargetView( pBackBuffer, NULL, &Dx_RenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;


	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = Dx_Device->CreateTexture2D( &descDepth, NULL, &Dx_DepthStencil );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = Dx_Device->CreateDepthStencilView(Dx_DepthStencil, &descDSV, &Dx_DepthStencilView);
	if( FAILED(hr) )
		return hr;


	Dx_DeviceContext->OMSetRenderTargets(1, &Dx_RenderTargetView, Dx_DepthStencilView);

	// Setup the Dx_Viewport
	Dx_Viewport.Width = (float)screenWidth;
	Dx_Viewport.Height = (float)screenHeight;
	Dx_Viewport.MinDepth = 0.0f;
	Dx_Viewport.MaxDepth = 1.0f;
	Dx_Viewport.TopLeftX = 0;
	Dx_Viewport.TopLeftY = 0;
	Dx_DeviceContext->RSSetViewports(1, &Dx_Viewport);


	static const D3D11_INPUT_ELEMENT_DESC inputDescVertex[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescAnimatedVertex[] =
	{
		{"POSITION",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",       0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",         0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION_MORPH", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD_MORPH", 0, DXGI_FORMAT_R32G32_FLOAT,    1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL_MORPH",   0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR_MORPH",	   0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};			

	static const D3D11_INPUT_ELEMENT_DESC inputDescFBX[] =
	{
		{ "SV_POSITION",		  0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",				  0, DXGI_FORMAT_R32G32B32_FLOAT,    1,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",			  0, DXGI_FORMAT_R32G32B32_FLOAT,    2,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",			  0, DXGI_FORMAT_R32G32_FLOAT,       3,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",		  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",		  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 16,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescInvisibilityEffect[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescSkybox[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescBillBoard[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	static const D3D11_INPUT_ELEMENT_DESC inputDescBillBoardInstanced[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "DUMMY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		/*//Instance 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 1},
		{ "DUMMY", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 1},
		{ "DUMMY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 1},
		{ "DUMMY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 1}*/
	};

	
	// Forward renderer shader:
	this->Shader_ForwardRendering = new Shader();
	if(FAILED(this->Shader_ForwardRendering->Init(Dx_Device, Dx_DeviceContext, "Shaders/ForwardRendering.fx", inputDescVertex, 4)))
	{
		MaloW::Debug("Failed to open ForwardRendering.fx");
		return E_FAIL;
	}
	
	// ShadowMap Shader //**TILLMAN - använd egen desc utan normal & color - likadant för animated & ändra ordning, float3 pos**
	this->Shader_ShadowMap = new Shader();
	if(FAILED(this->Shader_ShadowMap->Init(Dx_Device, Dx_DeviceContext, "Shaders/ShadowMap.fx", inputDescVertex, 4)))
	{
		MaloW::Debug("Failed to open ShadowMap.fx");
		return E_FAIL;
	}

	// ShadowMapAnimated Shader
	this->Shader_ShadowMapAnimated = new Shader();
	if(FAILED(this->Shader_ShadowMapAnimated->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/ShadowMapAnimated.fx", inputDescAnimatedVertex, 8)))
	{
		MaloW::Debug("Failed to open ShadowMapAnimated.fx");
		return E_FAIL;
	}	

	// For images
	this->Shader_Image = new Shader();
	if(FAILED(this->Shader_Image->Init(Dx_Device, Dx_DeviceContext, "Shaders/Image.fx", NULL, 0)))
	{
		MaloW::Debug("Failed to open Image.fx");
		return E_FAIL;
	}

	// For billboards  - **TILLMAN detta används inte**
	this->Shader_Billboard = new Shader();
	if(FAILED(this->Shader_Billboard->Init(Dx_Device, Dx_DeviceContext, "Shaders/Billboard.fx", inputDescBillBoard, 2)))
	{
		MaloW::Debug("Failed to open Billboard.fx");
		return E_FAIL;
	}

	// For billboards(instanced)  - **TILLMAN detta används inte**
	/*polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 1;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 1;
	polygonLayout[2].AlignedByteOffset = 0;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[2].InstanceDataStepRate = 1*/
		
	
	
	//***INSTANCE TILLMAN**
	

	this->Shader_BillboardInstanced = new Shader();
	if(FAILED(this->Shader_BillboardInstanced->Init(Dx_Device, Dx_DeviceContext, "Shaders/BillboardInstanced.fx", inputDescBillBoardInstanced, 4)))
	{
		MaloW::Debug("Failed to open BillboardInstanced.fx");
		return E_FAIL;
	}

	//INSTANCE ** TILLMAN
	//CREATE DATA
	this->instanceCapacityBillboard = 200; //20 = testvalue(malow debug)**
	this->instanceTotalCountBillboard = 0;
	//this->instanceSRVBillboard = NULL;

	this->nrOfInstanceGroupsBillboard = 0;

	//Keep a copy in memory for culling
	this->instancesDataBillboard.resize(this->instanceCapacityBillboard);// = new Vertex[this->instanceCountBillboard];
	this->instanceSRVsBillboard.resize(this->instanceCapacityBillboard);
	/*this->instanceSRVsBillboard = new ID3D11ShaderResourceView*[this->instanceCapacityBillboard]; //**EXPAND TILLMAN//**STORLEK
	for(int i = 0; i < this->instanceCapacityBillboard; i++)
	{
		this->instanceSRVsBillboard[i] = NULL;
	}
	*/
	this->instanceGroupCount.resize(this->instanceCapacityBillboard); //**STORLEK
	this->instanceGroupStartLocation.resize(this->instanceCapacityBillboard);//**STORLEK
	this->instanceGroupSRVBillboard.resize(this->instanceCapacityBillboard);//**STORLEK


	/*int size = 1.0f;
	float colorValue = 0.0f;
	
	for(int i = 0; i < this->instanceCountBillboard; i++)
	{
		colorValue = (float)i / (float)this->instanceCountBillboard;

		this->instancesDataBillboard[i] = Vertex(	D3DXVECTOR3(-25, i * size + size / 2.0f, 0),
													D3DXVECTOR2(size, size), 
													D3DXVECTOR3(), //dummy
													D3DXVECTOR3(colorValue, colorValue, colorValue));
		
	}*/

	//CREATE BUFFER(EMPTY)
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * instancesDataBillboard.size(); //**this->instanceCapacityBillboard**^ titta upp på vector
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	hr = this->Dx_Device->CreateBuffer(&vbd, 0, &this->instanceBufferBillboard);
	if(FAILED(hr))
	{
		float test = 1.0f;
	}

	//LOAD DATA INTO BUFFER
	/*D3D11_MAPPED_SUBRESOURCE mappedData; 
	//map to access data
	this->Dx_DeviceContext->Map(this->instanceBufferBillboard, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	Vertex* dataView = reinterpret_cast<Vertex*>(mappedData.pData);
	int instanceCountTest = 0;
	for(UINT i = 0; i < this->instancesDataBillboard.size(); ++i)
	{
		dataView[instanceCountTest++] = this->instancesDataBillboard[i];
	}
	//unmap so the GPU can have access
	this->Dx_DeviceContext->Unmap(this->instanceBufferBillboard, 0);
	*/


	/* old:
	// Create the instance buffer.
	BUFFER_INIT_DESC bufferInitDesc;
	bufferInitDesc.ElementSize = sizeof(Vertex);
	bufferInitDesc.InitData = this->instancesDataBillboard;
	bufferInitDesc.NumElements = this->instanceCountBillboard;
	bufferInitDesc.Type = VERTEX_BUFFER;
	bufferInitDesc.Usage = BUFFER_DEFAULT;

	this->instanceBufferBillboard = new Buffer();
	this->instanceBufferBillboard->Init(this->Dx_Device, this->Dx_DeviceContext, bufferInitDesc);
	*/
	//delete [] this->instancesDataBillboard; keep a copy in memory for culling

	//this->instanceBufferBillboard->


	// For text  
	this->Shader_Text = new Shader();
	if(FAILED(this->Shader_Text->Init(Dx_Device, Dx_DeviceContext, "Shaders/TextRenderer.fx", NULL, 0)))
	{
		MaloW::Debug("Failed to open TextRenderer.fx");
		return E_FAIL;
	}

	
	// Deferred Rendering Geometry pass
	this->Shader_DeferredGeometry = new Shader();
	if(FAILED(this->Shader_DeferredGeometry->Init(Dx_Device, Dx_DeviceContext, "Shaders/DeferredGeometry.fx", inputDescVertex, 4)))
	{
		MaloW::Debug("Failed to open DeferredGeometry.fx");
		return E_FAIL;
	}

	// Deferred Rendering Geometry blend map pass
	this->Shader_TerrainEditor = new Shader();
	if(FAILED(this->Shader_TerrainEditor->Init(Dx_Device, Dx_DeviceContext, "Shaders/TerrainEditor.fx", inputDescVertex, 4)))
	{
		MaloW::Debug("Failed to open TerrainEditor.fx");
		return E_FAIL;
	}

	// Deferred Rendering Geo pass for animated
	this->Shader_DeferredAnimatedGeometry = new Shader();
	if(FAILED(this->Shader_DeferredAnimatedGeometry->Init(Dx_Device, Dx_DeviceContext, "Shaders/DeferredAnimatedGeometry.fx", inputDescAnimatedVertex, 8)))
	{
		MaloW::Debug("Failed to open DeferredAnimatedGeometry.fx");
		return E_FAIL;
	}

	// Deferred PerPixel pass
	this->Shader_DeferredLightning = new Shader();
	if(FAILED(this->Shader_DeferredLightning->Init(Dx_Device, Dx_DeviceContext, "Shaders/DeferredLightning.fx", NULL, 0)))
	{
		MaloW::Debug("Failed to open DeferredLightning.fx");
		return E_FAIL;
	}

	// Translucent Geometry
	this->Shader_DeferredGeoTranslucent = new Shader();
	if(FAILED(this->Shader_DeferredGeoTranslucent->Init(Dx_Device, Dx_DeviceContext, "Shaders/DeferredGeoTranslucent.fx", inputDescVertex, 4)))
	{
		MaloW::Debug("Failed to open DeferredGeoTranslucent.fx");
		return E_FAIL;
	}
	
	// Translucent PerPixel
	this->Shader_DeferredPerPixelTranslucent = new Shader();
	if(FAILED(this->Shader_DeferredPerPixelTranslucent->Init(Dx_Device, Dx_DeviceContext, "Shaders/DeferredPerPixelTranslucent.fx", NULL, 0)))
	{
		MaloW::Debug("Failed to open DeferredPerPixelTranslucent.fx");
		return E_FAIL;
	}

	this->Shader_FBX = new Shader();
	if(FAILED(this->Shader_FBX->Init(Dx_Device, Dx_DeviceContext, "Shaders/BTTFBXViewer.fx", inputDescFBX, 6)))
	{
		MaloW::Debug("Failed to open BTTFBXViewer.fx");
		return E_FAIL;
	}

	// Skybox shader
	this->Shader_Skybox = new Shader();
	if(FAILED(this->Shader_Skybox->Init(Dx_Device, Dx_DeviceContext, "Shaders/SkyBox.fx", inputDescSkybox, 1)))	// + on last if added above
	{
		MaloW::Debug("Failed to open SkyBox.fx");
		return E_FAIL;
	}

	//Invisibility effect shader
	this->Shader_InvisibilityEffect = new Shader();
	if(FAILED(this->Shader_InvisibilityEffect->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/InvisibilityEffect.fx", inputDescInvisibilityEffect, 2)))
	{
		MaloW::Debug("Failed to open InvisibilityEffect.fx");
		return E_FAIL;
	}


	/*
	// Deferred Quad pass
	static const D3D11_INPUT_ELEMENT_DESC DeferredQuadDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	this->Shader_DeferredQuad = new Shader();
	if(FAILED(this->Shader_DeferredQuad->Init(Dx_Device, Dx_DeviceContext, "Shaders/DeferredQuad.fx", DeferredQuadDesc, 4)))
	{
		MaloW::Debug("Failed to open DeferredQuad.fx");
		return E_FAIL;
	}


	// Deferred Texture pass
	static const D3D11_INPUT_ELEMENT_DESC DeferredTextureDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	this->Shader_DeferredTexture = new Shader();
	if(FAILED(this->Shader_DeferredTexture->Init(Dx_Device, Dx_DeviceContext, "Shaders/DeferredTexture.fx", DeferredTextureDesc, 4)))
	{
		MaloW::Debug("Failed to open DeferredTexture.fx");
		return E_FAIL;
	}
	*/


	/*
	D3D11_TEXTURE2D_DESC DeferredQuadTextureDesc;
	ZeroMemory(&DeferredQuadTextureDesc, sizeof(DeferredQuadTextureDesc));
	DeferredQuadTextureDesc.Width = screenWidth;
	DeferredQuadTextureDesc.Height = screenHeight;	
	DeferredQuadTextureDesc.MipLevels = 1;
	DeferredQuadTextureDesc.ArraySize = 1;
	DeferredQuadTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	DeferredQuadTextureDesc.SampleDesc.Count = 1;
	DeferredQuadTextureDesc.SampleDesc.Quality = 0;
	DeferredQuadTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	DeferredQuadTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	DeferredQuadTextureDesc.CPUAccessFlags = 0;
	DeferredQuadTextureDesc.MiscFlags = 0;
	
	if(FAILED(this->Dx_Device->CreateTexture2D(&DeferredQuadTextureDesc, NULL, &this->Dx_DeferredTexture)))
		MaloW::Debug("Failed to initiate DeferredQuadTexture");


	D3D11_RENDER_TARGET_VIEW_DESC DescQuadRT;
	ZeroMemory(&DescQuadRT, sizeof(DescQuadRT));
	DescQuadRT.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	DescQuadRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	DescQuadRT.Texture2DArray.ArraySize = 1;
	DescQuadRT.Texture2DArray.MipSlice = 0;

	if(FAILED(this->Dx_Device->CreateRenderTargetView(this->Dx_DeferredTexture, NULL, &this->Dx_DeferredQuadRT)))
		MaloW::Debug("Failed to initiate Deferred Quad RT");


	D3D11_SHADER_RESOURCE_VIEW_DESC srQuadDesc;
	ZeroMemory(&srQuadDesc, sizeof(srQuadDesc));
	srQuadDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srQuadDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srQuadDesc.Texture2D.MostDetailedMip = 0;
	srQuadDesc.Texture2D.MipLevels = 1;

	if(FAILED(this->Dx_Device->CreateShaderResourceView(this->Dx_DeferredTexture, &srQuadDesc, &this->Dx_DeferredSRV)))
		MaloW::Debug("Failed to initiate Deferred Quad SRV");
	*/


	for(int i = 0; i < this->NrOfRenderTargets; i++)
	{
		D3D11_TEXTURE2D_DESC GBufferTextureDesc;
		ZeroMemory(&GBufferTextureDesc, sizeof(GBufferTextureDesc));
		GBufferTextureDesc.Width = screenWidth;
		GBufferTextureDesc.Height = screenHeight;	
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
			MaloW::Debug("Failed to initiate GbufferTexture");


		D3D11_RENDER_TARGET_VIEW_DESC DescRT;
		ZeroMemory(&DescRT, sizeof(DescRT));
		DescRT.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		DescRT.Texture2DArray.ArraySize = 1;
		DescRT.Texture2DArray.MipSlice = 0;

		//if(FAILED(this->Dx_Device->CreateRenderTargetView(this->Dx_GbufferTextures[i], NULL, &this->Dx_GbufferRTs[i])))
		if(FAILED(this->Dx_Device->CreateRenderTargetView(this->Dx_GbufferTextures[i], &DescRT, &this->Dx_GbufferRTs[i])))
			MaloW::Debug("Failed to initiate Gbuffer RT");


		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
		ZeroMemory(&srDesc, sizeof(srDesc));
		srDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = 1;

		if(FAILED(this->Dx_Device->CreateShaderResourceView(this->Dx_GbufferTextures[i], &srDesc, &this->Dx_GbufferSRVs[i])))
			MaloW::Debug("Failed to initiate Gbuffer SRV");
	}

	//Initialize resource manager (before anything that uses it).
	ResourceManagerInit(this->Dx_Device, this->Dx_DeviceContext);

	this->ssao = new SSAO(8, 1.0f, 0.0f);
	//this->ssao->Init(this->Dx_Device, this->Dx_DeviceContext);
	this->fxaa = new FXAA();
	this->fxaa->Init(this->Dx_Device, this->Dx_DeviceContext, this->Dx_SwapChain);
	this->Shader_Fxaa = new Shader();
	this->Shader_Fxaa->Init(this->Dx_Device, this->Dx_DeviceContext, "Shaders/FXAA.fx", NULL, 0);
	if(this->params.ShadowMapSettings > 0)
	{
		this->csm = new CascadedShadowMap();
		this->csm->Init(this->Dx_Device, this->params.ShadowMapSettings);
	}

	this->invisibleGeometry = false;

	return S_OK;
}