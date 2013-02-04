#define TEST //<----------------------- kommentera ut vid behov **********************


#if defined(DEBUG) || defined(_DEBUG)
	#include <vld.h>
	#define INCLUDE_MODEL_VIEWER
#endif

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"

void ReplaceSlashes(string& str, char replace, char with)
{
	for(unsigned int i = 0; i < str.size(); i++)
	{
		if(str.at(i) == replace)
			str.at(i) = with;
	}
}

// Does work, not when running through visual studio tho since it fucks up the path that GetModuleFileName gives.
void deleteCache()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szPath, MAX_PATH);
	string path = string(szPath);
	path = path.substr(0, path.size() - string("ModelViewer.exe").size());
	path += "\\Media\\Cache\\*";
	MaloW::Debug(path);

	int len = strlen(path.c_str()) + 2; // required to set 2 nulls at end of argument to SHFileOperation.
	char* tempdir = (char*) malloc(len);
	memset(tempdir,0,len);
	strcpy(tempdir,path.c_str()); //**tillman**

	SHFILEOPSTRUCT file_op = {
		NULL,
		FO_DELETE,
		tempdir,
		"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		"" };

	if ( int ret = SHFileOperation(&file_op) )
	{
		throw("Error!");
	}

	free(tempdir);
}

int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE, LPWSTR, int )
{
	MaloW::ClearDebug();
	if ( !GraphicsInit(hInstance) )
		MaloW::Debug("Failed Creating Graphics Engine!");
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	MaloW::Debug("(DEBUG): ModelViewer: Debug flag set to: _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF). ");
	#ifdef INCLUDE_MODEL_VIEWER
		MaloW::Debug("(DEBUG): ModelViewer: vld.h included.");
	#endif
#endif

	GetGraphics()->CreateSkyBox("Media/skymap.dds"); //** TILLMAN
	GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));
	iLight* li = GetGraphics()->CreateLight(GetGraphics()->GetCamera()->GetPosition());
	li->SetIntensity(0.001f);
	GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1));
	GetGraphics()->SetSceneAmbientLight(Vector3(0.4f, 0.4f, 0.4f));


//*************************************	     PRE TEST       **********************
#ifdef TEST


	//Preload testing
	unsigned int nrOfResources = 6;
	const char* resourceFileNames[6];
	resourceFileNames[0] = "Media/ball.obj";
	resourceFileNames[1] = "Media/bth.obj";			
	resourceFileNames[2] = "Media/scale.obj";		//1
	resourceFileNames[3] = "Media/FlagBlue.ani";		
	resourceFileNames[4] = "Media/BallTexture.png";	
	resourceFileNames[5] = "Media/scale.obj";		//2

	//resourceFileNames[10] = "skymap.dds";			//Fail
	//resourceFileNames[6] = "scale.obj";				//Fail
	//resourceFileNames[7] = "durp.fileformat";		//Fail
	//resourceFileNames[8] = "skymap.dds";			//fail
	//resourceFileNames[3] = "BallTexture.png";		//fail
	GetGraphics()->PreLoadResources(nrOfResources, resourceFileNames);






	iWaterPlane* wp = GetGraphics()->CreateWaterPlane(Vector3(0, 10, 0), "Media/WaterTexture.png");
	wp->SetScale(10.0f);
	iWaterPlane* wp2 = GetGraphics()->CreateWaterPlane(Vector3(5, 10, 0), "Media/WaterTexture.png");
	wp2->Rotate(Vector3(0.1f, 0.1f, 0.1f));
	wp2->SetScale(8.0f);
	
	iImage* iM = GetGraphics()->CreateImage(Vector2(100, 100), Vector2(100, 100), "Media/BallTexture.png");
	iImage* iM2 = GetGraphics()->CreateImage(Vector2(200, 200), Vector2(100, 100), "Media/BallTexture.png");
	
	//iMesh* bbbush = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(-1, 0, 0));

		

	iImage* iM5 = GetGraphics()->CreateImage(Vector2(300, 100), Vector2(100, 100), "Media/Arrow_v01.png");	//**minnesläcka om createmesh använder samma texture som en image OCH skapas FÖRE!

	iMesh* arrow = GetGraphics()->CreateMesh("Media/Arrow_v01.obj", Vector3(30, 10, 30));					//**ORSAKAR minnesläcka om createmesh använder samma texture OCH skapas FÖRE!
	arrow->Scale(1.0f * 0.05f);
	Vector3 arrowDir = Vector3(0, 0, -1);
	

	iImage* iM3 = GetGraphics()->CreateImage(Vector2(100, 200), Vector2(100, 100), "Media/Bush_leaf_01_v07.png"); //**minnesläcka om arrow mesh & image ligger ovanför och Bush_leaf_01_v07.png finns fortfarande efter den har tagits bort.
	iImage* iM4 = GetGraphics()->CreateImage(Vector2(200, 100), Vector2(100, 100), "Media/Bush_leaf_01_v07.png");



	int vertSize = 64;
	float testSize = 25.0f;
	
	iTerrain* iT = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(testSize, 0.0f, testSize), vertSize);
	iTerrain* iT2 = GetGraphics()->CreateTerrain(Vector3(testSize, 0, 0), Vector3(testSize, 0.0f, testSize), vertSize);
	iAnimatedMesh* iAM = GetGraphics()->CreateAnimatedMesh("Media/FlagBlue.ani", Vector3(30, 30, 30));
	//iImage* iM = GetGraphics()->CreateImage(Vector2(100, 100), Vector2(100, 100), "Media/BallTexture.png");
	//iText* iTe = GetGraphics()->CreateText("durp", Vector2(300, 100), 1.0f, "Media/Fonts/1");
	
	float test = 0.0f;
	Vector2 dd = Vector2(0.1f, 0.2f);
	test = dd[0];
	Vector3 durp = Vector3(1.0f, 2.0f, 3.0f);
	test = durp[2];
	Vector4 hurp = Vector4(10.0f, 20.0f, 30.0f, 40.0f);
	test = hurp[3];

	float inner = 1.0f;
	float outer = inner * 2;
	GetGraphics()->SetSpecialCircle(inner, outer, Vector2(0, 0));
	float hmData[65 * 65];
	for(int i = 0; i < vertSize * vertSize; i++)
	{
		/*hmData[0] = 0.0f;
		hmData[1] = 0.0f;
		hmData[2] = 2.0f;
		hmData[3] = 0.0f;*/
		hmData[i] = 0.0f;
	}
	iT->SetHeightMap(hmData);
	const char* fileNames[8];
	fileNames[0] = "Media/TerrainTexture.png";
	fileNames[1] = "Media/TerrainTexture.png";
	fileNames[2] = "Media/TerrainTexture.png";
	fileNames[3] = "Media/TerrainTexture.png";
	fileNames[4] = "Media/TerrainTexture.png";
	fileNames[5] = "Media/TerrainTexture.png";
	fileNames[6] = "Media/TerrainTexture.png";
	fileNames[7] = "Media/TerrainTexture.png";
	iT->SetTextures(fileNames);
	iT->SetHeightMap(hmData);
	iT->SetTextureScale(-4);

	float testF = 0.0f;
	try
	{
		testF = iT->GetYPositionAt(0.5f, 0.5f);
	}
	catch(...)
	{
	}
	
	int nrOfBlendMaps = 2;
	unsigned int size = 2048;
	unsigned int* sizes = new unsigned int[2];
	sizes[0] = size;
	sizes[1] = size;
	float** testData = new float*[nrOfBlendMaps]; 
	unsigned int channels = 4;
	
	testData[0] = new float[sizes[0]*sizes[0]*channels];
	for(unsigned int i = 0; i < sizes[0]*sizes[0]; i++)
	{
		testData[0][ i * channels + 0 ] = 0.0f;
		testData[0][ i * channels + 1 ] = 0.0f;
		testData[0][ i * channels + 2 ] = 0.0f;
		testData[0][ i * channels + 3 ] = 0.0f;
	}
	testData[1] = new float[sizes[1]*sizes[1]*channels];
	for(unsigned int i = 0; i < sizes[1]*sizes[1]; i++)
	{
		testData[1][ i * channels + 0 ] = 0.0f;
		testData[1][ i * channels + 1 ] = 0.0f;
		testData[1][ i * channels + 2 ] = 0.0f;
		testData[1][ i * channels + 3 ] = 0.0f;
	}


	iT->SetBlendMaps(nrOfBlendMaps, sizes, testData);


	fileNames[0] = "Media/TerrainTexture.png";
	fileNames[1] = "Media/TerrainTexture.png";
	fileNames[2] = "Media/TerrainTexture.png";
	fileNames[3] = "Media/TerrainTexture.png";
	fileNames[4] = "Media/TerrainTexture.png";
	fileNames[5] = "Media/TerrainTexture.png";
	fileNames[6] = "Media/TerrainTexture.png";
	fileNames[7] = "Media/TerrainTexture.png";
	iT2->SetTextures(fileNames);
	iT2->SetBlendMaps(nrOfBlendMaps, sizes, testData);

	int klerp = 3;
	iTerrain** iTs = new iTerrain*[klerp*klerp];
	for(int i = 0; i < klerp; i++)
	{
		for(int j = 0; j < klerp; j++)
		{
			iTs[i * klerp + j] = GetGraphics()->CreateTerrain(Vector3((testSize * 2) + (i * testSize), 0, (testSize * 2) + (j * testSize)), Vector3(testSize, 0.0f, testSize), vertSize);
			iTs[i * klerp + j]->SetTextures(fileNames);
			iTs[i * klerp + j]->SetBlendMaps(nrOfBlendMaps, sizes, testData);
			//iTs[i * klerp + j]->SetTextureScale(10);
		}
	}
	
	
	// test fps
	/*
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, -5));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, 0));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, 5));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 10 + i * 3, 10));
		*/
	
	iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0, -100, 0));
	ball->Scale(0.1f);
	iMesh* secModel = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(10, 2, 10));
	secModel->Scale(1.0f * 0.05f);

	iMesh* bush = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(30, 10, 30));
	bush->Scale(1.0f * 0.05f);

	//iMesh* arrow = GetGraphics()->CreateMesh("Media/Arrow_v01.obj", Vector3(30, 10, 30));
	//arrow->Scale(1.0f * 0.05f);
	//Vector3 arrowDir = Vector3(0, 0, -1);



	//CASCADED SHADOWMAP:
	//Transparancy:
	iMesh* bushCSMTest = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(10, 2, 0));
	bushCSMTest->Scale(1.0f * 0.05f);
	iMesh* flagCSMTest = GetGraphics()->CreateMesh("Media/FlagBlue.ani", Vector3(12, 2, 0));

	//General shadowing:
	iMesh* bushScaleBugTest = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(0, 0, 0));
	iMesh* scaleBugTest = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(5, 0, 0));
	iMesh* bushScaleBugTest2 = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(2, 0, 0));
	iMesh* scaleBugTest2 = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(4, 0, 0));

	bushScaleBugTest->Scale(1.0f * 0.05f);
	scaleBugTest->Scale(1.0f * 0.05f);
	bushScaleBugTest2->Scale(1.0f * 0.05f);
	scaleBugTest2->Scale(1.0f * 0.25f);
	/*
	Vector3 lookAt = Vector3(0, 0, 0);
	Vector3 pos = Vector3(-50, 50, -50);
	Vector3 posToLookAt = lookAt - pos; //Pos --> lookAt
	Vector3 dir;
	Vector3 up = Vector3(0, 1, 0);
	//D3DXVec3Normalize(&dir, &posToLookAt); 
	posToLookAt.Normalize();
	dir = posToLookAt;
	GetGraphics()->SetSunLightProperties(dir);
	//Set camera to look along the x-axis
	GetGraphics()->GetCamera()->SetPosition(Vector3(0, 1, 0));
	GetGraphics()->GetCamera()->LookAt(Vector3(1, 1, 0));

	//calculate points (in world space) for the frustum slice
	Vector3 camPos = GetGraphics()->GetCamera()->GetPosition();
	Vector3 camForward = GetGraphics()->GetCamera()->GetForward();
	Vector3 camRight = GetGraphics()->GetCamera()->GetRightVector();
	Vector3 camUp = GetGraphics()->GetCamera()->GetUpVector();
	float ww = (float)GetGraphics()->GetEngineParameters()->windowWidth;
	float hh = (float)GetGraphics()->GetEngineParameters()->windowHeight;
	float aspectRatio = ww / hh;
	float tmp = tan(GetGraphics()->GetEngineParameters()->FOV) * 2;

	float planes[4];
	planes[0] = GetGraphics()->GetEngineParameters()->NearClip;
	planes[1] = GetGraphics()->GetEngineParameters()->FarClip * 0.1f;
	planes[2] = GetGraphics()->GetEngineParameters()->FarClip * 0.4f;
	planes[3] = GetGraphics()->GetEngineParameters()->FarClip;

	//**vertices to render as debug info**
	int nrOfFrustumSlices = 3;
	int ttte = 8;
	Vector3* vertices = new Vector3[ttte * nrOfFrustumSlices];

	for(int i = 0; i < nrOfFrustumSlices; i++)
	{
		//Near plane	
		float halfNearHeight = tmp * planes[i];
		float halfNearWidth = halfNearHeight * aspectRatio;

		//Near points
		Vector3	nearCenter = camPos + camForward * planes[i]; //near plane of slice
		Vector3	nearTopLeft = nearCenter + (camUp * halfNearHeight) - (camRight * halfNearWidth);
		Vector3	nearTopRight = nearCenter + (camUp * halfNearHeight) + (camRight * halfNearWidth);
		Vector3	nearBottomLeft = nearCenter - (camUp * halfNearHeight) - (camRight * halfNearWidth);
		Vector3	nearBottomRight = nearCenter - (camUp * halfNearHeight) + (camRight * halfNearWidth);

		/*vertices[i * ttte] = nearCenter;
		vertices[i * ttte + 1] = nearTopLeft;
		vertices[i * ttte + 2] = nearTopRight;
		vertices[i * ttte + 3] = nearBottomLeft;
		vertices[i * ttte + 4] = nearBottomRight;*/
		/*vertices[i * ttte] = nearTopLeft;
		vertices[i * ttte + 1] = nearTopRight;
		vertices[i * ttte + 2] = nearBottomLeft;
		vertices[i * ttte + 3] = nearBottomRight;
		
		//Far plane
		float halfFarHeight = tmp * planes[i + 1];
		float halfFarWidth = halfFarHeight * aspectRatio;
		//Far points
		Vector3 farCenter = camPos + camForward * planes[i + 1]; //far plane of slice
		Vector3 farTopLeft = farCenter + (camUp * halfFarHeight) - (camRight * halfFarWidth);
		Vector3	farTopRight = farCenter + (camUp * halfFarHeight) + (camRight * halfFarWidth);
		Vector3	farBottomLeft = farCenter - (camUp * halfFarHeight) - (camRight * halfFarWidth);
		Vector3	farBottomRight = farCenter - (camUp * halfFarHeight) + (camRight * halfFarWidth);

		/*vertices[i * ttte + 5] = farCenter;
		vertices[i * ttte + 6] = farTopLeft;
		vertices[i * ttte + 7] = farTopRight;
		vertices[i * ttte + 8] = farBottomLeft;
		vertices[i * ttte + 9] = farBottomRight;*/
		/*vertices[i * ttte + 4] = farTopLeft;
		vertices[i * ttte + 5] = farTopRight;
		vertices[i * ttte + 6] = farBottomLeft;
		vertices[i * ttte + 7] = farBottomRight;
	}
	*/

	//Transform points into light’s homogeneous space.
	//GetGraphics()->DebugDummyFunction(vertices);

	//render
	/*iMesh** debugCSMPoints = new iMesh*[ttte * nrOfFrustumSlices];
	for(int i = 0; i < ttte * nrOfFrustumSlices; i++)
	{
		//debugCSMPoints[i] = GetGraphics()->CreateStaticMesh("Media/ball.obj", vertices[i]); //Camera frustum corners
	}
	//debugCSMPoints[0]->SetSpecialColor(GREEN_COLOR); //min
	//debugCSMPoints[1]->SetSpecialColor(BLUE_COLOR); //max

	//GetGraphics()->CreateStaticMesh("Media/CSMDebug.obj", Vector3(0, 0, 0)); //Camera frustum slices
	
	//restore camera settings
	/*GetGraphics()->GetCamera()->SetPosition(Vector3(25, 25, 20));
	GetGraphics()->GetCamera()->LookAt(Vector3(0, 0, 0));
	*/
	//for(int i = 0; i < 10; i++)
		//for(int u = 0; u < 10; u++)
			//GetGraphics()->CreateMesh("Media/Asdbfd.obj", Vector3(0 + i * 5, 5, 0 + u * 5))->SetScale(1.0f * 0.05f);

	iMesh* ball2 = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(20, 20, 20));
	ball2->Scale(0.8f);


	//AI grid test
	char aiData[64*64];
	for(int i = 0; i < 64*64/2; i++)
	{
		aiData[i] = 0; //not blocked = false = 0 = green
	}
	for(int i = 64*64/2; i < 64*64; i++)
	{
		aiData[i] = 255; //blocked = true = 255 = 1 = red
	}
	aiData[0] = 255; //blocked = true = 255 = 1 = red
	aiData[77] = 255; //blocked = true = 255 = 1 = red
	aiData[9] = 255; //blocked = true = 255 = 1 = red
	aiData[90] = 255; //blocked = true = 255 = 1 = red
	aiData[177] = 255; //blocked = true = 255 = 1 = red
	aiData[356] = 255; //blocked = true = 255 = 1 = red
	aiData[1278] = 255; //blocked = true = 255 = 1 = red
	aiData[2743] = 0; //blocked = false = 0 = green
	aiData[2999] = 0; //blocked = false = 0 = green
	aiData[3204] = 0; //blocked = false = 0 = green
	aiData[3777] = 0; //blocked = false = 0 = green
	
	//iMesh* aniFern = GetGraphics()->CreateMesh("Media/Fern_02.ani", Vector3(-1, 0, 0));
	Vector3 cameraPoint = Vector3(0.0f, 5.0f, 5.0f);
	Vector3 cameraLookAt = Vector3(100.0f, 0.0f, 0.0f);

	iLight* lll = GetGraphics()->CreateLight(Vector3(0,2,0));
	lll->SetLookAt(Vector3(1, 0, 1));
	lll->SetIntensity(1.0f);
	iMesh* mmm = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0,2,0));


	iMesh* temp = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(50, 0, 0));
	temp->SetScale(0.05f);




	GetGraphics()->CreateMesh("Media/scale.obj", Vector3(30, -300, 30));
	
#endif
//*************************************	    END OF PRE TEST       **********************

	

	iMesh* scaleHuman = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(30, -300, 30));
	iMesh* model = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(15, 20, 20));
	scaleHuman->Scale(1.0f / 20.0f);
	model->Scale(1.0f * 0.05f);
#ifdef TEST
	GetGraphics()->LoadingScreen("Media/LoadingScreen/LoadingScreenBG.png", "Media/LoadingScreen/LoadingScreenPB.png"
		, 1.0f, 1.0f, 1.0f, 1.0f);
#endif

	// To stop the engine rendering a splash image before it has loaded stuff 
	// to not get a clear-color rendered before skybox is in etc.
	GetGraphics()->StartRendering();	

	string lastSpecString = "";
	bool showscale = false;
	bool toggleScale = true;
	bool toggleDelCache = true;
	bool toggleLight = true;
	bool go = true;
	float tempInt = 10.0f;
	while(GetGraphics()->IsRunning() && go)
	{
		Sleep(10);
		string specString = GetGraphics()->GetSpecialString();
		ReplaceSlashes(specString, '\\', '/');
		if(specString != lastSpecString)
		{
			TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPath, MAX_PATH);
			string path = string(szPath);
			path = path.substr(0, path.size() - string("ModelViewer.exe").size());

			string loadModel = specString.substr(path.size() , specString.size());
			GetGraphics()->DeleteMesh(model);
			if(loadModel.substr((loadModel.size() - 4), loadModel.size()) == ".obj")
				model = GetGraphics()->CreateMesh(loadModel.c_str(), Vector3(15, 20, 20));
			else if(loadModel.substr((loadModel.size() - 4), loadModel.size()) == ".ani")
				model = GetGraphics()->CreateAnimatedMesh(loadModel.c_str(), Vector3(15, 20, 20));
			model->Scale(1.0f / 20.0f);
			lastSpecString = specString;
		}

		// Updates camera etc, does NOT render the frame, another process is doing that, so diff should be very low.
		float diff = GetGraphics()->Update();	
		

//*************************************	     RUN TESTS       **********************
#ifdef TEST
		CollisionData cd;
		//cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(
			//GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->Get3DPickingRay(), iAM);
		
		//diff = GetGraphics()->Update();
		//GetGraphics()->GetPhysicsEngine()->GetCollisionRayTerrain(
			//GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->GetForward(), iT);
		//diff = GetGraphics()->Update();
		//MaloW::Debug("Normal: " + MaloW::convertNrToString(diff));
		//diff = GetGraphics()->Update();
		cd = GetGraphics()->GetPhysicsEngine()->GetSpecialCollisionRayTerrain(
			GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->GetForward(), iT, testSize / (vertSize - 1));
		//diff = GetGraphics()->Update();
		//MaloW::Debug("Special: " + MaloW::convertNrToString(diff));
		//MaloW::Debug(" ");
		//diff = 100.0f;
		cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(GetGraphics()->GetCamera()->GetPosition(), 
			GetGraphics()->GetCamera()->GetForward(), wp);
		
		if(cd.BoundingSphereCollision)
		{
			ball->SetScale(0.2f);
		}
		else
		{
			ball->SetScale(0.1f);
		}

		if(cd.collision)
		{
			ball->SetPosition(Vector3(cd.posx, cd.posy, cd.posz));
		}
		
		static bool fesd = true;
		if(GetGraphics()->GetKeyListener()->IsPressed('H'))
		{
			if(fesd)
			{
				if(GetGraphics()->GetCamera()->GetCameraType() == RTS)
				{
					Vector3 fw = GetGraphics()->GetCamera()->GetForward();
					GetGraphics()->ChangeCamera(FPS);
					GetGraphics()->GetCamera()->SetForward(fw);
					//GetGraphics()->ResizeGraphicsEngine(500, 500);
				}
				else
				{
					Vector3 fw = GetGraphics()->GetCamera()->GetForward();
					GetGraphics()->ChangeCamera(RTS);
					GetGraphics()->GetCamera()->SetForward(fw);
					//GetGraphics()->ResizeGraphicsEngine(750, 250);
				}
				//fesd = false;
			}			
		}
		else
			fesd = true;

		if(GetGraphics()->GetKeyListener()->IsPressed('J'))
			GetGraphics()->SetSceneAmbientLight(Vector3(1.0f, 0.0f, 0.0f));


		if(GetGraphics()->GetKeyListener()->IsPressed(VK_UP))
			secModel->MoveBy(Vector3(1, 0, 0) * diff * 0.1f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_DOWN))
			secModel->MoveBy(Vector3(-1, 0, 0) * diff * 0.1f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_LEFT))
			secModel->MoveBy(Vector3(0, 0, 1) * diff * 0.1f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_RIGHT))
			secModel->MoveBy(Vector3(0, 0, -1) * diff * 0.1f);

		//CASCADED SHADOW MAPPING
		static float debugCSMScale = 1.0f;
		if(GetGraphics()->GetKeyListener()->IsPressed('Q'))
		{
			debugCSMScale += diff * 0.001f;
			model->Scale(1 + diff * 0.01f);
			fileNames[0] = "Media/TerrainTexture.png";
			fileNames[1] = "Media/TerrainTexture.png";
			fileNames[2] = "Media/TerrainTexture.png";
			fileNames[3] = "Media/TerrainTexture.png";
			fileNames[4] = "Media/BallTexture.png";
			fileNames[5] = "Media/BallTexture.png";
			fileNames[6] = "Media/BallTexture.png";
			fileNames[7] = "Media/BallTexture.png";
			iT2->SetTextures(fileNames);
			iT->SetAIGridThickness(0.0005f);
			iT->UseAIMap(true);
			static bool oncee = false;
			if(!oncee)
			{
				iT->SetAIGrid(64, (void*)&aiData);
				oncee = true;
			}
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('E'))
		{
			debugCSMScale -= diff * 0.001f;
			model->Scale(1 - diff * 0.01f);
			fileNames[0] = "Media/TerrainTexture.png";
			fileNames[1] = "Media/TerrainTexture.png";
			fileNames[2] = "Media/TerrainTexture.png";
			fileNames[3] = "Media/TerrainTexture.png";
			fileNames[4] = "Media/TerrainTexture.png";
			fileNames[5] = "Media/TerrainTexture.png";
			fileNames[6] = "Media/TerrainTexture.png";
			fileNames[7] = "Media/TerrainTexture.png";
			iT->SetTextures(fileNames);
			GetGraphics()->DeleteMesh(flagCSMTest);
			GetGraphics()->DeleteMesh(bushCSMTest);
			iT->UseAIMap(false);
		}
		/*for(int i = 0; i < ttte * nrOfFrustumSlices; i++)
		{
			//debugCSMPoints[i]->SetScale(debugCSMScale);
		}*/
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SHIFT))
			secModel->MoveBy(Vector3(0, 1, 0) * diff * 0.1f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_CONTROL))
			secModel->MoveBy(Vector3(0, -1, 0) * diff * 0.1f);


		if(GetGraphics()->GetKeyListener()->IsClicked(1))
		{
			arrow->SetPosition(GetGraphics()->GetCamera()->GetPosition());

			Vector3 vec(0, 0, -1);

			Vector3 camDir = GetGraphics()->GetCamera()->GetForward();
			arrowDir = camDir;

			arrow->ResetRotation();
			Vector3 around = vec.GetCrossProduct(camDir);
			float angle = acos(vec.GetDotProduct(camDir) / (vec.GetLength() * camDir.GetLength()));
			arrow->RotateAxis(around, angle);
		}

		arrow->MoveBy(arrowDir * diff * 0.01f);
		

		/*
		templol += diff;
		if(templol > 100000.0f)
		{
			CollisionData secCD = GetGraphics()->GetPhysicsEngine()->GetCollisionMeshMesh(model, secModel);
			if(secCD.collision)
			{
				secModel->SetSpecialColor(RED_COLOR);
				MaloW::Debug("Collision! :D");
			}
			else
			{
				MaloW::Debug("No collision... :(");
				secModel->SetSpecialColor(WHITE_COLOR);
			}
			templol = 0.0f;
		}
		
		*/

		//Editor crash testing
		//GetGraphics()->DeleteMesh(spawnP);
		/*static bool once = false;
		if(!once)
		{
			int index = 0;
			iMesh* spawnPP = GetGraphics()->CreateStaticMesh("Media/spawn.obj", Vector3(0,0, 2 * index++));
			iMesh* spawnPPP = GetGraphics()->CreateStaticMesh("Media/spawn.obj", Vector3(0,0, 2 * index++)); //Crashes
			once = true;
		}*/

		//Camera reset/teleport
		if(GetGraphics()->GetKeyListener()->IsPressed('V'))
		{
			GetGraphics()->GetCamera()->SetPosition(cameraPoint);
			GetGraphics()->GetCamera()->LookAt(cameraLookAt);
		}

		if(GetGraphics()->GetKeyListener()->IsPressed('O'))
		{
			GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), GetGraphics()->GetSunLightColor() * (1.0f + diff * 0.002f));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('P'))
		{
			GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), GetGraphics()->GetSunLightColor() * (1.0f - diff * 0.002f));
		}

		//Toggle shadow on/off
		if(GetGraphics()->GetKeyListener()->IsPressed('M'))
		{
			GetGraphics()->UseShadow(false);
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('K'))
		{
			GetGraphics()->UseShadow(true);
		}
		//Move test light
		if(GetGraphics()->GetKeyListener()->IsPressed('U'))
		{
			lll->Move(Vector3(0.01f * diff, 0, 0));
			mmm->MoveBy(Vector3(0.01f * diff, 0, 0));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('I'))
		{
			lll->Move(Vector3(-0.01f * diff, 0, 0));
			mmm->MoveBy(Vector3(-0.01f * diff, 0, 0));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('O'))
		{
			lll->Move(Vector3(0, 0, -0.01f * diff));
			mmm->MoveBy(Vector3(0, 0, -0.01f * diff));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('P'))
		{
			lll->Move(Vector3(0, 0, 0.01f * diff));
			mmm->MoveBy(Vector3(0, 0, 0.01f * diff));
		}

#endif
//*************************************	    END OF RUN TESTS       **********************

		li->SetPosition(GetGraphics()->GetCamera()->GetPosition());

		if(GetGraphics()->GetKeyListener()->IsPressed('W'))
			GetGraphics()->GetCamera()->MoveForward(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('A'))
			GetGraphics()->GetCamera()->MoveLeft(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('S'))	
			GetGraphics()->GetCamera()->MoveBackward(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed('D'))	
			GetGraphics()->GetCamera()->MoveRight(diff * 10.0f);

		if(GetGraphics()->GetKeyListener()->IsPressed('Z'))	
		{
			GetGraphics()->GetKeyListener()->SetMousePosition(Vector2(
				(float)(GetGraphics()->GetEngineParameters().WindowWidth) / 2.0f, 
				(float)(GetGraphics()->GetEngineParameters().WindowHeight) / 2.0f));

			GetGraphics()->GetCamera()->SetUpdateCamera(false);
			GetGraphics()->GetKeyListener()->SetCursorVisibility(true);
		}

		if(GetGraphics()->GetKeyListener()->IsPressed('X'))	
		{
			GetGraphics()->GetKeyListener()->SetMousePosition(Vector2(
				GetGraphics()->GetEngineParameters().WindowWidth / 2.0f, 
				GetGraphics()->GetEngineParameters().WindowHeight / 2.0f));
			GetGraphics()->GetCamera()->SetUpdateCamera(true);
			GetGraphics()->GetKeyListener()->SetCursorVisibility(false);
		}

		if(GetGraphics()->GetKeyListener()->IsPressed('C'))
		{
			if(toggleScale)
			{
				if(showscale)
					scaleHuman->SetPosition(Vector3(30, -100, 30));
				else
					scaleHuman->SetPosition(Vector3(15, 20, 25));
				showscale = !showscale;
				toggleScale = false;
			}
		}
		else
			toggleScale = true;


		if(GetGraphics()->GetKeyListener()->IsPressed('L'))
		{
			if(toggleDelCache)
			{
				deleteCache();
				toggleDelCache = false;
			}
		}
		else
			toggleDelCache = true;


		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ADD))
			GetGraphics()->GetCamera()->SetSpeed(GetGraphics()->GetCamera()->GetSpeed() * (1.0f + diff * 0.01f));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SUBTRACT))
			GetGraphics()->GetCamera()->SetSpeed(GetGraphics()->GetCamera()->GetSpeed() * (1.0f - diff * 0.01f));

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_SPACE))
			GetGraphics()->GetCamera()->MoveUp(diff * 10.0f);
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_CONTROL))
			GetGraphics()->GetCamera()->MoveDown(diff * 10.0f);

		if(GetGraphics()->GetKeyListener()->IsPressed(VK_ESCAPE))
			go = false;


		if(GetGraphics()->GetKeyListener()->IsPressed('1'))
		{
			if(toggleLight)
			{
				if(li->GetIntensity() < 0.01f)
				{
					li->SetIntensity(tempInt);
				}
				else
				{
					tempInt = li->GetIntensity();
					li->SetIntensity(0.001f);
				}
				toggleLight = false;
			}
		}
		else
			toggleLight = true;

		if(GetGraphics()->GetKeyListener()->IsPressed('T'))
		{
			li->SetIntensity(li->GetIntensity() * (1.0f + diff * 0.002f));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('Y'))
		{
			li->SetIntensity(li->GetIntensity() * (1.0f - diff * 0.002f));
		}


		if(GetGraphics()->GetKeyListener()->IsPressed('B'))
		{
			GetGraphics()->SetSceneAmbientLight(GetGraphics()->GetSceneAmbientLight() * (1.0f + diff * 0.002f));
		}
		if(GetGraphics()->GetKeyListener()->IsPressed('N'))
		{
			GetGraphics()->SetSceneAmbientLight(GetGraphics()->GetSceneAmbientLight() * (1.0f - diff * 0.002f));
		}

		
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_UP))
			GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_DOWN))
			GetGraphics()->SetSunLightProperties(Vector3(-1, -1, -1));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_LEFT))
			GetGraphics()->SetSunLightProperties(Vector3(-1, -1, 1));
		if(GetGraphics()->GetKeyListener()->IsPressed(VK_RIGHT))
			GetGraphics()->SetSunLightProperties(Vector3(1, -1, -1));
		
	}
	

	//*************************************	     POST TEST       **********************
#ifdef TEST
	delete[] iTs;
	delete [] sizes;
	delete testData[0];
	delete testData[1];
	delete [] testData;
	//delete[] resourceFileNames;

	GetGraphics()->DeleteImage(iM);
	GetGraphics()->DeleteImage(iM2);
	GetGraphics()->DeleteImage(iM3);
	GetGraphics()->DeleteImage(iM4);
	GetGraphics()->DeleteImage(iM5);

#endif
	//*************************************	   END OF POST TEST       **********************

	FreeGraphics();
	return 0;
}
