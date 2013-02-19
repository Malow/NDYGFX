#include "RTSCamera.h"

RTSCamera::RTSCamera(HWND g_hWnd, GraphicsEngineParams &params) : Camera(g_hWnd, params)
{
	this->up = D3DXVECTOR3(0, 1, 0);
	this->distanceFromMesh = Vector3(0, 0, 0);
	this->forward = D3DXVECTOR3(0, -2, 1);
	this->forward = this->NormalizeVector(this->forward);

	CursorControl cc;
	cc.SetVisibility(true);
}

RTSCamera::~RTSCamera()
{
	
}

void RTSCamera::UpdateSpecific(float delta)
{
	POINT p;
	if(GetCursorPos(&p))
	{
		if(ScreenToClient(this->g_hWnd, &p))
		{
			if(p.x < 50)
				this->MoveLeft(delta);
			if(p.x > this->params.WindowWidth - 50)
				this->MoveRight(delta);
			if(p.y < 50)
				this->MoveForward(delta);
			if(p.y > this->params.WindowHeight - 50)
				this->MoveBackward(delta);
		}
	}
}

void RTSCamera::MoveForward(float diff)
{
	Vector3 newF = Vector3(this->forward.x, 0, this->forward.z);
	newF.Normalize();

	newF = newF * ((float)diff/1000.0f) * this->speed;
	this->pos += D3DXVECTOR3(newF.x, newF.y, newF.z);

	//this->pos.z += ((float)diff/100) * this->speed;
}

void RTSCamera::MoveBackward(float diff)
{
	Vector3 newF = Vector3(this->forward.x, 0, this->forward.z);
	newF.Normalize();

	newF = newF * ((float)diff/1000.0f) * this->speed;
	this->pos -= D3DXVECTOR3(newF.x, newF.y, newF.z);

	//this->pos.z -= ((float)diff/100) * this->speed;
}

void RTSCamera::MoveLeft(float diff)
{
	this->pos.x -= sin(this->angleX) * ((float)diff/1000.0f) * this->speed;
	this->pos.z += cos(this->angleX) * ((float)diff/1000.0f) * this->speed;
	//this->pos.x -= ((float)diff/100) * this->speed;
}

void RTSCamera::MoveRight(float diff)
{
	this->pos.x += sin(this->angleX) * ((float)diff/1000.0f) * this->speed;
	this->pos.z -= cos(this->angleX) * ((float)diff/1000.0f) * this->speed;
	//this->pos.x += ((float)diff/100) * this->speed;
}

void RTSCamera::MoveUp( float diff )
{
	this->pos -= this->forward * ((float)diff/1000.0f) * this->speed;
}

void RTSCamera::MoveDown( float diff )
{
	this->pos += this->forward * ((float)diff/1000.0f) * this->speed;
}
