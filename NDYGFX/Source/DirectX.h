#pragma once

// EDIT: 2012-12-18, Added Warning Ignore

#pragma warning ( push ) // Ignore macro redefinition warnings
#pragma warning ( disable : 4005 )

// DirectX
#include <D3D11.h>
#include <dxgi.h>

// Extra Libraries
#include <D3DX10.h>
#include <DirectXMath.h>
#include <D3DX11.h>
#include <D3Dx11effect.h>
#include <D3DCompiler.h>

#pragma warning ( pop )

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxerr.lib")

#ifdef _DEBUG
	/*! Graphic debug libs */
	#pragma comment(lib, "d3dx11d.lib")
	#pragma comment(lib, "Effects11D.lib")
	#pragma comment(lib, "d3dx10d.lib")
#else
	/*! Graphic libs */
	#pragma comment(lib, "d3dx11.lib")
	#pragma comment(lib, "Effects11.lib")
	#pragma comment(lib, "d3dx10.lib")
#endif

// Safe Release
template<typename T>
inline void SAFE_RELEASE( T& a )
{
	if ( a ) a->Release(), a = 0;
}

// Safe Delete
template<typename T>
inline void SAFE_DELETE( T& a )
{
	if ( a ) delete a; a = 0;
}