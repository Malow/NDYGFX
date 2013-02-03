#pragma once

#include <..\Source\BTHFBX\Include\stdafx.h>

#include <..\Source\BTHFBX\Include\BTHFbx.h>
#include "LineDrawer.h"

class CatmullCurve
{
	D3DXVECTOR3					mCurrentPosition;
	std::vector<D3DXVECTOR3>	mControlPoints;
	int							mCurrentSegment;
	float						t;

	LineDrawer					mLineDrawer;
public:
	CatmullCurve();
	~CatmullCurve();

	bool Init(Device3D* device3D, IBTHFbxCurve* bthCurve);

	void Reset();

	void Update(float dt);

	void Render(D3DXMATRIX worldViewProj);

	bool Finished();

	D3DXVECTOR3 GetCurrentPosition();
};