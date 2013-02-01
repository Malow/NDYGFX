#include "LineDrawer.h"

#include "../D3D/Device3D.h"
#include "../D3D/Buffer.h"
#include "../D3D/Shader.h"
#include "ResourceManager.h"

LineDrawer::LineDrawer()
{
	mHasChanged = false;
	mPermanent = false;
	mLinesToDraw = 0;
}

LineDrawer::~LineDrawer()
{
	SAFE_DELETE(mVB);
}

void LineDrawer::SetPermanent(bool permanent)
{
	mPermanent = permanent;
}

bool LineDrawer::GetPermanent()
{
	return mPermanent;
}

bool LineDrawer::Init(Device3D* device3D, int capacity)
{
	mDevice3D = device3D;
	mCapacity = capacity * 2;

	//Create input layout for the FBX scene
	const D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION",			  0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",				  0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	int iNumElements = sizeof(inputDesc)/sizeof(D3D10_INPUT_ELEMENT_DESC);
	
	static Shader* s = 0;

	mShader = ResourceManager::GetInstance(mDevice3D)->GetShader("../Shaders/Lines.fx", inputDesc, iNumElements);


	BUFFER_INIT_DESC d;
	d.ElementSize = sizeof(LINE_VERTEX);
	d.InitData = NULL;
	d.NumElements = mCapacity;
	d.Type = VERTEX_BUFFER;
	d.Usage = BUFFER_CPU_WRITE_DISCARD;

	mVB = myNew Buffer();
	if(FAILED(mVB->Init(mDevice3D, d)))
	{
		SAFE_DELETE(mVB);
		return false;
	}

	return true;
}

bool LineDrawer::AddLine(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXVECTOR3 color)
{
	if((int)mLines.size() < mCapacity)
	{
		LINE_VERTEX v0 = { start, color };
		mLines.push_back(v0);
		LINE_VERTEX v1 = { end, color };
		mLines.push_back(v1);

		mHasChanged = true;

		return true;
	}
	return false;
}

bool LineDrawer::AddOBB(D3DXVECTOR3 corners[8], D3DXVECTOR3 color)
{
	AddLine(corners[0], corners[1], color); //FrontLowerLeft to FrontLowerRight
	AddLine(corners[2], corners[3], color); //FrontUpperLeft to FrontUpperRight

	AddLine(corners[0], corners[2], color); //FrontLowerLeft to FrontUpperLeft
	AddLine(corners[1], corners[3], color); //FrontLowerRight to FrontUpperRight
	
	
	AddLine(corners[4], corners[5], color); //BackLowerLeft to BackLowerRight
	AddLine(corners[6], corners[7], color); //BackUpperLeft to BackUpperRight

	AddLine(corners[4], corners[6], color); //BackLowerLeft to BackUpperLeft
	AddLine(corners[5], corners[7], color); //BackLowerRight to BackUpperRight

	AddLine(corners[0], corners[4], color); //FrontLowerLeft to BackLowerLeft
	AddLine(corners[2], corners[6], color); //FrontUpperLeft to BackUpperLeft
	AddLine(corners[1], corners[5], color); //FrontLowerRight to BackLowerRight
	bool result = AddLine(corners[3], corners[7], color); //FrontUpperRight to BackUpperRight

	return result;
}

bool LineDrawer::AddAABB(D3DXVECTOR3 min, D3DXVECTOR3 max, D3DXVECTOR3 color, D3DXMATRIX* transformMatrix)
{
	D3DXVECTOR3 p[8];

	p[0] = D3DXVECTOR3( min.x, min.y, min.z ); // xyz (min)
	p[1] = D3DXVECTOR3( max.x, min.y, min.z ); // Xyz
	p[2] = D3DXVECTOR3( min.x, max.y, min.z ); // xYz
	p[3] = D3DXVECTOR3( max.x, max.y, min.z ); // XYz
	p[4] = D3DXVECTOR3( min.x, min.y, max.z ); // xyZ
	p[5] = D3DXVECTOR3( max.x, min.y, max.z ); // XyZ
	p[6] = D3DXVECTOR3( min.x, max.y, max.z ); // 
	p[7] = D3DXVECTOR3( max.x, max.y, max.z ); // XYZ (max)

	if(transformMatrix)
		D3DXVec3TransformCoordArray(p, sizeof(D3DXVECTOR3), p, sizeof(D3DXVECTOR3), transformMatrix, 8);

	AddLine(p[0], p[1], color); //FrontLowerLeft to FrontLowerRight
	AddLine(p[2], p[3], color); //FrontUpperLeft to FrontUpperRight

	AddLine(p[0], p[2], color); //FrontLowerLeft to FrontUpperLeft
	AddLine(p[1], p[3], color); //FrontLowerRight to FrontUpperRight
	
	
	AddLine(p[4], p[5], color); //BackLowerLeft to BackLowerRight
	AddLine(p[6], p[7], color); //BackUpperLeft to BackUpperRight

	AddLine(p[4], p[6], color); //BackLowerLeft to BackUpperLeft
	AddLine(p[5], p[7], color); //BackLowerRight to BackUpperRight

	AddLine(p[0], p[4], color); //FrontLowerLeft to BackLowerLeft
	AddLine(p[2], p[6], color); //FrontUpperLeft to BackUpperLeft
	AddLine(p[1], p[5], color); //FrontLowerRight to BackLowerRight
	bool result = AddLine(p[3], p[7], color); //FrontUpperRight to BackUpperRight

	return result;
}

bool LineDrawer::AddOctreeSplitPlaneLines(D3DXVECTOR3 min, D3DXVECTOR3 max, BYTE mask, D3DXVECTOR3 color)
{
	bool b[8];
	for(int i = 0; i < 8; i++)
		b[i] = (mask & (BYTE)pow((float)i,2.0f)) ? true : false;

	return true;
	bool result = AddLine(D3DXVECTOR3(min.x, (min.y + max.y) * 0.5f, max.z), D3DXVECTOR3(max.x, (min.y + max.y) * 0.5f, max.z), color);

	return result;
}

void LineDrawer::Render(D3DXMATRIX worldViewProj)
{
	mDevice3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	mShader->SetMatrix("gWVP", worldViewProj);
	mShader->Apply(0);

	if(mHasChanged)
	{
		void* v = mVB->Map();
		if(v)
		{
			char s[100];
			sprintf_s(s, sizeof(s), "Line count: %d", mLines.size() / 2);
			//MessageBox(0, s, "", 0);
			memcpy(v, &mLines[0], sizeof(LINE_VERTEX) * mLines.size());
			mVB->Unmap();

			mHasChanged = false;


			mLinesToDraw = (int)mLines.size();
			mLines.clear();
		}
	}

	mVB->Apply();

	mDevice3D->GetDeviceContext()->Draw(mLinesToDraw, 0);
}