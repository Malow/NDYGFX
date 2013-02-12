#pragma once

#include "Process.h"

/* Process events for adding to rendering */

class RendererEvent : public MaloW::ProcessEvent
{
protected:
	string message;
	bool deleteSelf;

public:
	RendererEvent(string message = "") 
	{ 
		this->message = message; 
		this->deleteSelf = true;
	}
	virtual ~RendererEvent() 
	{ 
	}
	string getMessage() { return this->message; }
};

class TerrainEvent : public RendererEvent
{
private:
	Terrain* terrain;

public:
	TerrainEvent(string message, Terrain* terrain) : RendererEvent(message)
	{
		this->terrain = terrain;
	}
	virtual ~TerrainEvent() 
	{
		if(this->deleteSelf && this->message.substr(0, 6) != "Delete")
		{
			if(this->terrain) delete this->terrain; this->terrain = NULL;
		}
	}

	Terrain* GetTerrain() { this->deleteSelf = false; return this->terrain; }
};


class MeshEvent : public RendererEvent
{
private:
	StaticMesh* mesh;
	AnimatedMesh* ani;

public:
	MeshEvent(string message, StaticMesh* mesh, AnimatedMesh* ani) : RendererEvent(message)
	{
		this->mesh = mesh; 
		this->ani = ani;
	}
	virtual ~MeshEvent() 
	{
		if(this->deleteSelf && this->message.substr(0, 6) != "Delete")
		{
			if(this->mesh)
				delete this->mesh;
			if(this->ani)
				delete this->ani;
		}
	}

	StaticMesh* GetStaticMesh() { this->deleteSelf = false; return this->mesh; }
	AnimatedMesh* GetAnimatedMesh() { this->deleteSelf = false; return this->ani; }
};

class LightEvent : public RendererEvent
{
private:
	Light* light;

public:
	LightEvent(string msg, Light* light) : RendererEvent(msg)
	{
		this->light = light;
	}
	virtual ~LightEvent() 
	{
		if(this->deleteSelf && this->message.substr(0, 6) != "Delete")
		{
			if(this->light)
				delete this->light;
		}
	}
	Light* GetLight() { this->deleteSelf = false; return this->light; }
};

class ImageEvent : public RendererEvent
{
private:
	Image* img;

public:
	ImageEvent(string msg, Image* img) : RendererEvent(msg)
	{
		this->img = img;
	}
	virtual ~ImageEvent() 
	{
		if(this->deleteSelf && this->message.substr(0, 6) != "Delete")
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
	BillboardEvent(string msg, Billboard* billboard) : RendererEvent(msg)
	{
		this->billboard = billboard;
	}
	virtual ~BillboardEvent() 
	{
		if(this->deleteSelf && this->message.substr(0, 6) != "Delete")
		{
			if(this->billboard)
				delete this->billboard;
		}
	}
	Billboard* GetBillboard() { this->deleteSelf = false; return this->billboard; }
};

class TextEvent : public RendererEvent
{
private:
	Text* txt;

public:
	TextEvent(string msg, Text* txt) : RendererEvent(msg)
	{
		this->txt = txt;
	}
	virtual ~TextEvent() 
	{
		if(this->deleteSelf && this->message.substr(0, 6) != "Delete")
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
	ResizeEvent(string msg, unsigned int width, unsigned int height) : RendererEvent(msg)
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
	SetCameraEvent(string msg, Camera* cam) : RendererEvent(msg)
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
	WaterPlaneEvent(string msg, WaterPlane* wp) : RendererEvent(msg)
	{
		this->wp = wp;
	}
	virtual ~WaterPlaneEvent() 
	{
		if(this->deleteSelf && this->message.substr(0, 6) != "Delete")
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
	FBXEvent(string message, FBXMesh* mesh) : RendererEvent(message)
	{
		this->mesh = mesh; 
	}
	virtual ~FBXEvent() 
	{
		if(this->deleteSelf && this->message.substr(0, 6) != "Delete")
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
	ChangeShadowQualityEvent(string message, int qual) : RendererEvent(message)
	{
		this->qual = qual; 
	}
	virtual ~ChangeShadowQualityEvent() 
	{

	}

	int GetQuality() { return this->qual; }
};