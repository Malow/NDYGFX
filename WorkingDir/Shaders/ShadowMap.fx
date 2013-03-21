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
};

struct VSIn
{
	float3 Pos		: POSITION;
	float2 Tex		: TEXCOORD;
	//float3 Normal	: NORMAL;
	//float3 Color	: COLOR;
};

struct PSIn
{
	float4 Pos	: SV_POSITION;
	float2 Tex	: TEXCOORD;
};

PSIn VS(VSIn input)
{
	PSIn output = (PSIn)0;
	output.Pos = mul(float4(input.Pos, 1.0f), lightWVP);
	output.Tex = input.Tex;

	return output;
}

void PS(PSIn input)// : SV_Depth
{
	if(textured)
	{
		if(diffuseMap.Sample(PointWrapSampler, input.Tex).a < 0.5f)
		{
			discard;
		}
	}

	//return input.Pos.z;
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