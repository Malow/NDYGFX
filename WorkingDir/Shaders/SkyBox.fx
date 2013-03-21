//------------------------------------------------------------------------------------------------------
//	Modified by Markus Tillman at Blekinga Tekniska Högskola for project "Not dead yet".
//
//	Rendering: Skybox.
//	Renders a skybox using spheremapping.
//------------------------------------------------------------------------------------------------------

cbuffer EveryFrame
{
	matrix gWVP;
	matrix world;
	float FogHeight;
	float CamY;
}

TextureCube SkyMap;

struct VSIn
{
	float3 Pos : POSITION;
};

struct SKYMAP_VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 worldPos : POSITION;
	float3 texCoord : TEXCOORD; //(pos)
};
/*
struct PSOut
{
	float4 Texture : SV_TARGET0;
	float4 NormalAndDepth : SV_TARGET1;
	float4 Position : SV_TARGET2;
	float4 Specular : SV_TARGET3;
};*/

RasterizerState RastDesc
{
	CullMode = None;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
	DepthFunc = LESS_EQUAL;
};
BlendState SrcAlphaBlendingAdd 
{ 
	BlendEnable[0] = TRUE; 
	SrcBlend = SRC_ALPHA; 
	DestBlend = INV_SRC_ALPHA; 
	BlendOp = ADD; 
	SrcBlendAlpha = ZERO; 
	DestBlendAlpha = ZERO; 
	BlendOpAlpha = ADD; 
	RenderTargetWriteMask[0] = 0x0F; 
}; 

SamplerState linearSampler 
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};



SKYMAP_VS_OUTPUT SKYMAP_VS(VSIn input)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	//Set Pos to xyww instead of xyzw, so that z/w will always be 1 (furthest from camera)
	output.Pos = mul(float4(input.Pos, 1.0f), gWVP).xyww; 
	output.texCoord = input.Pos;
	output.worldPos = mul(float4(input.Pos, 1.0f), world);

	return output;
}
//PSOut SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	float4 ret = ret = SkyMap.Sample(linearSampler, input.texCoord);
	if(input.worldPos.y < FogHeight)
	{
		float factor = (input.worldPos.y - CamY) / (FogHeight - CamY);

		ret = lerp(float4(0.45f, 0.45f, 0.45f, 1.0f), ret, saturate(factor));
	}
	return ret;

	/*PSOut output = (PSOut)0;

	output.Texture = SkyMap.Sample(linearSampler, input.texCoord);
	output.NormalAndDepth = float4(0,0,0,-1);
	output.Position = float4(0,0,0,0);
	output.Specular = float4(0,0,0,0);

	return output;*/
}

technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, SKYMAP_VS() ) );

        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, SKYMAP_PS() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( RastDesc );
		SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    } 
}