//--------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinga Tekniska Högskola.
//	//**TODO:implement**
//--------------------------------------------------------------------------------------
#include "stdafx.fx"

//-----------------------------------------------------------------------------------------
//	Global variables (non-numeric values cannot be added to a constantbuffer.)
//-----------------------------------------------------------------------------------------
Texture2D tex1;
Texture2D tex2;
Texture2D tex3;

//-----------------------------------------------------------------------------------------
// Constant buffers
//-----------------------------------------------------------------------------------------
cbuffer PerFrame
{
	float4 cameraPosition;
};
cbuffer PerObject
{
	matrix	WVP;
	matrix	worldMatrix;
	matrix	worldMatrixInverseTranspose;
};
cbuffer PerStrip
{
	bool	textured;
	
	float	specularPower;
	float4	specularColor;
	float4	diffuseColor;
	float4	ambientLight;
};

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
struct VSIn
{
	float4 pos		: POSITION;
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float4 color	: COLOR;
};

struct PSSceneIn
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float4 color	: COLOR;

	float4 posW		: POSITION;	//world position
};

struct PSOut			
{
	float4 Texture			: SV_TARGET0;	//Texture XYZ, unused W
	float4 NormalAndDepth	: SV_TARGET1;	//Normal XYZ, depth W
	float4 Position			: SV_TARGET2;	//Position XYZ, unused W
	float4 Specular			: SV_TARGET3;	//Specular XYZ, specular power W
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	input.color.w = 1.0f;
	input.pos.w = 1.0f;

	PSSceneIn output = (PSSceneIn)0;
	output.pos = mul(input.pos, WVP);
	output.posW = mul(input.pos, worldMatrix);
	output.tex = input.tex;
	output.norm = normalize(mul(input.norm, (float3x3)worldMatrixInverseTranspose));
	output.color = input.color;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSScene
//-----------------------------------------------------------------------------------------
PSOut PSScene(PSSceneIn input) : SV_Target
{	
	
	PSOut output = (PSOut)0; //**

	//Texture RT
	float4 textureColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if(textured)
	{
		textureColor.xyz = tex1.Sample(LinearWrapSampler, input.tex).xyz; 
	}
	float4 finalColor = (textureColor + input.color) * diffuseColor;
	output.Texture = finalColor;

	//NormalAndDepth RT
	output.NormalAndDepth = float4(input.norm.xyz, input.pos.z / input.pos.w);	
	float depth = length(cameraPosition.xyz - input.posW.xyz) / 200.0f;		// Haxfix
	output.NormalAndDepth.w = depth;

	//Position RT
	output.Position = input.posW;
	
	//Specular RT
	output.Specular = specularColor;
	output.Specular.w = specularPower;

	return output;
}


//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 DeferredGeometryBlendMapTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
    }  
}