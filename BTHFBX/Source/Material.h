#pragma once

#pragma warning(disable : 4995)

#include <vector>
#include <string>
#include "../Include/BTHFbxMaterial.h"

class Material : public IBTHFbxMaterial
{
public:
	Material(int nMaterialIndex);
	~Material();

	void SetDiffuseTextureName(std::string strDiffuseTexture) { m_strDiffuseTexture = strDiffuseTexture; }
	std::string GetDiffuseTextureName() { return m_strDiffuseTexture; }

	void SetNormalTextureName(std::string strNormalTexture) { m_strNormalTexture = strNormalTexture; }
	std::string GetNormalTextureName() { return m_strNormalTexture; }

	void AddTexturePath(std::string strTexturePath) { m_TexturePaths.push_back(strTexturePath); }

	int GetMaterialIndex() { return m_nMaterialIndex; }


	/*
	void SetAmbientColor(const D3DXVECTOR3& AmbientColor)				{ m_AmbientColor = AmbientColor; }
	void SetEmissiveColor(const D3DXVECTOR3& EmissiveColor)				{ m_EmissiveColor = EmissiveColor; }
	void SetDiffuseColor(const D3DXVECTOR3& DiffuseColor)				{ m_DiffuseColor = DiffuseColor; }
	void SetSpecularColor(const D3DXVECTOR3& SpecularColor)				{ m_SpecularColor = SpecularColor; }
	*/
	void SetSpecularPower(float fSpecularPower)							{ m_fSpecularPower = fSpecularPower; }
	void SetTransparency(float fTransparency)							{ m_fTransparency = fTransparency; }

	/*
	const D3DXVECTOR3& GetAmbientColor()								{ return m_AmbientColor; }
	const D3DXVECTOR3& GetEmissiveColor()								{ return m_EmissiveColor; }
	const D3DXVECTOR3& GetDiffuseColor()								{ return m_DiffuseColor; }
	const D3DXVECTOR3& GetSpecularColor()								{ return m_SpecularColor; }
	*/

	void SetAmbientColor2(const BTHFBX_VEC3& AmbientColor)				{ m_AmbientColor2 = AmbientColor; }
	void SetEmissiveColor2(const BTHFBX_VEC3& EmissiveColor)				{ m_EmissiveColor2 = EmissiveColor; }
	void SetDiffuseColor2(const BTHFBX_VEC3& DiffuseColor)				{ m_DiffuseColor2 = DiffuseColor; }
	void SetSpecularColor2(const BTHFBX_VEC3& SpecularColor)				{ m_SpecularColor2 = SpecularColor; }

	virtual const BTHFBX_VEC3& GetAmbientColor()									{ return m_AmbientColor2; }
	virtual const BTHFBX_VEC3& GetEmissiveColor()								{ return m_EmissiveColor2; }
	virtual const BTHFBX_VEC3& GetDiffuseColor()									{ return m_DiffuseColor2; }
	virtual const BTHFBX_VEC3& GetSpecularColor()								{ return m_SpecularColor2; }
	virtual float SetSpecularPower()											{ return m_fSpecularPower; }
	virtual float SetTransparency()												{ return m_fTransparency; }

	virtual bool HasDiffuseTexture()									{ return m_strDiffuseTexture != "NOT_SET"; }
	virtual const char* GetDiffuseTextureFilename()						{ return m_strDiffuseTexture.c_str(); }

	virtual bool HasNormalTexture()										{ return m_strNormalTexture != "NOT_SET"; }
	virtual const char* GetNormalTextureFilename()						{ return m_strNormalTexture.c_str(); }
protected:
	std::string m_strDiffuseTexture;
	std::string m_strNormalTexture;

	std::vector<std::string> m_TexturePaths;

	int m_nMaterialIndex;

	/*
	D3DXVECTOR3 m_AmbientColor;
	D3DXVECTOR3 m_EmissiveColor;
	D3DXVECTOR3 m_DiffuseColor;
	D3DXVECTOR3 m_SpecularColor;
	*/
	float m_fSpecularPower;
	float m_fTransparency;


	BTHFBX_VEC3 m_AmbientColor2;
	BTHFBX_VEC3 m_EmissiveColor2;
	BTHFBX_VEC3 m_DiffuseColor2;
	BTHFBX_VEC3 m_SpecularColor2;
};