//--------------------------------------------------------------------------------------
// Basic.fx
// Direct3D 11 Shader Model 4.0 Demo
// Copyright (c) Stefan Petersson, 2011
//--------------------------------------------------------------------------------------

// Marcus Löwegren
#include "stdafx.fx"

// For textures
Texture2D tex2D;
Texture2D tex2D2;
SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------


cbuffer EveryStrip
{
	matrix WVP;
	matrix worldMatrix;
	matrix worldMatrixInverseTranspose;
	bool textured;

	float4 AmbientLight; //**tillman opt - används inte
	float SpecularPower;
	float4 SpecularColor;
	float4 DiffuseColor;
};
cbuffer EveryMesh
{
	uint specialColor;
}

struct VSIn
{
	float4 Pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 Color : COLOR;
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 Color : COLOR;

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
3: Position XYZ, W unused
4: Specular XYZ, W Specular Power

*/

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	input.Color.w = 1.0;
	input.Pos.w = 1.0;

	PSSceneIn output = (PSSceneIn)0;
	output.Pos = mul(input.Pos, WVP);
	output.WorldPos = mul(input.Pos, worldMatrix);
	output.tex = input.tex;
	//output.norm = normalize(mul(input.norm, (float3x3)worldMatrixInverseTranspose));
	output.norm = normalize(mul(input.norm, (float3x3)worldMatrixInverseTranspose));	
	// Dont, do it in pixel shader cuz normal is used for elevation.
	// Hardcode the normal to straight up to get rid of lines between planes
	// Should work decently unless the plane is angled ALOT.

	output.Color = input.Color;

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
		textureColor = tex2D.Sample(linearSampler, float2(input.WorldPos.x, input.WorldPos.z) * 0.1f) * 0.65f;
		textureColor += tex2D2.Sample(linearSampler, 
			float2(input.WorldPos.x - (input.norm.x * timerMillis * 5.0f + sin(timerMillis) * 0.2f), 
			input.WorldPos.z - (input.norm.z * timerMillis * 5.0f + cos(timerMillis) * 0.2f)) * 0.2f) * 0.35f;
	}
	float4 finalColor = float4((textureColor.xyz + input.Color.xyz) * DiffuseColor.xyz, 1.0f);
	//finalColor.w = (float)specialColor;	/// Doesnt work, renders the entire plane with clear color


	PSout output;
	output.Texture = finalColor;
	output.NormalAndDepth = float4(float3(0.0f, 1.0f, 0.0f), input.Pos.z / input.Pos.w);		// pos.z / pos.w should work?
	float depth = length(CameraPosition.xyz - input.WorldPos.xyz) / FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	output.Position = input.WorldPos;
	output.Specular = SpecularColor;
	output.Specular.w = SpecularPower;

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
	    SetRasterizerState( NoCulling );
    }  
}