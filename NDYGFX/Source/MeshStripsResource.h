//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	MeshStrips-resource class. Used by the ResourceManager.
//  Requirements: ReferenceCounted.h. 
//--------------------------------------------------------------------------------------------------

#pragma once

#include "ReferenceCounted.h"
#include "MaloW.h"
#include "Array.h"
#include "MeshStrip.h"
#include "Vertex.h"
#include "Material.h"
#include <string>
using namespace std;

class MeshStripsResource : public ReferenceCounted
{
	private:
		string						zFilePath;
		MaloW::Array<MeshStrip*>*	zMesh;

	private:
		virtual ~MeshStripsResource();

	public:
		MeshStripsResource();
		MeshStripsResource(string filePath, MaloW::Array<MeshStrip*>* mesh);
		MeshStripsResource(const MeshStripsResource& origObj);

		string GetName() const { return this->zFilePath; }
		MaloW::Array<MeshStrip*>* GetMeshStripsPointer() const { return this->zMesh; }
		void SetMeshStripsPointer(MaloW::Array<MeshStrip*>* pointer) { this->zMesh = pointer; }

};
