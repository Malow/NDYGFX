// EDIT 2013-01-23 by Tillman: Added transparency.
#include "stdafx.fx"
Texture2D diffuseMap;

cbuffer PerObject
{
	matrix lightWVP;
};
cbuffer PerStrip
{
	bool textured;
}

struct VSIn
{
	float4 Pos		: POSITION;
	float2 Tex		: TEXCOORD;
	float3 Normal	: NORMAL;
	float4 Color	: COLOR;
};

struct PSIn
{
	float4 Pos	: SV_POSITION;
	float2 Tex	: TEXCOORD;
};

PSIn VS(VSIn input)
{
	PSIn output = (PSIn)0;
	output.Pos = mul(input.Pos, lightWVP);
	output.Tex = input.Tex;

	return output;
}
float4 PS(PSIn input) : SV_TARGET
{
	if(textured)
	{
		if(diffuseMap.Sample(PointWrapSampler, input.Tex).a < 0.5f)
		{
			discard;
		}
	}
	return mul(input.Pos, lightWVP);
}

technique11 RenderShadowMap
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