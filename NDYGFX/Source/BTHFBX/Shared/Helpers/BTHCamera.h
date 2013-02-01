#ifndef CAMERA__H
#define CAMERA__H

#include "BTHFBX\Include\BTHstdafx.h"

class BTHCamera
{
	D3DXVECTOR3 mDestination;
	D3DXVECTOR3	mPosition;
	D3DXVECTOR3	mLookAt;
	D3DXVECTOR3 mToLookAt;
	D3DXVECTOR3 mUp;
	D3DXVECTOR3	mRight;

	D3DXVECTOR3	mLookVec;

	D3DXMATRIX	mView;
	D3DXMATRIX	mProjection;
	D3DXMATRIX	mViewProjection;

	float		mNear;
	float		mFar;
	float		mFOV;

	void CalculateView();

	//////////////////////////////////////////////////////////////////////////
	// Orbit position data
	D3DXVECTOR3	mOrbitPos;
	float		mOrbitDistance;
	float		mOrbitRadian;
	//////////////////////////////////////////////////////////////////////////

public:
	BTHCamera();

	D3DXMATRIX	GetView();
	D3DXMATRIX	GetProjection();
	D3DXMATRIX	GetViewProjection();

	HRESULT Update(float deltaTime, float Width, float Height);


	void SetDestination(const D3DXVECTOR3& destination);
	const D3DXVECTOR3& GetDestination();
	void SetPosition(const D3DXVECTOR3& pos);
	const D3DXVECTOR3& GetPosition();
	void SetLookAt(const D3DXVECTOR3& lookAt);
	const D3DXVECTOR3& GetLookAt();
	
	void SetUp(const D3DXVECTOR3& up);
	const D3DXVECTOR3& GetUp();
	const D3DXVECTOR3& GetRightVec();

	void SetFar(float f);
	const float GetFar();
	void SetNear(float n);
	const float GetNear();

	void SetFOV(float fov);
	const float GetFOV();

	const D3DXVECTOR3& GetLookVec();

	void CalculatePickRayWS(const D3DXVECTOR2& mousePosOnScreen,
		UINT32 screenWidth, UINT32 screenHeight);
};

#endif
