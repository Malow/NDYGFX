#pragma once

#include "Shader.h"
#include <string>

class ParticleShader : public Shader
{
	private:
		std::string*			m_pFileName; 
		ID3DX11EffectTechnique*	m_pStreamOutputEffectTechnique;

	public:
		ParticleShader();
		virtual~ParticleShader();

		const std::string* GetFileName() const;

		//**arv**
		HRESULT Init(ID3D11Device* device, std::string fileName, const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, unsigned int nrOfElements);
		HRESULT Apply(unsigned int technique, unsigned int pass);

		//**
		ID3D11InputLayout*		GetInputLayout();
		ID3DX11EffectTechnique* GetDrawEffectTechnique();
		ID3DX11EffectTechnique* GetStreamOutputEffectTechnique();
};