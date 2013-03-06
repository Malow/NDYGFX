//--------------------------------------------------------------------------------------------------
//	Original code taken from ShadowMap.fx.
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Shader for drawing static geometry instanced.
//	
//--------------------------------------------------------------------------------------------------

#include "stdafx.fx"
Texture2D g_DiffuseMap;

cbuffer PerCascade
{
	float4x4 g_LightViewProj;
}
cbuffer PerStrip
{
	bool g_IsTextured;
};

struct VSIn
{
	float3 pos		: POSITION;
	float2 tex		: TEXCOORD;
	//float3 dummy	: NORMAL;
	//float3 dummy	: COLOR;
	//float3 dummy	: TANGENT
	//float3 dummy	: BINORMAL;

	//Instance data
	row_major float4x4 world	: WORLD;
};

struct PSIn
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD;
};

PSIn VS(VSIn input)
{
	PSIn output = (PSIn)0;

	output.pos = mul(float4(input.pos, 1.0f), mul(input.world, g_LightViewProj)); 
	output.tex = input.tex;

	return output;
}

void PS(PSIn input)
{
	if(g_IsTextured)
	{
		if(g_DiffuseMap.Sample(PointWrapSampler, input.tex).a < 0.5f)
		{
			discard;
		}
	}
}

technique11 RenderShadowMapInstanced
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader ( ps_4_0, PS() ) );
		SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( SolidFrontCulling );
	}
}