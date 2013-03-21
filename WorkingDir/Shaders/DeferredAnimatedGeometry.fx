//--------------------------------------------------------------------------------------
// Basic.fx
// Direct3D 11 Shader Model 4.0 Demo
// Copyright (c) Stefan Petersson, 2011
//--------------------------------------------------------------------------------------

// Marcus Löwegren
#include "stdafx.fx"

// For textures
Texture2D tex2D;
Texture2D normalMap;
/*SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};*/

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------


cbuffer EveryStrip
{
	matrix WVP;
	matrix worldMatrix;
	matrix worldMatrixInverseTranspose;
	bool textured;
	bool useNormalMap;

	//float4 AmbientLight; //MaloW Opt
	float SpecularPower;
	float4 SpecularColor; //MaloW Opt
	float4 DiffuseColor; //MaloW Opt
	float t;
};
cbuffer EveryMesh
{
	uint specialColor;
}
cbuffer EveryFrame
{
	float4 CameraPosition;
	float g_FarClip;
};

struct VSIn
{
	float4 Pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 Color : COLOR;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;

	float4 Pos_Morph : POSITION_MORPH;
	float2 tex_Morph : TEXCOORD_MORPH;
	float3 norm_Morph : NORMAL_MORPH;
	float4 Color_Morph : COLOR_MORPH;
	float3 Tangent_Morph : TANGENT_MORPH;
	float3 Binormal_Morph : BINORMAL_MORPH;
};


struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 Color : COLOR;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;

	float4 WorldPos : POSITION;
};

struct PSout
{
	float4 Texture : SV_TARGET0;
	float4 NormalAndDepth : SV_TARGET1;
	float4 Position : SV_TARGET2;
	float4 Specular : SV_TARGET3;
};

/*
RTs:
1: Texture XYZ, W Special Color
2: Normal XYZ, W Depth
3: Position XYZ, W Type of object(unused)
4: Specular XYZ, W Specular Power


*/

//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
/*RasterizerState BackCulling
{
	CullMode = Back;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};*/


//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	input.Color.w = 1.0;
	input.Pos.w = 1.0;

	PSSceneIn output = (PSSceneIn)0;
	output.Pos = mul(lerp(input.Pos, input.Pos_Morph, t), WVP);
	output.WorldPos = mul(lerp(input.Pos, input.Pos_Morph, t), worldMatrix);
	output.tex = lerp(input.tex, input.tex_Morph, t);
	output.norm = normalize(mul(lerp(input.norm, input.norm_Morph, t), (float3x3)worldMatrixInverseTranspose));
	output.Tangent = normalize(mul(lerp(input.Tangent, input.Tangent_Morph, t), (float3x3)worldMatrixInverseTranspose));
	output.Binormal = normalize(mul(lerp(input.Binormal, input.Binormal_Morph, t), (float3x3)worldMatrixInverseTranspose));
	output.Color = lerp(input.Color, input.Color_Morph, t);

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
PSout PSScene(PSSceneIn input) : SV_Target
{	
	float4 textureColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if(textured)
	{
		textureColor = tex2D.Sample(LinearWrapSampler, input.tex);
		if ( textureColor.a < 0.5f )
			discard;
	}
	float4 finalColor = (textureColor + input.Color) * DiffuseColor; // MaloW Opt

	finalColor.w = (float)specialColor;

	PSout output;
	output.Texture = finalColor;
	output.NormalAndDepth = float4(input.norm.xyz, input.Pos.z / input.Pos.w);		// pos.z / pos.w should work?

	float depth = length(CameraPosition.xyz - input.WorldPos.xyz) / g_FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	output.Position.xyz = input.WorldPos.xyz;
	output.Position.w = -1.0f;

	output.Specular = SpecularColor;

	output.Specular.w = SpecularPower;

	if(useNormalMap)
	{
		// NormalMap
		float4 bumpMap = normalMap.Sample(LinearWrapSampler, input.tex);
		// Expand the range of the normal value from (0, +1) to (-1, +1).
		bumpMap = (bumpMap * 2.0f) - 1.0f;
		// Calculate the normal from the data in the bump map.
		float3 bumpNormal = input.norm + bumpMap.x * input.Tangent + bumpMap.y * input.Binormal;
		// Normalize the resulting bump normal.
		bumpNormal = normalize(bumpNormal);
		output.NormalAndDepth.xyz = bumpNormal.xyz;
		// NormalMap
	}

	return output;
}


//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}