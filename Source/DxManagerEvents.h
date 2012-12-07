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
	float width;
	float height;

public:
	ResizeEvent(string msg, float width, float height) : RendererEvent(msg)
	{
		this->width = width;
		this->height = height;
	}
	virtual ~ResizeEvent()
	{

	}
	float GetWidth() { return this->width; }
	float GetHeight() { return this->height; }

};