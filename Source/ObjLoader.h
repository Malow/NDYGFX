#ifndef OBJLOADER
#define OBJLOADER

#include <string>
#include <fstream>
#include <iostream>
#include "DirectX.h"
#include "Array.h"

struct MaterialData
{
	std::string name;
	std::string texture;
	int illum;
	D3DXVECTOR3 kd;
	D3DXVECTOR3 ka;
	D3DXVECTOR3 tf;
	D3DXVECTOR3 ks;
	
	float ni;

	MaterialData()
	{
		name = "";
		illum = 0;
		kd = D3DXVECTOR3(0, 0, 0);
		ka = D3DXVECTOR3(0, 0, 0);
		tf = D3DXVECTOR3(0, 0, 0);
		texture = "";
		ks = D3DXVECTOR3(0, 0, 0);
		ni = 0.0f;
	}
};

struct FaceData
{
	int data[3][3];
	// NOTE: string in material is slow?
	std::string material;
};

class ObjData
{
public:
	MaloW::Array<D3DXVECTOR3>* vertspos;
	MaloW::Array<D3DXVECTOR2>* textcoords;
	MaloW::Array<D3DXVECTOR3>* vertsnorms;
	MaloW::Array<FaceData>* faces;
	MaloW::Array<MaterialData>* mats;

	ObjData() 
	{ 
		vertspos = new MaloW::Array<D3DXVECTOR3>();
		textcoords = new MaloW::Array<D3DXVECTOR2>();
		vertsnorms = new MaloW::Array<D3DXVECTOR3>();
		faces = new MaloW::Array<FaceData>();
		mats = new MaloW::Array<MaterialData>();
	}

	virtual ~ObjData()
	{
		
		if(this->vertspos)
			delete this->vertspos;

		if(this->textcoords)
			delete this->textcoords;

		if(this->vertsnorms)
			delete this->vertsnorms;

		if(this->faces)
			delete this->faces;

		if(this->mats)
			delete this->mats;
			
	}
};

class ObjLoader
{
private:
	D3DXVECTOR3 GetVertPosData(std::string line);
	D3DXVECTOR2 GetTextCoordsData(std::string line);
	D3DXVECTOR3 GetVertsNormsData(std::string line);
	FaceData GetFaceData(std::string line);
	void trianglulate(std::string& filename);

public:
	ObjLoader() { }
	virtual ~ObjLoader() { }

	ObjData* LoadObjFile(std::string filepath);
	void ReadFromBinaryFile(ObjData* returndata, ifstream& binfile);
	void CreateBinaryFile(std::string filename, std::string cacheFolder, ObjData* returndata);

};


#endif