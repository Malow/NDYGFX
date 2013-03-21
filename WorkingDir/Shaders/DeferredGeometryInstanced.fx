//--------------------------------------------------------------------------------------
//	Original Code taken from DeferredGeometry.fx.
//
//	Written by Markus Tillman for projekt NotDeadYet at Blekinga tekniska högskola.
//
//	Shader for drawing static geometry. 
//--------------------------------------------------------------------------------------
#include "stdafx.fx"

// For textures
Texture2D g_DiffuseMap;
Texture2D g_NormalMap;

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

cbuffer EveryFrame
{
	float		g_FarClip;
	float3		g_CamPos;
	float4x4	g_CamViewProj;
};

cbuffer EveryStrip
{
	//Textures
	bool g_Textured;
	bool g_UseNormalMap;
	//Material
	float4 g_DiffuseColor;
	float3 g_SpecularColor;
	float g_SpecularPower;
};



struct VSIn
{
	float3 Pos		: POSITION;
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float3 color	: COLOR;
	float3 Tangent	: TANGENT;
	//float3 Binormal	: BINORMAL;
	//instance data
	row_major float4x4 world					: WORLD;
	row_major float4x4 worldInverseTranspose	: WIT;
};

struct PSSceneIn
{
	float4 Pos		: SV_POSITION;
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float3 color	: COLOR;
	float3 Tangent	: TANGENT;
	float3 Binormal : BINORMAL;

	float4 worldPos : POSITION;
};

struct PSout
{
	float4 Texture			: SV_TARGET0;
	float4 NormalAndDepth	: SV_TARGET1;
	float4 Position			: SV_TARGET2;
	float4 Specular			: SV_TARGET3;
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
	PSSceneIn output = (PSSceneIn)0;

	output.Pos		= mul(float4(input.Pos, 1.0f), mul(input.world, g_CamViewProj));
	output.tex		= input.tex;
	output.color	= input.color;
	output.norm		= normalize(mul(input.norm, (float3x3)input.worldInverseTranspose));
	output.Tangent	= normalize(mul(input.Tangent, (float3x3)input.worldInverseTranspose));
	
	output.Binormal = cross(output.norm, output.Tangent);
	output.worldPos = mul(float4(input.Pos, 1.0f), input.world);

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
PSout PSScene(PSSceneIn input) : SV_Target
{	
	PSout output;

	//Texture
	float4 textureColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if(g_Textured)
	{
		textureColor = g_DiffuseMap.Sample(LinearWrapSampler, input.tex);
		
		if ( textureColor.a < 0.5f )
			discard;
	}
	float4 finalColor = (textureColor + float4(input.color, 0.0f)) * g_DiffuseColor;
	finalColor.w = 0.0f;
	output.Texture = finalColor;


	float depth = length(g_CamPos.xyz - input.worldPos.xyz) / g_FarClip;		// Haxfix

	//Normal and depth
	if(g_UseNormalMap)
	{
		// NormalMap
		float4 bumpMap = g_NormalMap.Sample(LinearWrapSampler, input.tex);
		// Expand the range of the normal value from (0, +1) to (-1, +1).
		bumpMap = (bumpMap * 2.0f) - 1.0f;
		// Calculate the normal from the data in the bump map.
		float3 bumpNormal = input.norm + bumpMap.x * input.Tangent + bumpMap.y * input.Binormal;

		// Normalize the resulting bump normal.
		bumpNormal = normalize(bumpNormal);
		output.NormalAndDepth.xyz = bumpNormal.xyz;
		// NormalMap
	}
	else
	{
		output.NormalAndDepth = float4(input.norm.xyz, input.Pos.z / input.Pos.w);		// pos.z / pos.w should work?
	}
	output.NormalAndDepth.w = depth;

	//Position (and object type)
	output.Position.xyz = input.worldPos.xyz;
	output.Position.w = -1.0f;

	//Specular
	output.Specular.xyz = g_SpecularColor;
	output.Specular.w = g_SpecularPower;
	
	
	
	return output;
}

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 DeferredGeometryInstanced
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( EnableDepth, 0 );
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	    SetRasterizerState( BackCulling );
    }  
}