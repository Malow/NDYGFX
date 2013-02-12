#pragma once

class TestCase
{
public:
	TestCase() {};
	virtual ~TestCase() {};

	virtual void PreTest() {};
	virtual void RunTest(float diff) {};
	virtual void PostTest() {};
};