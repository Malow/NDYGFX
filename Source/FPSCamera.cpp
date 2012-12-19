#include "FPSCamera.h"

#define PI (3.14159265358979323846f)

FPSCamera::FPSCamera(HWND g_hWnd, GraphicsEngineParams params) : Camera(g_hWnd, params)
{
	this->up = D3DXVECTOR3(0, 1, 0);
	this->forward = D3DXVECTOR3(0, 0, 1);
	this->DistanceFromTarget = 6.0f;

	CursorControl cc;
	cc.SetVisibility(false);

	POINT np;
	np.x = this->params.windowWidth/2;
	np.y = this->params.windowHeight/2;
	if(ClientToScreen(this->g_hWnd, &np))
	{
		SetCursorPos(np.x, np.y);
	}
}

FPSCamera::~FPSCamera()
{

}

void FPSCamera::UpdateSpecific(float delta)
{
	POINT p;
	if(GetCursorPos(&p))
	{
		if(ScreenToClient(this->g_hWnd, &p))
		{
			float diffX = (this->params.windowWidth/2) - (float)p.x;
			float diffY = (this->params.windowHeight/2) - (float)p.y;
			
			this->angleX += diffX * (this->sensitivity * 0.001f);
			this->angleY += diffY * (this->sensitivity * 0.001f);
			
			fmod(angleX, 2 * PI);
			fmod(angleY, 2 * PI);

			//if(angleY > PI/2)
			//	angleY = PI/2;
			//if(angleY < -PI/2)
			//	angleY = -PI/2;
			
			if(angleX > 2*PI)
				angleX -= 2*PI;
			if(angleX < 0)
				angleX += 2 * PI;

			if(angleY > PI)
				angleY = PI;
			if(angleY < -PI)
				angleY = -PI;

			this->forward.x = cos(angleX);
			//this->forward.y = sin(angleY);
			this->forward.y = angleY;
			this->forward.z = sin(angleX);

			this->forward = this->NormalizeVector(this->forward);
			
			//calc new up
			//crossproduct between y-axis & forward vector
			Vector3 yAxis = Vector3(0, 1, 0);
			Vector3 tmpForward = Vector3();//already normalized
			tmpForward.x = this->forward.x;
			tmpForward.y = this->forward.y; 
			tmpForward.z = this->forward.z; 
			Vector3 rightVec = yAxis.GetCrossProduct(tmpForward);
			rightVec.Normalize();
			//crossproduct between forward & right vector 
			Vector3 tmpUp = Vector3();
			tmpUp = tmpForward.GetCrossProduct(rightVec);
			this->up.x = tmpUp.x;
			this->up.y = tmpUp.y;
			this->up.z = tmpUp.z;
			this->up = this->NormalizeVector(this->up);

			POINT np;
			np.x = this->params.windowWidth/2;
			np.y = this->params.windowHeight/2;
			if(ClientToScreen(this->g_hWnd, &np))
			{
				SetCursorPos(np.x, np.y);
			}
		}
	}
}

void FPSCamera::MoveForward(float diff)
{
	this->pos += this->forward * ((float)diff/100) * this->speed;
}

void FPSCamera::MoveBackward(float diff)
{
	this->pos -= this->forward * ((float)diff/100) * this->speed;
}

void FPSCamera::MoveLeft(float diff)
{
	this->pos.x -= sin(this->angleX) * ((float)diff/100) * this->speed;
	this->pos.z += cos(this->angleX) * ((float)diff/100) * this->speed;
}

void FPSCamera::MoveRight(float diff)
{
	this->pos.x += sin(this->angleX) * ((float)diff/100) * this->speed;
	this->pos.z -= cos(this->angleX) * ((float)diff/100) * this->speed;
}
