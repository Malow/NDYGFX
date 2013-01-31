//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Buffer-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h. Buffer.h
//--------------------------------------------------------------------------------------------------

#pragma once

#include "ReferenceCounted.h"
#include "Buffer.h"
#include <string>
using namespace std;

class BufferResource : public ReferenceCounted
{
	private:
		string	zName; //Used as identification.
		Buffer*	zBuffer;

	private:
		virtual ~BufferResource();

	public:
		BufferResource();
		BufferResource(string ID, Buffer* buffer);

		const string& GetName() const { return this->zName; }
		Buffer* GetBufferPointer() const { return this->zBuffer; }
		void SetBufferPointer(Buffer* pointer) { this->zBuffer = pointer; }

};
