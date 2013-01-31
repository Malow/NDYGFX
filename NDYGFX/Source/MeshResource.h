//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Mesh(strips)-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h. 
//--------------------------------------------------------------------------------------------------

#pragma once

#include "ReferenceCounted.h"
#include "MaloW.h"
#include "Array.h"
#include "MeshStrip.h"
#include <string>
using namespace std;

class MeshResource : public ReferenceCounted
{
	private:
		string						zFilePath;
		MaloW::Array<MeshStrip*>*	zMesh;

	private:
		virtual ~MeshResource();

	public:
		MeshResource();
		MeshResource(string filePath, MaloW::Array<MeshStrip*>* mesh);

		string GetName() const { return this->zFilePath; }
		MaloW::Array<MeshStrip*>* GetMeshPointer() const { return this->zMesh; }
		void SetMeshPointer(MaloW::Array<MeshStrip*>* pointer) { this->zMesh = pointer; }

};
