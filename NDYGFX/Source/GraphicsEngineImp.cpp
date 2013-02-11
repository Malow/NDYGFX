#include "GraphicsEngineImp.h"

bool CursorControl::visable = true;

int GraphicsEngineParams::WindowWidth = 1024;
int GraphicsEngineParams::WindowHeight = 768;
bool GraphicsEngineParams::Maximized = false;
int GraphicsEngineParams::ShadowMapSettings = 0;
int GraphicsEngineParams::FXAAQuality = 0;
CameraType GraphicsEngineParams::CamType = FPS;
float GraphicsEngineParams::FOV = 75.0f;
float GraphicsEngineParams::NearClip = 0.01f;
float GraphicsEngineParams::FarClip = 200.0f;
int GraphicsEngineParams::RefreshRate = 60;
int GraphicsEngineParams::MaxFPS = 0;
int GraphicsEngineParams::ShadowFit = 50;
float GraphicsEngineParams::BillboardRange = 0.75f;

GraphicsEngineImp::GraphicsEngineImp(const GraphicsEngineParams &params, HINSTANCE hInstance, int nCmdShow) :
	parameters(params)
{
	this->cam = NULL;
	this->dx = NULL;
	this->hInstance = NULL;
	this->hWnd = NULL;
	this->fbx = NULL;

	this->keepRunning = true;
	this->loading = false;

	LARGE_INTEGER li;
	if(!QueryPerformanceFrequency(&li))
		MaloW::Debug("QueryPerformanceFrequency Failed!, High resolution performance counter not available?");

	this->PCFreq = float(li.QuadPart)/1000.0f;
	QueryPerformanceCounter(&li);
	this->prevTimeStamp = li.QuadPart;

	this->prevFrameCount = 0;
	this->fpsLast = 0;
	this->fpsTimer = 0.0f;
	this->isManagingMyOwnWindow = true;

	this->kl = new KeyListener(this->hWnd);
	this->InitWindow(hInstance, nCmdShow);
	kl->SetHWND(this->hWnd); // Because of key listener being created before the window

	this->Start();
}

GraphicsEngineImp::GraphicsEngineImp(const GraphicsEngineParams &params, HWND hWnd) :
	parameters(params)
{
	this->cam = NULL;
	this->dx = NULL;
	this->hInstance = NULL;
	this->hWnd = NULL;

	this->keepRunning = true;
	this->loading = false;

	LARGE_INTEGER li;
	if(!QueryPerformanceFrequency(&li))
		MaloW::Debug("QueryPerformanceFrequency Failed!, High resolution performance counter not available?");

	this->PCFreq = float(li.QuadPart)/1000.0f;
	QueryPerformanceCounter(&li);
	this->prevTimeStamp = li.QuadPart;


	this->prevFrameCount = 0;
	this->fpsLast = 0;
	this->fpsTimer = 0.0f;
	this->isManagingMyOwnWindow = false;

	this->hWnd = hWnd;
	this->kl = new KeyListener(this->hWnd);
	this->InitObjects();

	this->Start();
}

GraphicsEngineImp::~GraphicsEngineImp()
{
	// Close self thread.
	this->Close();
	this->WaitUntillDone();

	// Close DxManager thread.
	this->dx->Close();
	this->dx->WaitUntillDone();
	
	if ( this->dx ) delete dx, dx=0;
	if ( this->kl ) delete kl, kl=0;
	if(this->physx)
	{
		delete this->physx;
		this->physx = NULL;
	}
	DestroyBTHFbx();

	// DestroyWindow(this->hWnd); // Why is this commented out, Alex
}

LRESULT CALLBACK GraphicsEngineImp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	GraphicsEngineImp* gfx = NULL;

	if(message == WM_CREATE)
	{
		gfx = (GraphicsEngineImp*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)gfx);
	}
	else
	{
		gfx = (GraphicsEngineImp*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	switch (message) 
	{
		case WM_KEYDOWN:
			if (gfx) gfx->GetKeyList()->KeyDown(wParam);
			break;
		
		case WM_KEYUP:
			if (gfx) gfx->GetKeyList()->KeyUp(wParam);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		// Left Mouse Pressed
		case WM_LBUTTONDOWN:
			if (gfx) gfx->GetKeyList()->MouseDown(1);
			break;

		case WM_LBUTTONUP:
			if (gfx) gfx->GetKeyList()->MouseUp(1);
			break;

		// Right Mouse Pressed
		case WM_RBUTTONDOWN:
			if (gfx) gfx->GetKeyList()->MouseDown(2);
			break;

		case WM_RBUTTONUP:
			if (gfx) gfx->GetKeyList()->MouseUp(2);
			break;

		// TODO: Handle File Drops
		case WM_DROPFILES:
			{
			HDROP drop = (HDROP)wParam;
			int nrOfFiles = DragQueryFile(drop, 0xFFFFFFFF, NULL, NULL);
			if(nrOfFiles != 1)
				MaloW::Debug("Multiple files not supported, you tried to drop " + MaloW::convertNrToString((float)nrOfFiles) + " files.");

			TCHAR lpszFile[MAX_PATH] = {0};
			lpszFile[0] = '\0';
			if(DragQueryFile(drop, 0, lpszFile, MAX_PATH))
			{
				if(gfx)
					gfx->specialString = string(lpszFile);
			}
			else
				MaloW::Debug("Failed to load a droppped file.");
			}
			break;

		// TODO: Handle Resize
		case WM_SIZE:
			{
				/*
				RECT rc;
				GetClientRect(hWnd, &rc);
				int screenWidth = rc.right - rc.left;;
				int screenHeight = rc.bottom - rc.top;
				if(gfx)
				{
					bool manage = gfx->GetManagingWindow();
					gfx->SetManagingWindow(false);
					gfx->ResizeGraphicsEngine(screenWidth, screenHeight);
					gfx->SetManagingWindow(manage);
				}
					*/
				if ( wParam == SIZE_MAXHIDE )
				{

				}
				else if ( wParam == SIZE_MAXIMIZED )
				{

				}
				else if ( wParam == SIZE_MAXSHOW )
				{

				}
				else if ( wParam == SIZE_MINIMIZED )
				{

				}
				else if ( wParam == SIZE_RESTORED )
				{

				}
			}
			break;

			/*
		case WM_MOVING:
			break;
		case WM_ENTERSIZEMOVE:
			break;
		case WM_EXITSIZEMOVE:
			break;
			*/
				
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

HRESULT GraphicsEngineImp::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	this->hInstance = hInstance;

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = 0;//CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = this->WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = this->hInstance;
	wcex.hIcon          = LoadIcon(hInstance, "GameIcon.ico");
	//wcex.hCursor        = LoadCursor(hInstance, "cursor1.cur");
	//wcex.hCursor		= (HCURSOR)LoadImage(hInstance, "cursor1.cur", IMAGE_ICON, 32,32,LR_LOADFROMFILE);
	wcex.hCursor      = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = "GraphicsEngine";
	wcex.hIconSm        = (HICON)LoadImage(hInstance, "GameIcon.ico", IMAGE_ICON, 32,32,LR_LOADFROMFILE);
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	RECT rc = { 0, 0, this->parameters.WindowWidth, this->parameters.WindowHeight };
	if(this->parameters.Maximized)
	{
		AdjustWindowRectEx(&rc, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
		this->hWnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, "GraphicsEngine", 
			"GraphicsEngine - Direct3D 11.0", WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE, 
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, this->hInstance, this);
	}
	else
	{
		AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
		this->hWnd = CreateWindow("GraphicsEngine", "GraphicsEngine - Direct3D 11.0", 
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, 
			NULL, NULL, this->hInstance, this);
	}
	if(!this->hWnd)
		return E_FAIL;

	ShowWindow(this->hWnd, nCmdShow);
	MoveWindow(this->hWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, false);
	DragAcceptFiles(hWnd,true);

	this->InitObjects();

	return S_OK;
}

void GraphicsEngineImp::InitObjects()
{
	this->dx = new DxManager(this->hWnd, this->parameters, this->cam);
	this->physx = new PhysicsEngine();

	if(this->parameters.CamType == FPS)
	{
		this->cam = new FPSCamera(this->hWnd, this->parameters);
	}
	else if(this->parameters.CamType == RTS)
	{
		this->cam = new RTSCamera(this->hWnd, this->parameters);
	}
	/*
	else if(this->parameters.CamType == TRD)
	{
		this->cam = new TRDCamera(this->hWnd, this->parameters);
	}
	*/
	this->dx->SetCamera(this->cam);
	this->dx->SetMaxFPS(this->parameters.MaxFPS);
	this->dx->Start();
	this->fbx = InitBTHFbx();
}

StaticMesh* GraphicsEngineImp::CreateStaticMesh(string filename, D3DXVECTOR3 pos, const char* billboardFilePath, float distanceToSwapToBillboard)
{
	StaticMesh* mesh = new StaticMesh(pos, billboardFilePath, distanceToSwapToBillboard);
	LoadMeshEvent* re = new LoadMeshEvent(filename, mesh, NULL, NULL);
	this->PutEvent(re);

	return mesh;
}

StaticMesh* GraphicsEngineImp::CreateStaticMesh(string filename, D3DXVECTOR3 pos, MaterialType material)
{
	Material* mat = new Material(material);
	return this->CreateStaticMesh(filename, pos, mat);
}

StaticMesh* GraphicsEngineImp::CreateStaticMesh(string filename, D3DXVECTOR3 pos, Material* material)
{
	StaticMesh* mesh = new StaticMesh(pos);

	LoadMeshEvent* re = new LoadMeshEvent(filename, mesh, NULL, material);
	this->PutEvent(re);

	return mesh;
}

iMesh* GraphicsEngineImp::CreateStaticMesh( const char* filename, const Vector3& pos )
{
	return CreateStaticMesh( std::string(filename), D3DXVECTOR3(pos.x,pos.y,pos.z) );
}

AnimatedMesh* GraphicsEngineImp::CreateAnimatedMesh(string filename, D3DXVECTOR3 pos, const char* billboardFilePath, float distanceToSwapToBillboard)
{
	AnimatedMesh* mesh = new AnimatedMesh(pos, billboardFilePath, distanceToSwapToBillboard);

	LoadMeshEvent* re = new LoadMeshEvent(filename, NULL, mesh, NULL);
	this->PutEvent(re);

	return mesh;
}

iAnimatedMesh* GraphicsEngineImp::CreateAnimatedMesh( const char* filename, const Vector3& pos )
{
	return CreateAnimatedMesh( std::string(filename), D3DXVECTOR3(pos.x,pos.y,pos.z) );
}

Light* GraphicsEngineImp::CreateLight(D3DXVECTOR3 pos, bool UseShadowMap)
{
	return this->dx->CreateLight(pos, UseShadowMap);
}

iTerrain* GraphicsEngineImp::CreateTerrain(const Vector3& pos, const Vector3& scale, const unsigned int& size)
{
	Terrain* terrain = new Terrain(D3DXVECTOR3(pos.x, pos.y, pos.z), D3DXVECTOR3(scale.x, scale.y, scale.z), size);
	this->dx->CreateTerrain(terrain);

	return terrain;
}


Image* GraphicsEngineImp::CreateImage(D3DXVECTOR2 position, D3DXVECTOR2 dimensions, string texture)
{
	Image* img = new Image(position, dimensions);
	this->dx->CreateImage(img, texture);
	return img;
}

iImage* GraphicsEngineImp::CreateImage( Vector2 pos, Vector2 dimensions, const char* texture )
{
	return this->CreateImage(D3DXVECTOR2(pos.x, pos.y), D3DXVECTOR2(dimensions.x, dimensions.y), string(texture));
}

bool GraphicsEngineImp::DeleteImage(Image* delImage)
{
	this->dx->DeleteImage(delImage);
	return true;
}

void GraphicsEngineImp::DeleteImage( iImage* &delImg )
{
	this->DeleteImage(dynamic_cast<Image*>(delImg));
	delImg = NULL;
}

Billboard* GraphicsEngineImp::CreateBillboard(D3DXVECTOR3 position, D3DXVECTOR2 size, string texture)
{
	Billboard* billboard = new Billboard(position, size);
	this->dx->CreateBillboard(billboard, texture);
	return billboard;
}

iBillboard* GraphicsEngineImp::CreateBillboard( Vector3 pos, Vector2 size, const char* texture )
{
	return this->CreateBillboard(D3DXVECTOR3(pos.x, pos.y, pos.z), D3DXVECTOR2(size.x, size.y), string(texture));
}

bool GraphicsEngineImp::DeleteBillboard(Billboard* delBillboard)
{
	this->dx->DeleteBillboard(delBillboard);
	return true;
}

void GraphicsEngineImp::DeleteBillboard( iBillboard* &delbillboard )
{
	this->DeleteBillboard(dynamic_cast<Billboard*>(delbillboard));
	delbillboard = NULL;
}

Text* GraphicsEngineImp::CreateText(string text, D3DXVECTOR2 position, float size, string fontTexturePath)
{
	Text* textobj = new Text(text, position, size);
	this->dx->CreateText(textobj, fontTexturePath);
	return textobj;
}

iText* GraphicsEngineImp::CreateText( const char* text, Vector2 pos, float size, const char* fontTexturePath )
{
	return this->CreateText(string(text), D3DXVECTOR2(pos.x, pos.y), size, string(fontTexturePath));
}

bool GraphicsEngineImp::DeleteText(Text* delText)
{
	this->dx->DeleteText(delText);
	return true;
}

void GraphicsEngineImp::DeleteText( iText* &deltxt )
{
	this->DeleteText(dynamic_cast<Text*>(deltxt));
	deltxt = NULL;
}

float GraphicsEngineImp::Update()
{
	MSG msg = {0};
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))		// changed from if to while, cuz I wanna clear the msg log because inputs are more important than updates.
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if(msg.message == WM_QUIT)
			this->keepRunning = false;
	}
	
	// Timer
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	float diff = (li.QuadPart - prevTimeStamp) / this->PCFreq;
	this->prevTimeStamp = li.QuadPart;

	this->fpsTimer += diff;
	if(this->fpsTimer > 1000.0f)
	{
		int framecount = this->dx->GetFrameCount();
		this->fpsLast = framecount - prevFrameCount;
		this->prevFrameCount = framecount;
		this->fpsTimer = 0;
	}
	
	this->dx->Update(diff);
	this->fbx->UpdateScenes(diff * 0.001f, true);

	if(this->isManagingMyOwnWindow)
	{
		std::string txt = "FPS: " + MaloW::convertNrToString((float)this->fpsLast) + " - "; 
		txt += "Camera Pos: " + MaloW::convertNrToString(this->cam->GetPosition().x) + " " + 
		MaloW::convertNrToString(this->cam->GetPosition().y) + " " + 
		MaloW::convertNrToString(this->cam->GetPosition().z) + "  -  Mesh Count: " + 
		MaloW::convertNrToString(this->dx->GetMeshCount()) + "  - Ren. Meshes: " + 
		MaloW::convertNrToString(this->dx->GetRenderedMeshCount()) + "  -  Ter. Count: " +
		MaloW::convertNrToString(this->dx->GetTerrainCount()) + "  -  Ren. Ters: " +
		MaloW::convertNrToString(this->dx->GetRenderedTerrainCount()) + "  -  Mesh shad: " +
		MaloW::convertNrToString(this->dx->GetRenderedMeshShadowCount() / 4) + "  -  Terr shad: " +
		MaloW::convertNrToString(this->dx->GetRenderedTerrainShadowCount() / 4); //** / 4 = temp**

		

		SetWindowText(this->hWnd, txt.c_str());
	}

	return diff;	// Return in seconds
}

bool GraphicsEngineImp::IsRunning()
{
	return this->keepRunning;
}

void GraphicsEngineImp::Life()
{
	while(this->stayAlive)
	{
		if(MaloW::ProcessEvent* ev = this->WaitEvent())
		{
			this->loading = true;
			if( LoadMeshEvent* LME = dynamic_cast<LoadMeshEvent*>(ev) )
			{
				string filename = LME->GetFileName();
				if(StaticMesh* mesh = LME->GetStaticMesh())
				{
					bool success = mesh->LoadFromFile(filename);
					if(success)
					{
						this->dx->CreateStaticMesh(mesh);
					}

					if(Material* material = LME->GetMaterial())
					{
						MaloW::Array<MeshStrip*>* strips = mesh->GetStrips();
						for(int i = 0; i < strips->size(); i++)
						{
							strips->get(i)->SetMaterial(material);
							if(i+1 < strips->size())
								material = new Material(material);
						}
					}
				}
				else if(AnimatedMesh* mesh = LME->GetAnimatedMesh())
				{
					//Check if loading the animation from file was successful...
					if(mesh->LoadFromFile(filename))
					{
						//...and if so, add it
						this->dx->CreateAnimatedMesh(mesh); 
					}
					else
					{
						//If not, delete the mesh.
						MaloW::Debug("Warning: Deleting animated mesh because of failure.");
						delete mesh;
					}
				}
				else if(FBXMesh* mesh = LME->GetFBXMesh())
				{
					bool success = mesh->LoadFromFile(filename, this->fbx, this->dx->GetDevice(), this->dx->GetContextDevice());
					if(success)
					{
						this->dx->CreateFBXMesh(mesh);
					}
				}
			}
			else if(PreLoadEvent* ple = dynamic_cast<PreLoadEvent*>(ev))
			{
				GetResourceManager()->PreLoadResources(ple->GetNrOfResources(), ple->GetResourceFileNames());
			}
			delete ev;

			if(this->GetEventQueueSize() == 0)
				this->loading = false;
		}
	}
}

void GraphicsEngineImp::CreateSkyBox(const char* texture)
{
	this->dx->CreateSkyBox(string(texture));
}

void GraphicsEngineImp::UseShadow(bool useShadow)
{
	this->dx->UseShadow(useShadow);
}

void GraphicsEngineImp::SetSpecialCircle(float innerRadius, float outerRadius, Vector2& targetPos) const
{
	this->dx->SetSpecialCircle(innerRadius, outerRadius, targetPos);
}

void GraphicsEngineImp::PreLoadResources(unsigned int nrOfResources, const char** resourcesFileNames)
{
	GetResourceManager()->PreLoadResources(nrOfResources, resourcesFileNames);
	//PreLoadEvent* re = new PreLoadEvent(nrOfResources, resourcesFileNames);
	//this->PutEvent(re);
}

void GraphicsEngineImp::LoadingScreen(const char* BackgroundTexture, const char* ProgressBarTexture, float FadeBlackInInTime, float FadeBlackInOutTime, float FadeBlackOutInTime, float FadeBlackOutOutTime)
{
	bool updateCam = this->cam->GetUpdatingCamera();
	this->cam->SetUpdateCamera(false);
	this->Update();

	// Set MaxFPS during loading screen
	float prevRendSleep = this->dx->GetRendererSleep();
	this->dx->SetMaxFPS(30.0f);

	Image* bg = NULL;
	if( strcmp(BackgroundTexture, "") != 0 )
		bg = this->CreateImage(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), BackgroundTexture);

	Image* pb = NULL;
	if( strcmp(ProgressBarTexture, "") != 0)
		pb = this->CreateImage(D3DXVECTOR2((this->parameters.WindowWidth / 4.0f), ((this->parameters.WindowHeight * 3.0f) / 4.0f)), 
			D3DXVECTOR2(0, this->parameters.WindowHeight / 10.0f), ProgressBarTexture);

	int TotalItems = this->GetEventQueueSize();

	float dx = (this->parameters.WindowWidth / 2.0f) / TotalItems;
	float y = this->parameters.WindowHeight / 10.0f;

	Image* fade = NULL;
	if(FadeBlackInInTime != 0.0f || FadeBlackInOutTime != 0.0f || FadeBlackOutInTime != 0.0f || FadeBlackOutOutTime != 0.0f)
		fade = this->CreateImage(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->parameters.WindowWidth, (float)this->parameters.WindowHeight), "Media/LoadingScreen/FadeTexture.png");

	int state = 0;
	/*
	0 = fade in to black
	1 = fade out to loading screen
	2 = loading 
	3 = fade in to black
	4 = fade out to game
	*/
	this->StartRendering();
	float timer = 0.0f;
	bool go = true;
	while(go)
	{
		float diff = this->Update();
		timer += diff * 0.001f;
		
		
		if(state == 0)
		{
			if(FadeBlackInInTime > 0.0f)
			{
				float op = timer / FadeBlackInInTime;
				if(op > 1.0f)
					op = 1.0f;
				if(fade)
					fade->SetOpacity(op);
			}
			if(timer > FadeBlackInInTime)
			{
				state++;
				timer = 0;

				// Added by Alexivan
				if ( bg ) bg->SetDimensions(Vector2((float)this->parameters.WindowWidth, (float)this->parameters.WindowHeight));
			}
		}
		else if(state == 1)
		{
			if(FadeBlackInOutTime > 0.0f)
			{
				float op = 1 - (timer / FadeBlackInOutTime);
				if(op < 0.0f)
					op = 0.0f;
				if(fade)
					fade->SetOpacity(op);
			}

			if(timer > FadeBlackInOutTime)
				state++;
		}
		else if(state == 2)
		{
			if(!this->loading)
			{
				timer = 0;
				state++;
			}
		}
		else if(state == 3)
		{
			if(FadeBlackOutInTime > 0.0f)
			{
				float op = timer / FadeBlackOutInTime;
				if(op > 1.0f)
					op = 1.0f;
				if(fade)
					fade->SetOpacity(op);
			}

			if(timer > FadeBlackOutInTime)
			{
				state++;
				timer = 0;

				if(pb)
					this->DeleteImage(pb);
				if(bg)
					this->DeleteImage(bg);
			}

		}
		else
		{
			if(FadeBlackOutOutTime > 0.0f)
			{
				float op = 1 - (timer / FadeBlackOutOutTime);
				if(op < 0.0f)
					op = 0.0f;
				if(fade)
					fade->SetOpacity(op);
			}

			if(timer > FadeBlackOutOutTime)
			{
				state++;
				go = false;
			}
		}


		int ItemsToGo = this->GetEventQueueSize();
		
		if(this->loading && state > 0)
			if(pb)
			{
				if(ItemsToGo == 0)
					pb->SetDimensions(Vector2(dx * (TotalItems - 0.5f), y));
				else if(ItemsToGo < TotalItems - 1)
					pb->SetDimensions(Vector2(dx * (TotalItems - ItemsToGo - 1), y));
				else
					pb->SetDimensions(Vector2(dx * 0.5f, y));
			}

		// Sleep for a bit
		Sleep(15);
	}

	if(fade)
	{
		this->DeleteImage(fade);
	}

	if(this->cam->GetCameraType() == FPS)
		this->GetKeyList()->SetMousePosition(Vector2(this->parameters.WindowWidth / 2.0f, this->parameters.WindowHeight / 2.0f));
	this->cam->SetUpdateCamera(updateCam);
	this->dx->SetRendererSleep(prevRendSleep);
}

iMesh* GraphicsEngineImp::CreateMesh( const char* filename, const Vector3& pos, const char* billboardFilePath, float distanceToSwapToBillboard)
{
	string tmp = string(filename); //".fileType"
	if(tmp.substr(tmp.length() - 4) == ".obj") 
	{
		return CreateStaticMesh(filename, D3DXVECTOR3(pos.x,pos.y,pos.z), billboardFilePath, distanceToSwapToBillboard);
	}
	else if(tmp.substr(tmp.length() - 4) == ".ani")
	{
		return CreateAnimatedMesh(filename, D3DXVECTOR3(pos.x,pos.y,pos.z), billboardFilePath, distanceToSwapToBillboard);
	}
	else if (tmp.substr(tmp.length() - 4) == ".fbx")
	{
		return this->CreateFBXMesh(filename, pos);
	}
	else
	{
		MaloW::Debug("WARNING: Failed to create mesh! (Returning NULL). ");
		return NULL;
	}
}

iLight* GraphicsEngineImp::CreateLight( Vector3 pos )
{
	return this->CreateLight(D3DXVECTOR3(pos.x, pos.y, pos.z));
}

void GraphicsEngineImp::DeleteLight( iLight* &light )
{
	this->DeleteLight(dynamic_cast<Light*>(light));

	light = NULL;
}

iCamera* GraphicsEngineImp::GetCamera() const
{
	return this->GetCam();
}

iKeyListener* GraphicsEngineImp::GetKeyListener() const
{
	return this->GetKeyList();
}

iGraphicsEngineParams* GraphicsEngineImp::GetEngineParameters() const
{
	return &this->GetEngineParams();
}

void GraphicsEngineImp::DeleteMesh( iMesh* delMesh )
{
	Mesh* tmpMesh = dynamic_cast<Mesh*>(delMesh);
	if( tmpMesh != NULL )
	{
		string fileName = tmpMesh->GetFilePath();
		if ( fileName.length() > 4 )
		{
			if(fileName.substr(fileName.length() - 4) == ".obj") 
			{
				this->DeleteStaticMesh(dynamic_cast<StaticMesh*>(delMesh));
			}
			else if(fileName.substr(fileName.length() - 4) == ".ani")
			{
				this->DeleteAnimatedMesh(dynamic_cast<AnimatedMesh*>(delMesh));
			}
			else if(fileName.substr(fileName.length() - 4) == ".fbx")
			{
				this->DeleteFBXMesh(dynamic_cast<FBXMesh*>(delMesh));
			}
			delMesh = NULL;
		}
	}
	if(WaterPlane* wp = dynamic_cast<WaterPlane*>(tmpMesh))
		this->DeleteWaterPlane(wp);
}

const char* GraphicsEngineImp::GetSpecialString()
{
	return this->specialString.c_str();
}

void GraphicsEngineImp::SetSunLightProperties( Vector3 direction, Vector3 lightColor /*= Vector3(1.0f, 1.0f, 1.0f)*/, float intensity /*= 1.0f*/ )
{
	this->dx->SetSunLightProperties(direction, lightColor, intensity);
}

iPhysicsEngine* GraphicsEngineImp::GetPhysicsEngine() const
{
	return this->physx;
}

iCamera* GraphicsEngineImp::ChangeCamera( CameraType newCamType )
{
	Camera* oldcam = this->cam;
	if(newCamType == FPS)
	{
		this->cam = new FPSCamera(this->hWnd, this->parameters);
	}
	else if(newCamType == RTS)
	{
		this->cam = new RTSCamera(this->hWnd, this->parameters);
	}
	this->cam->SetPosition(oldcam->GetPosition());
	this->cam->SetActiveWindowDisabling(oldcam->GetActiveWindowDisabling());
	this->cam->SetUpdateCamera(oldcam->GetUpdatingCamera());
	this->dx->SetCamera(this->cam);
	this->parameters.CamType = newCamType;
	return this->cam;	
}

void GraphicsEngineImp::SetSceneAmbientLight( Vector3 ambientLight )
{
	this->dx->SetSceneAmbientLight(D3DXVECTOR3(ambientLight.x, ambientLight.y, ambientLight.z));
}

void GraphicsEngineImp::ResizeGraphicsEngine( unsigned int width, unsigned int height )
{
	MaloW::Debug("Resizing Engine to: " + MaloW::convertNrToString(width) + ", " + MaloW::convertNrToString(height));
	if(this->isManagingMyOwnWindow)
		SetWindowPos(this->hWnd, 0 , 0 , 0, width, height, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

	this->parameters.WindowWidth = width;
	this->parameters.WindowHeight = height;
	this->dx->ResizeEngine(width, height);
}

Vector3 GraphicsEngineImp::GetSceneAmbientLight() const
{
	D3DXVECTOR3 amb = this->dx->GetSceneAmbientLight();
	return Vector3(amb.x, amb.y, amb.z);
}

void GraphicsEngineImp::DeleteTerrain( iTerrain*& terrain )
{
	this->dx->DeleteTerrain(dynamic_cast<Terrain*>(terrain));
	terrain = 0;
}

void GraphicsEngineImp::SetSunLightDisabled()
{
	this->dx->SetSunLightDisabled();
}

Vector3 GraphicsEngineImp::GetSunLightDirection() const
{
	D3DXVECTOR3 v = this->dx->GetSunLight().direction;
	return Vector3(v.x, v.y, v.z);
}

Vector3 GraphicsEngineImp::GetSunLightColor() const
{
	D3DXVECTOR3 v = this->dx->GetSunLight().lightColor;
	return Vector3(v.x, v.y, v.z);
}

float GraphicsEngineImp::GetSunLightIntensity() const
{
	return this->dx->GetSunLight().intensity;
}

iWaterPlane* GraphicsEngineImp::CreateWaterPlane( Vector3& pos, const char* texture )
{
	WaterPlane* plane = new WaterPlane(D3DXVECTOR3(pos.x, pos.y, pos.z));
	this->dx->CreateWaterPlane(plane, texture);
	return plane;
}

void GraphicsEngineImp::DeleteWaterPlane( iWaterPlane* del )
{
	if(WaterPlane* plane = dynamic_cast<WaterPlane*>(del))
		this->dx->DeleteWaterPlane(plane);
}

iFBXMesh* GraphicsEngineImp::CreateFBXMesh( const char* filename, Vector3 pos )
{
	FBXMesh* mesh = new FBXMesh(D3DXVECTOR3(pos.x, pos.y, pos.z));
	LoadMeshEvent* re = new LoadMeshEvent(filename, mesh);
	this->PutEvent(re);
	return mesh;
}

void GraphicsEngineImp::DeleteFBXMesh( iFBXMesh* mesh )
{
	if(FBXMesh* fmesh = dynamic_cast<FBXMesh*>(mesh))
		this->dx->DeleteFBXMesh(fmesh);
}