#pragma once

#include <string>
#include "..\..\..\DirectX.h"

class BTHTexture
{
	class Device3D* mDevice3D;

	ID3D11DeviceContext* mDeviceContextUsedToLock;

	ID3D11ShaderResourceView* m_pTextureView;
	ID3D11Texture2D* m_pTexture;

	float	m_iTextureWidth;
	float	m_iTextureHeight;

	std::string zFileName;

public:
	BTHTexture();
	~BTHTexture();

	HRESULT Init(const std::string& fileName, ID3D11Device* dev, ID3D11DeviceContext* devCont);
	HRESULT Init(ID3D11Device* dev, ID3D11DeviceContext* devCont, int width, int height, bool mappable = false, void* initData = NULL);
	void Release();
	ID3D11ShaderResourceView* GetResource();

	float GetWidth();
	float GetHeight();

	void UpdateData(void* data, ID3D11DeviceContext* deviceContext = NULL, D3D11_BOX* pDestRegion = NULL);

	void* Map(ID3D11DeviceContext* deviceContext = 0);
	void Unmap();

	// File Name
	const std::string& GetFilename() { return zFileName; }	
};