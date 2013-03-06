// EDIT 2013-01-23 by Tillman: Added transparency.
#include "stdafx.fx"
Texture2D diffuseMap0;
//Texture2D diffuseMap1; //Only need one diffuse map if no blending is one.

cbuffer PerObject
{
	float t;
	matrix lightWVP;
};

cbuffer PerStrip
{
	bool textured;
};

struct VSIn
{
	float3 Pos		: POSITION; 
	float2 Tex		: TEXCOORD;
	//float3 Normal	: NORMAL;//dummy
	//float4 Color	: COLOR; //dummy

	float3 Pos_Morph	: POSITION_MORPH; 
	float2 Tex_Morph	: TEXCOORD_MORPH;
	//float3 Normal_Morph : NORMAL_MORPH;//dummy
	//float4 Color_Morph	: COLOR_MORPH;//dummy
};

struct PSIn 
{
	float4 Pos	: SV_POSITION;
	float2 Tex	: TEX_COORD;
};

PSIn VS(VSIn input) 
{
	PSIn output = (PSIn)0;
	output.Pos = mul(lerp(float4(input.Pos, 1.0f), float4(input.Pos_Morph, 1.0f), t), lightWVP); //Interpolate position and transform it.
	output.Tex = lerp(input.Tex, input.Tex_Morph, t);

	return output;
}
void PS(PSIn input)
{
	if(textured)
	{
		if(diffuseMap0.Sample(PointWrapSampler, input.Tex).a < 0.5f) 
		{
			discard;
		}
	}
}



technique11 RenderShadowMapAnimated
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