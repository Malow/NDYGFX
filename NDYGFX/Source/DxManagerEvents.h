#pragma once

#include "Process.h"

/* Process events for adding to rendering */

class RendererEvent : public MaloW::ProcessEvent
{
protected:
	bool Adding;
	bool deleteSelf;

public:
	RendererEvent(bool adding = true) 
	{ 
		this->Adding = adding; 
		this->deleteSelf = true;
	}
	virtual ~RendererEvent() 
	{ 
	}
	bool IsAdding() { return this->Adding; }
};

class TerrainEvent : public RendererEvent
{
private:
	Terrain* terrain;

public:
	TerrainEvent(bool adding, Terrain* terrain) : RendererEvent(adding)
	{
		this->terrain = terrain;
	}
	virtual ~TerrainEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->terrain) delete this->terrain; this->terrain = NULL;
		}
	}

	Terrain* GetTerrain() { this->deleteSelf = false; return this->terrain; }
};


class StaticMeshEvent : public RendererEvent
{
private:
	StaticMesh* mesh;

public:
	StaticMeshEvent(bool adding, StaticMesh* mesh) : RendererEvent(adding)
	{
		this->mesh = mesh; 
	}
	virtual ~StaticMeshEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->mesh)
				delete this->mesh;
		}
	}

	StaticMesh* GetStaticMesh() { this->deleteSelf = false; return this->mesh; }
};

class AnimatedMeshEvent : public RendererEvent
{
private:
	AnimatedMesh* ani;

public:
	AnimatedMeshEvent(bool adding, AnimatedMesh* ani) : RendererEvent(adding)
	{
		this->ani = ani;
	}
	virtual ~AnimatedMeshEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->ani)
				delete this->ani;
		}
	}

	AnimatedMesh* GetAnimatedMesh() { this->deleteSelf = false; return this->ani; }
};

class LightEvent : public RendererEvent
{
private:
	Light* light;
	bool useShadows;

public:
	LightEvent(bool adding, Light* light, bool useShadows) : RendererEvent(adding)
	{
		this->light = light;
		this->useShadows = useShadows;
	}
	virtual ~LightEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->light)
				delete this->light;
		}
	}
	Light* GetLight() { this->deleteSelf = false; return this->light; }
	bool IsUsingShadows() { return this->useShadows; }
};

class ImageEvent : public RendererEvent
{
private:
	Image* img;

public:
	ImageEvent(bool adding, Image* img) : RendererEvent(adding)
	{
		this->img = img;
	}
	virtual ~ImageEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->img)
				delete this->img;
		}
	}
	Image* GetImage() { this->deleteSelf = false; return this->img; }
};

class BillboardEvent : public RendererEvent
{
private:
	Billboard* billboard;

public:
	BillboardEvent(bool adding, Billboard* billboard) : RendererEvent(adding)
	{
		this->billboard = billboard;
	}
	virtual ~BillboardEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->billboard)
				delete this->billboard;
		}
	}
	Billboard* GetBillboard() { this->deleteSelf = false; return this->billboard; }
};

class BillboardCollectionEvent : public RendererEvent
{
private:
	BillboardCollection* billboardCollection;

public:
	BillboardCollectionEvent(bool adding, BillboardCollection* billboardCollection) : RendererEvent(adding)
	{
		this->billboardCollection = billboardCollection;
	}
	virtual ~BillboardCollectionEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->billboardCollection)
				delete this->billboardCollection;
		}
	}
	BillboardCollection* GetBillboardCollection() { this->deleteSelf = false; return this->billboardCollection; }
};


class TextEvent : public RendererEvent
{
private:
	Text* txt;

public:
	TextEvent(bool adding, Text* txt) : RendererEvent(adding)
	{
		this->txt = txt;
	}
	virtual ~TextEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->txt)
				delete this->txt;
		}
	}
	Text* GetText() { this->deleteSelf = false; return this->txt; }
};

class ResizeEvent : public RendererEvent
{
private:
	unsigned int width;
	unsigned int height;

public:
	ResizeEvent(bool adding, unsigned int width, unsigned int height) : RendererEvent(adding)
	{
		this->width = width;
		this->height = height;
	}
	virtual ~ResizeEvent()
	{

	}
	unsigned int GetWidth() { return this->width; }
	unsigned int GetHeight() { return this->height; }
};

class SetCameraEvent : public RendererEvent
{
private:
	Camera* cam;

public:
	SetCameraEvent(bool adding, Camera* cam) : RendererEvent(adding)
	{
		this->cam = cam;
	}
	virtual ~SetCameraEvent()
	{
		if(this->deleteSelf)
		{
			if(this->cam)
				delete this->cam;
		}
	}
	Camera* GetCamera() { this->deleteSelf = false; return this->cam; }
};

class WaterPlaneEvent : public RendererEvent
{
private:
	WaterPlane* wp;

public:
	WaterPlaneEvent(bool adding, WaterPlane* wp) : RendererEvent(adding)
	{
		this->wp = wp;
	}
	virtual ~WaterPlaneEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->wp)
				delete this->wp;
		}
	}
	WaterPlane* GetWaterPlane() { this->deleteSelf = false; return this->wp; }
};


class FBXEvent : public RendererEvent
{
private:
	FBXMesh* mesh;
	AnimatedMesh* ani;

public:
	FBXEvent(bool adding, FBXMesh* mesh) : RendererEvent(adding)
	{
		this->mesh = mesh; 
	}
	virtual ~FBXEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->mesh)
				delete this->mesh;
		}
	}

	FBXMesh* GetFBXMesh() { this->deleteSelf = false; return this->mesh; }
};


class ChangeShadowQualityEvent : public RendererEvent
{
private:
	int qual;

public:
	ChangeShadowQualityEvent(bool adding, int qual) : RendererEvent(adding)
	{
		this->qual = qual; 
	}
	virtual ~ChangeShadowQualityEvent() 
	{

	}

	int GetQuality() { return this->qual; }
};


class ReloadShaderEvent : public RendererEvent
{
private:
	int shad;

public:
	ReloadShaderEvent(bool adding, int shader) : RendererEvent(adding)
	{
		this->shad = shader; 
	}
	virtual ~ReloadShaderEvent() 
	{

	}

	int GetShader() { return this->shad; }
};


class DecalEvent : public RendererEvent
{
private:
	Decal* dec;

public:
	DecalEvent(bool adding, Decal* dec) : RendererEvent(adding)
	{
		this->dec = dec; 
	}
	virtual ~DecalEvent() 
	{
		if(this->deleteSelf && this->Adding)
		{
			if(this->dec)
				delete this->dec;
		}
	}

	Decal* GetDecal() { this->deleteSelf = false; return this->dec; }
};



