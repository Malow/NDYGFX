//--------------------------------------------------------------------------------------------------
//	Code modified from ShadowMap.fx.
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//--------------------------------------------------------------------------------------------------
#include "stdafx.fx"
Texture2D diffuseMap;

//-----------------------------------------------------------------------------------------
// Constant buffers
//----------------------------------------------------------------------------------------
cbuffer PerFrame
{
	float3		gCameraPos;
	float4x4	gCamViewProj;
	float		gFarClip;
};
cbuffer PerBillboard
{
	matrix		gLightWVP;
	bool		gTextured;
};

struct VSIn
{
	float4 pos		: POSITION;
	float2 tex		: TEXCOORD;
	float3 dummy	: DUMMY; //ta bort
	float4 color	: COLOR;
};

struct PSIn
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD;
};

PSIn VS(VSIn input)
{
	PSIn output = (PSIn)0;
	output.pos = mul(input.pos, gLightWVP);
	output.tex = input.tex;

	return output;
}

float PS(PSIn input) : SV_Depth
{
	/*if(textured)
	{
		if(diffuseMap.Sample(PointWrapSampler, input.Tex).a < 0.5f)
		{
			discard;
		}
	}*/

	return input.pos.z;
}

technique11 RenderShadowMapBillboardInstanced
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