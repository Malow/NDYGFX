/*
#pragma once

#include <..\Source\BTHFBX\Include\stdafx.h>

class Shader;
class Buffer;
class Device3D;

class LineDrawer
{
	struct LINE_VERTEX
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR3 Color;
	};

	Device3D*					mDevice3D;
	Shader*						mShader;
	Buffer*						mVB;

	std::vector<LINE_VERTEX>	mLines;
	int							mCapacity;
	int							mLinesToDraw;

	bool						mPermanent;
	bool						mHasChanged;
public:
	LineDrawer();
	~LineDrawer();

	bool Init(Device3D* device3D, int capacity);

	void SetPermanent(bool permanent);
	bool GetPermanent();

	bool AddLine(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXVECTOR3 color = D3DXVECTOR3(1,1,1));

	bool AddAABB(D3DXVECTOR3 min, D3DXVECTOR3 max, D3DXVECTOR3 color = D3DXVECTOR3(1,1,1), D3DXMATRIX* transformMatrix = NULL);
	bool AddOBB(D3DXVECTOR3 corners[8], D3DXVECTOR3 color = D3DXVECTOR3(1,1,1));

	bool AddOctreeSplitPlaneLines(D3DXVECTOR3 min, D3DXVECTOR3 max, BYTE mask, D3DXVECTOR3 color = D3DXVECTOR3(1,1,1));

	void Render(D3DXMATRIX worldViewProj);
};
*/