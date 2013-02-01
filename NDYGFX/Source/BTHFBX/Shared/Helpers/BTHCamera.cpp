#include "Camera.h"

Camera::Camera()
{
	SetPosition(D3DXVECTOR3(0,0,-250));
	SetLookAt(D3DXVECTOR3(0,0,0));
	mLookAt = mToLookAt;
	SetUp(D3DXVECTOR3(0,1,0));
	SetFar(150.0f);
	SetNear(0.5f);
	SetFOV((float)D3DX_PI * (1.0f / 4.0f));
}

D3DXMATRIX Camera::GetView()
{
	return mView;
}

D3DXMATRIX Camera::GetProjection()
{
	return mProjection;
}

HRESULT Camera::Update(float deltaTime, float Width, float Height)
{
	D3DXVec3Lerp(&mLookAt, &mLookAt, &mToLookAt, deltaTime);

	//move to destination position
	D3DXVECTOR3 movementDir = mDestination - GetPosition();
	mPosition += movementDir * deltaTime;

	CalculateView();

	D3DXMatrixPerspectiveFovLH(&mProjection, mFOV, Width / Height, mNear, mFar);

	mViewProjection = mView * mProjection;

	return S_OK;
}

void Camera::CalculateView()
{
	D3DXMatrixLookAtLH(&mView, &GetPosition(), &GetLookAt(), &GetUp());

	mRight = D3DXVECTOR3(mView._11, mView._21, mView._31);
	D3DXVec3Normalize(&mRight, &mRight);
}

D3DXMATRIX Camera::GetViewProjection()
{
	return mViewProjection;
}

void Camera::SetPosition(const D3DXVECTOR3& pos)
{
	mPosition = pos;
	SetDestination(pos);
}

const D3DXVECTOR3& Camera::GetPosition()
{
	return mPosition;
}

void Camera::SetLookAt(const D3DXVECTOR3& lookAt)
{
	mToLookAt = lookAt;
}

const D3DXVECTOR3& Camera::GetLookAt()
{
	return mLookAt;
}

void Camera::SetUp(const D3DXVECTOR3& up)
{
	mUp = up;
}

const D3DXVECTOR3& Camera::GetUp()
{
	return mUp;
}

const D3DXVECTOR3& Camera::GetLookVec()
{
	mLookVec = GetLookAt() - GetPosition();
	
	D3DXVec3Normalize(&mLookVec, &mLookVec);

	return mLookVec;
}

const D3DXVECTOR3& Camera::GetRightVec()
{
	return mRight;
}

void Camera::SetDestination(const D3DXVECTOR3& destination)
{
	mDestination = destination;
}

const D3DXVECTOR3& Camera::GetDestination()
{
	return mDestination;
}

void Camera::SetFar(float f)
{
	mFar = f;
}

const float Camera::GetFar()
{
	return mFar;
}

void Camera::SetNear(float n)
{
	mNear = n;
}

const float Camera::GetNear()
{
	return mNear;
}

void Camera::SetFOV(float fov)
{
	mFOV = fov;
}

const float Camera::GetFOV()
{
	return mFOV;
}