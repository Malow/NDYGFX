#include "Material.h"

//--------------------------------------------------------------------------------------
Material::Material(int nMaterialIndex) : 
	m_nMaterialIndex(nMaterialIndex)
{
	m_TexturePaths.push_back("../Media/Textures/");
	m_TexturePaths.push_back("../Textures/");

	m_fSpecularPower = 1.0f;
	m_fTransparency = 1.0f;

	SetDiffuseTextureName("NOT_SET");
	SetNormalTextureName("NOT_SET");
}

//--------------------------------------------------------------------------------------
Material::~Material()
{

}