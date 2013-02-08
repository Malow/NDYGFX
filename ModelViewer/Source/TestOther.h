#pragma once

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"

class OtherTest
{
private:
	iMesh* arrow;
	Vector3 arrowDir;
	iMesh* ball;
	iMesh* secModel;
	iWaterPlane* wp;

public:
	OtherTest() {};
	virtual ~OtherTest() {};

	void PreTest();
	void RunTest(float diff);
	void PostTest();
};

void OtherTest::PreTest()
{

}

void OtherTest::RunTest(float diff)
{

}
void OtherTest::PostTest()
{

}