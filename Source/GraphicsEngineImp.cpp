#include "GraphicsEngineImp.h"
/*
DxManager* GraphicsEngine::dx = NULL;
HINSTANCE GraphicsEngine::hInstance = NULL;
HWND GraphicsEngine::hWnd = NULL;
MaloW::KeyListener* GraphicsEngine::kl = NULL;
SoundEngine* GraphicsEngine::sound = NULL;
*/
bool CursorControl::visable = true;

int GraphicsEngineParams::windowWidth = 1024;
int GraphicsEngineParams::windowHeight = 768;
bool GraphicsEngineParams::Maximized = true;
int GraphicsEngineParams::ShadowMapSettings = 0;
int GraphicsEngineParams::FXAAQuality = 0;
CameraType GraphicsEngineParams::CamType = FPS;


GraphicsEngineImp::GraphicsEngineImp(GraphicsEngineParams params, HINSTANCE hInstance, int nCmdShow) :
	GraphicsEngine()
{
	this->parameters = params;
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
	
	this->physx = new PhysicsEngine();
	this->kl = new KeyListener(this->hWnd);
	this->InitWindow(hInstance, nCmdShow);
	kl->SetHWND(this->hWnd); // Because of key listener being created before the window

	this->Start();
}

GraphicsEngineImp::GraphicsEngineImp(GraphicsEngineParams params, HWND hWnd) :
	GraphicsEngine()
{
	this->parameters = params;
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

	this->physx = new PhysicsEngine();
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
				MaloW::Debug("Multiple files not supported, you tried to drop " + MaloW::convertNrToString(nrOfFiles) + " files.");

			TCHAR lpszFile[MAX_PATH] = {0};
			lpszFile[0] = '\0';
			if(DragQueryFile(drop, 0, lpszFile, MAX_PATH))
			{
				gfx->specialString = string(lpszFile);
			}
			else
				MaloW::Debug("Failed to load a droppped file.");
			}
			break;

		// TODO: Handle Resize
		case WM_SIZE:
			{
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
	RECT rc = { 0, 0, this->parameters.windowWidth, this->parameters.windowHeight };
	if(this->parameters.Maximized)
	{
		AdjustWindowRectEx(&rc, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
		this->hWnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, "GraphicsEngine", "GraphicsEngine - Direct3D 11.0", WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, this->hInstance, this);
	}
	else
	{
		AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
		this->hWnd = CreateWindow("GraphicsEngine", "GraphicsEngine - Direct3D 11.0", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, this->hInstance, this);
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

	this->dx->Start();
}

StaticMesh* GraphicsEngineImp::CreateStaticMesh(string filename, D3DXVECTOR3 pos)
{
	StaticMesh* mesh = new StaticMesh(pos);
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

AnimatedMesh* GraphicsEngineImp::CreateAnimatedMesh(string filename, D3DXVECTOR3 pos)
{
	AnimatedMesh* mesh = new AnimatedMesh(pos);

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

void GraphicsEngineImp::DeleteImage( iImage* delImg )
{
	this->DeleteImage(dynamic_cast<Image*>(delImg));
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

void GraphicsEngineImp::DeleteText( iText* deltxt )
{
	this->DeleteText(dynamic_cast<Text*>(deltxt));
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

	std::string txt = "FPS: " + MaloW::convertNrToString((float)this->fpsLast) + " - "; 
	txt += "Camera Pos: " + MaloW::convertNrToString(this->dx->GetCamera()->GetPosition().x) + " " + 
		MaloW::convertNrToString(this->dx->GetCamera()->GetPosition().y) + " " + 
		MaloW::convertNrToString(this->dx->GetCamera()->GetPosition().z) + "    -    Triangle Count: " + MaloW::convertNrToString((float)this->dx->GetTriangleCount());

	SetWindowText(this->hWnd, txt.c_str());

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
			if(dynamic_cast<LoadMeshEvent*>(ev) != NULL)
			{
				string filename = ((LoadMeshEvent*)ev)->GetFileName();
				if(StaticMesh* mesh = ((LoadMeshEvent*)ev)->GetStaticMesh())
				{
					mesh->LoadFromFile(filename);
					this->dx->CreateStaticMesh(mesh);

					if(Material* material = ((LoadMeshEvent*)ev)->GetMaterial())
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
				else if(AnimatedMesh* mesh = ((LoadMeshEvent*)ev)->GetAnimatedMesh())
				{
					mesh->LoadFromFile(filename);
					this->dx->CreateAnimatedMesh(mesh);
				}
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

void GraphicsEngineImp::LoadingScreen(string BackgroundTexture, string ProgressBarTexture, float FadeBlackInInTime, float FadeBlackInOutTime, float FadeBlackOutInTime, float FadeBlackOutOutTime)
{
	this->Update();

	Image* bg = NULL;
	if(BackgroundTexture != "")
		bg = this->CreateImage(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->parameters.windowWidth, (float)this->parameters.windowHeight), BackgroundTexture);

	Image* pb = NULL;
	if(ProgressBarTexture != "")
		pb = this->CreateImage(D3DXVECTOR2((this->parameters.windowWidth / 4.0f), ((this->parameters.windowHeight * 3.0f) / 4.0f)), D3DXVECTOR2(0, this->parameters.windowHeight / 10.0f), ProgressBarTexture);

	int TotalItems = this->GetEventQueueSize();

	float dx = (this->parameters.windowWidth / 2.0f) / TotalItems;
	float y = this->parameters.windowHeight / 10.0f;

	Image* fade = NULL;
	if(FadeBlackInInTime != 0.0f || FadeBlackInOutTime != 0.0f || FadeBlackOutInTime != 0.0f || FadeBlackOutOutTime != 0.0f)
		fade = this->CreateImage(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2((float)this->parameters.windowWidth, (float)this->parameters.windowHeight), "Media/LoadingScreen/FadeTexture.png");

	int state = 0;
	/*
	0 = fade in
	1 = fade out to loading screen
	2 = loading 
	3 = fade in
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
		
		if(this->loading)
			if(pb)
				pb->SetDimensions(Vector2(dx * (TotalItems - ItemsToGo), y));
	}

	if(fade)
	{
		this->DeleteImage(fade);
	}
}

iMesh* GraphicsEngineImp::CreateMesh( const char* filename, const Vector3& pos )
{
	return CreateStaticMesh(filename,D3DXVECTOR3(pos.x,pos.y,pos.z));
}

iLight* GraphicsEngineImp::CreateLight( Vector3 pos )
{
	return this->CreateLight(D3DXVECTOR3(pos.x, pos.y, pos.z));
}

void GraphicsEngineImp::DeleteLight( iLight* light )
{
	this->DeleteLight(dynamic_cast<Light*>(light));
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
	this->DeleteStaticMesh(dynamic_cast<StaticMesh*>(delMesh));
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
