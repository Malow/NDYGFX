#pragma once

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"

class TillmanTest
{
private:
	unsigned int* sizes;
	float** testData;
	iImage* iM;
	iImage* iM2;
	iImage* iM5;
	iImage* iM3;
	iImage* iM4;
	iLight* lll;
	iMesh* mmm;
	iMesh* unloadTestAni;
	Vector3 cameraPoint;
	Vector3 cameraLookAt;
	Vector3 cameraPoint2;
	Vector3 cameraLookAt2;
	Vector3 cameraPoint3;
	Vector3	cameraLookAt3;
	Vector3 cameraPoint4;
	iBillboard* iBBMemLeak;


	iMesh* bushCSMTest;
	const char* fileNames[8];
	iTerrain* iT;
	char aiData[64*64];
	iTerrain* iT2;

	int vertSize;
	float testSize;
	iTerrain* createTerrainIndexBufferCraschText;


public:
	TillmanTest() {};
	virtual ~TillmanTest() {};

	void PreTest();
	void RunTest(float diff);
	void PostTest();
};

void TillmanTest::PreTest()
{
	//Preload testing
	unsigned int nrOfResources = 5;
	const char* resourceFileNames[5];
	resourceFileNames[0] = "Media/ball.obj";
	resourceFileNames[1] = "Media/bth.obj";			
	resourceFileNames[2] = "Media/scale.obj";		//1
	resourceFileNames[3] = "Media/FlagBlue.ani";		
	resourceFileNames[4] = "Media/BallTexture.png";	
	//resourceFileNames[5] = "Media/scale.obj";		//2

	//resourceFileNames[10] = "skymap.dds";			//Fail
	//resourceFileNames[6] = "scale.obj";				//Fail
	//resourceFileNames[7] = "durp.fileformat";		//Fail
	//resourceFileNames[8] = "skymap.dds";			//fail
	//resourceFileNames[3] = "BallTexture.png";		//fail
	GetGraphics()->PreLoadResources(nrOfResources, resourceFileNames);


	iM = GetGraphics()->CreateImage(Vector2(100, 100), Vector2(100, 100), "Media/BallTexture.png");
	iM2 = GetGraphics()->CreateImage(Vector2(200, 200), Vector2(100, 100), "Media/BallTexture.png");


	iM5 = GetGraphics()->CreateImage(Vector2(300, 100), Vector2(100, 100), "Media/Arrow_v01.png");
	

	iM3 = GetGraphics()->CreateImage(Vector2(100, 200), Vector2(100, 100), "Media/Bush_leaf_01_v07.png");
	iM4 = GetGraphics()->CreateImage(Vector2(200, 100), Vector2(100, 100), "Media/Bush_leaf_01_v07.png");

	//iMesh* cacheBowCraschTest = GetGraphics()->CreateMesh("Media/Bow_v01.obj", Vector3(0, 2, 0));
	//cacheBowCraschTest->SetScale(5.0f);

	vertSize = 64;
	testSize = 25.0f;
	
	iT = GetGraphics()->CreateTerrain(Vector3(0, 0, 0), Vector3(testSize, 1.0f, testSize), vertSize);
	iT2 = GetGraphics()->CreateTerrain(Vector3(testSize, 0, 0), Vector3(testSize, 1.0f, testSize), vertSize);
	//iImage* iM = GetGraphics()->CreateImage(Vector2(100, 100), Vector2(100, 100), "Media/BallTexture.png");
	//iText* iTe = GetGraphics()->CreateText("durp", Vector2(300, 100), 1.0f, "Media/Fonts/1");
	
	float test = 0.0f;
	Vector2 dd = Vector2(0.1f, 0.2f);
	test = dd[0];
	Vector3 darp = Vector3(1.0f, 2.0f, 3.0f);
	test = darp[2];
	Vector4 hurp = Vector4(10.0f, 20.0f, 30.0f, 40.0f);
	test = hurp[3];

	float inner = 1.0f;
	float outer = inner * 2;
	//GetGraphics()->SetSpecialCircle(inner, outer, Vector2(0, 0));
	float hmData[65 * 65];
	for(int i = 0; i < vertSize * vertSize; i++)
	{
		/*hmData[0] = 0.0f;
		hmData[1] = 0.0f;
		hmData[2] = 2.0f;
		hmData[3] = 0.0f;*/
		/*if(i % vertSize > 16 && i % vertSize < 48)
		{
			hmData[i] = 10.0f;
		}
		else
		{
			hmData[i] = 0.0f;
		}*/
		hmData[i] = 1.0f;
	}

	hmData[2033] = 15.0f;
	hmData[2034] = 15.0f;
	hmData[2035] = 15.0f;
	hmData[2036] = 15.0f;
	hmData[2097] = 15.0f;
	hmData[2098] = 15.0f;
	hmData[2099] = 15.0f;
	hmData[2100] = 15.0f;

	iT->SetHeightMap(hmData);
	fileNames[0] = "Media/01_v02-Moss.png";
	fileNames[1] = "Media/01_v02-Moss.png";
	fileNames[2] = "Media/01_v02-Moss.png";
	fileNames[3] = "Media/01_v02-Moss.png";
	fileNames[4] = "Media/01_v02-Moss.png";
	fileNames[5] = "Media/01_v02-Moss.png";
	fileNames[6] = "Media/01_v02-Moss.png";
	fileNames[7] = "Media/01_v02-Moss.png";
	iT->SetTextures(fileNames);
	float normals2[64 * 64 * 3]; //vertSize = 64;
	for(int i = 0; i < 64 * 64 * 3; i+=3)
	{
		normals2[i] = 0.0f;	//x
		normals2[i + 1] = 1.0f;	//y
		normals2[i + 2] = 0.0f;	//z
	}
	iT->SetNormals(normals2);
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
	sizes = new unsigned int[2];
	sizes[0] = size;
	sizes[1] = size;
	testData = new float*[nrOfBlendMaps]; 
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



	fileNames[0] = "Media/01_v02-Moss.png";
	fileNames[1] = "Media/01_v02-Moss.png";
	fileNames[2] = "Media/01_v02-Moss.png";
	fileNames[3] = "Media/01_v02-Moss.png";
	fileNames[4] = "Media/01_v02-Moss.png";
	fileNames[5] = "Media/01_v02-Moss.png";
	fileNames[6] = "Media/01_v02-Moss.png";
	fileNames[7] = "Media/01_v02-Moss.png";

	float normals[64 * 64 * 3]; //vertSize = 64;
	/*for(int i = 0; i < 64 * 64 * 3; i+=3)
	{
		normals[i] = 0.57f;		//x
		normals[i + 1] = 0.57f;	//y
		normals[i + 2] = 0.57f;	//z
	}
	for(int i = 0; i < 64 * 64 * 3; i+=3)
	{
		normals[i] = 1.0f;		//x
		normals[i + 1] = 1.0f;	//y
		normals[i + 2] = 1.0f;	//z
	}*/
	iT2->SetHeightMap(hmData);
	iT2->SetTextures(fileNames);
	iT2->SetBlendMaps(nrOfBlendMaps, sizes, testData);
	iT2->SetNormals(normals2);

	/*int klerp = 3;
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
	}*/

	//iMesh* bush = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(30, 10, 30));
	//bush->Scale(1.0f * 0.05f);




	//CASCADED SHADOWMAP:
	//Transparancy:
	//bushCSMTest = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(10, 2, 0));
	//bushCSMTest->Scale(1.0f * 0.05f);
	//iMesh* flagCSMTest = GetGraphics()->CreateMesh("Media/FlagBlue.ani", Vector3(12, 2, 0));

	//General shadowing:
	/*iMesh* bushScaleBugTest = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(0, 0, 0));
	iMesh* scaleBugTest = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(5, 0, 0));
	iMesh* bushScaleBugTest2 = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(2, 0, 0));
	iMesh* scaleBugTest2 = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(4, 0, 0));
	
	bushScaleBugTest->Scale(1.0f * 0.05f);
	scaleBugTest->Scale(1.0f * 0.05f);
	bushScaleBugTest2->Scale(1.0f * 0.05f);
	scaleBugTest2->Scale(1.0f * 0.25f);
*/	/*
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

	//AI grid test
	
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
	
	cameraPoint = Vector3(0.0f, 5.0f, 5.0f);
	cameraLookAt = Vector3(100.0f, 0.0f, 0.0f);
	cameraPoint2 = Vector3(25.0f, 5.0f, 25.0f);
	cameraLookAt2 = Vector3(100.0f, 0.0f, 100.0f);
	cameraPoint3 = Vector3(-10.0f, 2.0f, 47.5f);
	cameraLookAt3 = Vector3(100.0f, 2.0f, 47.5f);
	cameraPoint4 = Vector3(-50.0f, 20.0f, 150.0f);

	
	lll = GetGraphics()->CreateLight(Vector3(0,2,0));
	lll->SetLookAt(Vector3(1, 0, 1));
	lll->SetIntensity(1.0f);
	mmm = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0,2,0));


	float navArrowsScale = 0.25f;
	iMesh* navArrowX = GetGraphics()->CreateMesh("Media/RedArrow.obj", Vector3(0, 2, 0));
	iMesh* navArrowZ = GetGraphics()->CreateMesh("Media/BlueArrow.obj", Vector3(-7.5f * navArrowsScale, 2, 7.5f * navArrowsScale));
	iMesh* navArrowSpawn = GetGraphics()->CreateMesh("Media/BlackArrow.obj", Vector3(0, 2, 0));
	navArrowX->RotateAxis(Vector3(0, 0, 1), 3.14159265359 * 0.5f); //Point down x-axis
	navArrowZ->RotateAxis(Vector3(1, 0, 0), -3.14159265359 * 0.5f); //Point down z-axis
	navArrowSpawn->RotateAxis(Vector3(0, 0, 1), 3.14159265359 * 0.5f); //Point down x-axis
	navArrowX->Scale(navArrowsScale);
	navArrowZ->Scale(navArrowsScale);
	navArrowSpawn->Scale(navArrowsScale);


	//GRASS
	/*for(int i = 0; i < 25; i++)
	{
		for(int j = 0; j < 25; j++)
		{
			iMesh* tmp = GetGraphics()->CreateMesh("Media/ShrubberyGrass_01_v01.obj", Vector3(i - 12.5f, 0, j - 12.5f));
			tmp->SetScale(0.05f);
		}
	}*/
	//BILLBOARD && DONTRENDER & INVISIBILITY
	iMesh* temp = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(0, 0, 0));
	temp->SetScale(0.05f);
	temp->UseInvisibilityEffect(true);
	//temp->DontRender(true);

	//iMesh* treeWithBillboard = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(-10, 0, 0), "Media/TreeBillboard.png", 25.5f);
	//treeWithBillboard->SetScale(0.05f);
	//treeWithBillboard->DontRender(true);

	//iMesh* flagWithBillboard = GetGraphics()->CreateMesh("Media/FlagBlue.ani", Vector3(-15, 0, -5), "Media/TreeBillboard.png", 25.5f);

	/*
	int klerp23 = 10;
	for(int i = 0; i < klerp23; i++)
	{
		iTerrain* itt = GetGraphics()->CreateTerrain(Vector3(i * 10, 0, 0), Vector3(10, 10, 10), 2);
		iTerrain* itt2 = GetGraphics()->CreateTerrain(Vector3(i * 10, 0, 10), Vector3(10, 10, 10), 2);
		itt->SetTextures(fileNames);
		itt2->SetTextures(fileNames);
	}
	for(int i = 0; i < klerp23 * 2; i++)
	{
		iMesh* treeWithBillboard = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(i * 5, 0, 0), "Media/TreeBillboard.png", 500.0f);
		treeWithBillboard->SetScale(0.05f);
		treeWithBillboard->GetBillboard()->SetPosition(Vector3(i * 5, 5, 0));
		treeWithBillboard->GetBillboard()->SetSize(Vector2(10, 10));
	}*/

	//Scale animation
	//iMesh* viking = GetGraphics()->CreateMesh("Media/Scale.ani", Vector3(-20,0,0));
	//viking->SetScale(0.05f);

	//Unload object data resource test.'
	unloadTestAni = GetGraphics()->CreateMesh("Media/Fern_02.ani", Vector3(5, 5, -5));
	unloadTestAni->SetScale(0.5f);

	//new crasch testing
	//iMesh* aniGrassPlantCrasch = GetGraphics()->CreateMesh("Media/GrassPlant_01.ani", Vector3(5, -5, -5));


	//iMesh* tklerp = GetGraphics()->CreateMesh("Media/scale.obj", Vector3(30, -300, 30));

	//createTerrainIndexBufferCraschText = GetGraphics()->CreateTerrain(Vector3(-25, 0, 0), Vector3(25, 0, 25), 32);

	//Instancing benchmark
	iTerrain* djurp;
	for(int i = 0; i < 2; i++)
	{
		djurp = GetGraphics()->CreateTerrain(Vector3(i * 25, 0, 50), Vector3(testSize, 0.0f, testSize), vertSize);
		djurp->SetTextures(fileNames);
		djurp->SetBlendMaps(nrOfBlendMaps, sizes, testData);
	}
	string billboardFile = "Media/TreeBillboard.png";
	for(int i = 0; i < 10; i++)
	{
		//Billboards
		if(i == 4)
		{
			billboardFile = "Media/BallTexture.png";
		}
		if(i == 6)
		{
			billboardFile = "Media/TreeBillboard.png";
		}
		if(i == 8)
		{
			billboardFile = "Media/StoneItem_01_v01.png";
		}
		iMesh* treeWithBillboard = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(i * 5, 0, 47.5f), billboardFile.c_str(), 0.5f);
		treeWithBillboard->SetScale((0.061f));
		iMesh* fernWithBillboard = GetGraphics()->CreateMesh("Media/Fern_02.ani", Vector3(i * 5, 0, 42.5f), billboardFile.c_str(), 55.5f);
		fernWithBillboard->SetScale((0.15f));

		//Meshes
		iMesh* treeWithWOBB = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(i * 5, 0, 55));
		treeWithWOBB->SetScale((0.041f));
	}
	//iBBMemLeak = GetGraphics()->CreateBillboard(Vector3(0, 20, 0), Vector2(10, 10), "Media/TreeBillboard.png");

	//Shadow fail test with tree animation:
	iMesh* shadowFail = GetGraphics()->CreateMesh("Media/Tree_02.ani", Vector3(-10, 0, -7.5f));
	shadowFail->SetScale(0.05f);
	//shadowFail = GetGraphics()->CreateMesh("Media/GrassPlant_01.ani", Vector3(-11, 0, 0));
	//shadowFail->SetScale(0.05f);
	shadowFail = GetGraphics()->CreateMesh("Media/Fern_02.ani", Vector3(-10, 0, 0), "Media/TreeBillboard.png", 25.0f);
	shadowFail->SetScale(0.5f);
	//shadowFail = GetGraphics()->CreateMesh("Media/Scale.ani", Vector3(-13, 0, 0));
	//shadowFail->SetScale(0.05f);
	shadowFail = GetGraphics()->CreateAnimatedMesh("Media/FlagBlue.ani", Vector3(-10, 0, 7.5f));


	//Bottleneck test
	billboardFile = "Media/TreeBillboard.png";
	for(int i = 0; i < 10; i++)
	{
		//Billboards
		if(i == 4)
		{
			billboardFile = "Media/BallTexture.png";
		}
		if(i == 6)
		{
			billboardFile = "Media/TreeBillboard.png";
		}
		if(i == 8)
		{
			billboardFile = "Media/StoneItem_01_v01.png";
		}
		iMesh* treeWithBillboard = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(i * -5 -50, 0, 100.0f), billboardFile.c_str(), 0.5f);
		treeWithBillboard->SetScale((0.061f));
		iMesh* fernWithBillboard = GetGraphics()->CreateMesh("Media/Fern_02.ani", Vector3(i * -5 -50, 0, 0.0f));
		fernWithBillboard->SetScale((0.15f));
		iMesh* treeWithWOBB = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(i * -5 -50, 0, -100.0f));
		treeWithWOBB->SetScale((0.041f));
	}
	
}

void TillmanTest::RunTest(float diff)
{
		/*Vector3 spawnPoint = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 camPosOffset = GetGraphics()->GetCamera()->GetPosition();
		Vector3 camForwardoffset = GetGraphics()->GetCamera()->GetForward() * 2.0f;
		Vector3 arrowOffset = Vector3(-7.5f * navArrowsScale, 0, 7.5f * navArrowsScale);
		Vector3 cameraOffset = Vector3(7.5f * navArrowsScale, -1.0f, 0.0f); 
		navArrowX->SetPosition(camPosOffset + camForwardoffset + cameraOffset);
		navArrowZ->SetPosition(camPosOffset + camForwardoffset + cameraOffset + arrowOffset);
		
		navArrowSpawn->SetPosition(camPosOffset + camForwardoffset + cameraOffset);
		Vector3 vecBetweenSpawnAndArrow = spawnPoint - navArrowSpawn->GetPosition();
		vecBetweenSpawnAndArrow.Normalize();
		navArrowSpawn->ResetRotation();
		Vector3 vec = Vector3(0, -1, 0);
		Vector3 around = vec.GetCrossProduct(vecBetweenSpawnAndArrow);
		float angle = acos(vec.GetDotProduct(vecBetweenSpawnAndArrow) / (vec.GetLength() * vecBetweenSpawnAndArrow.GetLength()));
		navArrowSpawn->RotateAxis(around, angle);
		*/



	CollisionData cd = GetGraphics()->GetPhysicsEngine()->GetSpecialCollisionRayTerrain(GetGraphics()->GetCamera()->GetPosition(),
		GetGraphics()->GetCamera()->Get3DPickingRay(), iT2, testSize / (vertSize));	
	if(cd.collision)
		mmm->SetPosition(Vector3(cd.posx, cd.posy, cd.posz));
	else
	{
		mmm->SetPosition(Vector3(0, 0, 0));
	}

	static bool fesd = true;
	if(GetGraphics()->GetKeyListener()->IsClicked(1))
	{		
		if(fesd)
		{
			Vector3 pos = mmm->GetPosition();
			iDecal* dec = GetGraphics()->CreateDecal(pos, "Media/BloodTexture.png", Vector3(0, -1, 0), Vector3(1, 0, 0));
			dec->SetOpacity(0.5f);
			fesd = false;
		}
	}
	else
		fesd = true;

	//CASCADED SHADOW MAPPING
	static float debugCSMScale = 1.0f;
	float normals[64 * 64 * 3]; //vertSize = 64;
	for(int i = 0; i < 64 * 64 * 3; i+=3)
	{
		normals[i] = 0.577350259f;		//x
		normals[i + 1] = 0.577350259f;	//y
		normals[i + 2] = 0.577350259f;	//z
	}
	float hmData[65 * 65];
	for(int i = 0; i < 64 * 64; i++)
	{
		/*hmData[0] = 0.0f;
		hmData[1] = 0.0f;
		hmData[2] = 2.0f;
		hmData[3] = 0.0f;*/
		/*if(i % vertSize > 16 && i % vertSize < 48)
		{
			hmData[i] = 10.0f;
		}
		else
		{
			hmData[i] = 0.0f;
		}*/
		hmData[i] = 1.0f;
	}
	static bool oncee = false;
	if(GetGraphics()->GetKeyListener()->IsPressed('Q'))
	{
		GetGraphics()->ReloadShaders(8); //shadow map animated
		//iT2->SetNormals(normals);
		//iT2->SetHeightMap(hmData);
		//debugCSMScale += diff * 0.001f;
		//iT->SetAIGridThickness(0.0005f);
		//iT->UseAIMap(true);*/
		//GetGraphics()->DeleteBillboard(iBBMemLeak);
		/*
		fileNames[0] = "Media/TerrainTexture.png";
		fileNames[1] = "Media/TerrainTexture.png";
		fileNames[2] = "Media/TerrainTexture.png";
		fileNames[3] = "Media/TerrainTexture.png";
		fileNames[4] = "Media/BallTexture.png";
		fileNames[5] = "Media/BallTexture.png";
		fileNames[6] = "Media/BallTexture.png";
		fileNames[7] = "Media/BallTexture.png";
		iT->SetTextures(fileNames);
		int nrOfBlendMaps = 2;
		unsigned int size = 32;
		sizes[0] = size;
		sizes[1] = size;
		unsigned int channels = 4;
		for(unsigned int i = 0; i < sizes[0]*sizes[0]; i++)
		{
			testData[0][ i * channels + 0 ] = 0.0f;
			testData[0][ i * channels + 1 ] = 0.0f;
			testData[0][ i * channels + 2 ] = 0.0f;
			testData[0][ i * channels + 3 ] = 0.0f;
		}
		for(unsigned int i = 0; i < sizes[1]*sizes[1]; i++)
		{
			testData[1][ i * channels + 0 ] = 1.0f;
			testData[1][ i * channels + 1 ] = 1.0f;
			testData[1][ i * channels + 2 ] = 1.0f;
			testData[1][ i * channels + 3 ] = 1.0f;
		}
		iT->SetBlendMaps(nrOfBlendMaps, sizes, testData);
		/*if(!oncee)
		{	
			iMesh* loadFromFileTest = NULL;
			iT->SetAIGrid(64, (void*)&aiData);
			oncee = true;
			createTerrainIndexBufferCraschText = GetGraphics()->CreateTerrain(Vector3(-25, 0, 0), Vector3(25, 0, 25), 64);
			for(int i = 0; i < 1; i++)
			{
				iMesh* loadFromFileTest = GetGraphics()->CreateMesh("Media/Bush_01_v04_r.obj", Vector3(-3000, 10, 30));

				//createTerrainIndexBufferCraschText = GetGraphics()->CreateTerrain(Vector3(i * -25.0f, 0, 0), Vector3(25, 0, 25), 64);
				//testtts = GetGraphics()->CreateMesh()
			}
		}*/
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('E'))
	{
		//GetGraphics()->ReloadShaders(22); //deffered geometry instanced
		GetGraphics()->ReloadShaders(23); //deffered animated geometry instanced
		//GetGraphics()->ReloadShaders(11); //deferred lightning
		//GetGraphics()->ReloadShaders(20); //shadow map billboard instanced
		/*
		debugCSMScale -= diff * 0.001f;
		fileNames[0] = "Media/TerrainTexture.png";
		fileNames[1] = "Media/TerrainTexture.png";
		fileNames[2] = "Media/TerrainTexture.png";
		fileNames[3] = "Media/TerrainTexture.png";
		fileNames[4] = "Media/TerrainTexture.png";
		fileNames[5] = "Media/TerrainTexture.png";
		fileNames[6] = "Media/TerrainTexture.png";
		fileNames[7] = "Media/TerrainTexture.png";
		iT->SetTextures(fileNames);
		//GetGraphics()->DeleteMesh(flagCSMTest);
		//GetGraphics()->DeleteMesh(bushCSMTest);
		iT->UseAIMap(false);
		GetGraphics()->DeleteTerrain(createTerrainIndexBufferCraschText);
		oncee = false;*/
	}

		/*for(int i = 0; i < ttte * nrOfFrustumSlices; i++)
		{
			//debugCSMPoints[i]->SetScale(debugCSMScale);
		}*/

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
	//Camera reset/teleport
	if(GetGraphics()->GetKeyListener()->IsPressed('B'))
	{
		GetGraphics()->GetCamera()->SetPosition(cameraPoint3);
		GetGraphics()->GetCamera()->LookAt(cameraLookAt3);
	}
	//Camera reset/teleport
	if(GetGraphics()->GetKeyListener()->IsPressed('N'))
	{
		GetGraphics()->GetCamera()->SetPosition(cameraPoint4);
		GetGraphics()->GetCamera()->LookAt(cameraLookAt3);
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

	static bool oncett = false;
	if(!oncett)
	{
		GetGraphics()->DeleteMesh(unloadTestAni);
		oncett = true;
	}
}

void TillmanTest::PostTest()
{
	//delete[] iTs;
	delete [] sizes;
	delete testData[0];
	delete testData[1];
	delete [] testData;
	//delete[] resourceFileNames;

	GetGraphics()->DeleteTerrain(iT);
	GetGraphics()->DeleteTerrain(iT2);
	GetGraphics()->DeleteImage(iM);
	GetGraphics()->DeleteImage(iM2);
	GetGraphics()->DeleteImage(iM3);
	GetGraphics()->DeleteImage(iM4);
	GetGraphics()->DeleteImage(iM5);
}