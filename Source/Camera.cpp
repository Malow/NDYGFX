#include "Camera.h"

Camera::Camera(HWND g_hWnd, GraphicsEngineParams params)
{
	this->forceBoundries = false;
	this->g_hWnd = g_hWnd;
	this->params = params;
	this->pos = D3DXVECTOR3(0, 0, 0);
	this->terrain = NULL;
	this->followTarget = NULL;
	
	this->angleX = 0;
	this->angleY = 0;
	
	this->speed = 1.0f;
	this->sensitivity = 1.0f;

	this->updateCamera = true;
	this->activeWindowDisabling = true;

	D3DXMatrixPerspectiveFovLH(&this->projection, (float)D3DX_PI * this->params.FOV, this->params.windowWidth / (float)this->params.windowHeight, this->params.NearClip, this->params.FarClip);
}

Camera::~Camera()
{

}

D3DXMATRIX Camera::GetViewMatrix()
{
	D3DXVECTOR3 at = this->pos + this->forward;
	D3DXMatrixLookAtLH(&view, &this->pos, &at, &this->up);
	return this->view; 
}

D3DXMATRIX Camera::GetProjectionMatrix()
{
	return this->projection; 
}

Vector3 Camera::GetPosition()
{
	return Vector3(this->pos.x, this->pos.y, this->pos.z);
}

void Camera::SetPosition(Vector3 pos)
{
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
}

Vector3 Camera::GetForward()
{
	return Vector3(this->forward.x, this->forward.y, this->forward.z);
}

void Camera::SetForward(Vector3 forward)
{
	this->angleX = -atan2(forward.x * 0 - 1 * forward.z, forward.x * 1 + forward.z * 0);
	this->angleY = asin(forward.y);

	this->forward = D3DXVECTOR3(forward.x, forward.y, forward.z);
	this->forward = this->NormalizeVector(this->forward);
}

void Camera::LookAt(Vector3 target)
{
	this->forward = D3DXVECTOR3(target.x, target.y, target.z) - this->pos;
	this->forward = this->NormalizeVector(this->forward);

	this->angleX = -atan2(this->forward.x*0 - 1*this->forward.z, this->forward.x * 1 + this->forward.z * 0);
	this->angleY = asin(this->forward.y);
}

Vector3 Camera::GetUpVector()
{
	return Vector3(this->up.x, this->up.y, this->up.z);
}

void Camera::SetUpVector(Vector3 up)
{
	this->up = D3DXVECTOR3(up.x, up.y, up.z);
}

void Camera::MoveToTerrain()
{
	if(this->terrain)
		this->pos.y = this->terrain->GetYPositionAt(this->pos.x, this->pos.z) + this->DistanceFromTarget;
}

void Camera::Move(Vector3 moveBy)
{
	this->pos += D3DXVECTOR3(moveBy.x, moveBy.y, moveBy.z);
	this->MoveToTerrain();
}

D3DXVECTOR3 Camera::NormalizeVector(D3DXVECTOR3 vec)
{
	float length = sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
	vec.x /= length;
	vec.y /= length;
	vec.z /= length;
	return vec;
}

void Camera::MoveToFollowPosition()
{
	if(this->followTarget)
	{
		Vector3 vec = this->followTarget->GetPosition() - Vector3(0, -15, this->DistanceFromTarget);
		this->pos = D3DXVECTOR3(vec.x, vec.y, vec.z);
	}
}

void Camera::Update(float delta)
{
	if(this->updateCamera)
	{
		if(activeWindowDisabling)
		{
			if(GetForegroundWindow() == this->g_hWnd)
				UpdateSpecific(delta);
		}
		else
		{
			UpdateSpecific(delta);
		}

		this->MoveToTerrain();
		this->MoveToFollowPosition();
		if(this->forceBoundries)
		{
			if(this->pos.x < this->minBoundries.x)
				this->pos.x = this->minBoundries.x;
			else if(this->pos.x > this->maxBoundries.x)
				this->pos.x = this->maxBoundries.x;

			if(this->pos.y < this->minBoundries.y)
				this->pos.y = this->minBoundries.y;
			else if(this->pos.y > this->maxBoundries.y)
				this->pos.y = this->maxBoundries.y;

			if(this->pos.z < this->minBoundries.z)
				this->pos.z = this->minBoundries.z;
			else if(this->pos.z > this->maxBoundries.z)
				this->pos.z = this->maxBoundries.z;
		}
	}
}

void Camera::SetBoundries(Vector3 minBoundries, Vector3 maxBoundries)
{
	this->minBoundries = D3DXVECTOR3(minBoundries.x, minBoundries.y, minBoundries.z);
	this->maxBoundries = D3DXVECTOR3(maxBoundries.x, maxBoundries.y, maxBoundries.z);
	this->forceBoundries = true;
}

void Camera::DisableBoundries()
{
	this->forceBoundries = false;
}

void Camera::FollowMesh( iMesh* target )
{
	this->followTarget = dynamic_cast<Mesh*>(target);
}

void Camera::RecreateProjectionMatrix()
{
	D3DXMatrixPerspectiveFovLH(&this->projection, (float)D3DX_PI * this->params.FOV, this->params.windowWidth / (float)this->params.windowHeight, this->params.NearClip, this->params.FarClip);
}
