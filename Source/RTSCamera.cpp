#include "RTSCamera.h"

RTSCamera::RTSCamera(HWND g_hWnd, GraphicsEngineParams params) : Camera(g_hWnd, params)
{
	this->up = D3DXVECTOR3(0, 1, 0);
	this->DistanceFromTarget = 75.0f;
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
			if(p.x > this->params.windowWidth - 50)
				this->MoveRight(delta);
			if(p.y < 50)
				this->MoveForward(delta);
			if(p.y > this->params.windowHeight - 50)
				this->MoveBackward(delta);
		}
	}
}

void RTSCamera::MoveForward(float diff)
{
	this->pos.z += ((float)diff/100) * this->speed;
}

void RTSCamera::MoveBackward(float diff)
{
	this->pos.z -= ((float)diff/100) * this->speed;
}

void RTSCamera::MoveLeft(float diff)
{
	this->pos.x -= ((float)diff/100) * this->speed;
}

void RTSCamera::MoveRight(float diff)
{
	this->pos.x += ((float)diff/100) * this->speed;
}