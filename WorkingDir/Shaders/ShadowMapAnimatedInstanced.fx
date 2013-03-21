//--------------------------------------------------------------------------------------------------
//	Original code taken from ShadowMapAnimated.fx.
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Shader for drawing animated geometry instanced.
//	
//--------------------------------------------------------------------------------------------------

#include "stdafx.fx"
Texture2D gDiffuseMap0;
//Texture2D diffuseMap1; //Only need one diffuse map if no blending is one.

cbuffer PerCascade
{
	float4x4 gLightViewProj;
};

cbuffer PerStrip
{
	bool gIsTextured;
};

struct VSIn
{
	//Vertex buffer 1
	float3 pos		: POSITION; 
	float2 tex		: TEXCOORD;
	//float3 Normal	: NORMAL;//dummy
	//float4 Color	: COLOR; //dummy

	//Vertex buffer 2
	float3 posMorph	: POSITION_MORPH; 
	float2 texMorph	: TEXCOORD_MORPH;
	//float3 Normal_Morph : NORMAL_MORPH;//dummy
	//float4 Color_Morph	: COLOR_MORPH;//dummy

	//Instance buffer
	row_major float4x4 world	: WORLD;
};

struct PSIn 
{
	float4 Pos	: SV_POSITION;
	float2 Tex	: TEX_COORD;
};

PSIn VS(VSIn input) 
{
	PSIn output = (PSIn)0;
	
	//Fetch interpolation value stored in last element.
	float interpolationValue = input.world[3][3];
	//Set last element to 1 again
	input.world[3][3] = 1;

	output.Pos = mul(lerp(float4(input.pos, 1.0f), float4(input.posMorph, 1.0f), interpolationValue), mul(input.world, gLightViewProj)); //Interpolate position and transform it.
	output.Tex = lerp(input.tex, input.texMorph, interpolationValue);

	return output;
}
void PS(PSIn input)
{
	if(gIsTextured)
	{
		if(gDiffuseMap0.Sample(PointWrapSampler, input.Tex).a < 0.5f) 
		{
			discard;
		}
	}
}



technique11 RenderShadowMapAnimatedInstanced
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader ( ps_4_0, PS() ) );
		//SetPixelShader( NULL );
		SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( SolidFrontCulling );
	}
}