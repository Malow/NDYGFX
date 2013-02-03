#include "CatmullCurve.h"

CatmullCurve::CatmullCurve()
{
	Reset();
	mLineDrawer.SetPermanent(true);
}

CatmullCurve::~CatmullCurve()
{

}

bool CatmullCurve::Init(Device3D* device3D, IBTHFbxCurve* bthCurve)
{
	if(mLineDrawer.Init(device3D, 10000) == false)
	{
		return false;
	}

	for(int i = 0; i < bthCurve->GetNumControlPoints(); i++)
	{
		D3DXVECTOR3 tmp = *(D3DXVECTOR3*)&bthCurve->GetControlPoint(i);
		mControlPoints.push_back(tmp);
	}

	bool first = true;
	D3DXVECTOR3 tmp;
	while(!Finished())
	{
		Update(0.25f);

		if(first == false)
			mLineDrawer.AddLine(tmp, mCurrentPosition, D3DXVECTOR3(1,1,0));

		first = false;
		tmp = mCurrentPosition;
	}

	Reset();

	return true;
}

D3DXVECTOR3 CatmullCurve::GetCurrentPosition()
{
	return mCurrentPosition;
}

void CatmullCurve::Reset()
{
	t = 0;
	mCurrentSegment = 0;
}

void CatmullCurve::Update(float dt)
{
	if(mCurrentSegment < (int)mControlPoints.size() - 4)
	{
		D3DXVECTOR3 p0 = mControlPoints[mCurrentSegment];
		D3DXVECTOR3 p1 = mControlPoints[mCurrentSegment+1];
		D3DXVECTOR3 p2 = mControlPoints[mCurrentSegment+2];
		D3DXVECTOR3 p3 = mControlPoints[mCurrentSegment+3];

		mCurrentPosition = 0.5f * (
			(2*p1) + 
			(-p0+p2) * t + 
			(2*p0-5*p1+4*p2-p3) * t * t + 
			(-p0+3*p1-3*p2+p3) * t * t * t);
		
		t += dt;

		if(t >= 1.0f)
		{
			mCurrentSegment++;
			t -= 1.0f;
		}
	}
}

void CatmullCurve::Render(D3DXMATRIX worldViewProj)
{
	mLineDrawer.Render(worldViewProj);
}

bool CatmullCurve::Finished()
{
	return mCurrentSegment >= (int)mControlPoints.size() - 4;
}