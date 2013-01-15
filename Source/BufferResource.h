//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Buffer-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h. Buffer.h
//--------------------------------------------------------------------------------------------------

#pragma once

#include "ReferenceCounted.h"
#include "DirectX.h"
#include <d3dx11.h>
#include "Buffer.h"
#include <string>
using namespace std;

class BufferResource : public ReferenceCounted
{
	private:
		string	zFilePath;
		Buffer*	zBuffer;

	private:
		virtual ~BufferResource();

	public:
		BufferResource();
		BufferResource(string filePath, Buffer* buffer);

		const string& GetName() const { return this->zFilePath; }
		Buffer* GetSRVPointer() const { return this->zBuffer; }
		void SetSRVPointer(Buffer* pointer) { this->zBuffer = pointer; }

};
