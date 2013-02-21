#pragma once

#pragma warning( push )
#pragma warning( disable : 4996 )	// Ignore Deprecation Warnings from SDK

// Include
#include <fbxsdk.h>

// FBX Library Dependencies
#ifdef _DEBUG
#pragma comment(lib, "fbxsdk-2014.1-mdd.lib")
#else
#pragma comment(lib, "fbxsdk-2014.1-md.lib")
#endif // _DEBUG

#pragma warning( pop )