

cbuffer EveryFrame
{
	matrix gWVP;
}

TextureCube SkyMap;

struct VSIn
{
	float3 Pos : POSITION;
};

struct SKYMAP_VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD; //(pos)
};

struct PSout
{
	float4 Texture : SV_TARGET0;
	float4 NormalAndDepth : SV_TARGET1;
	float4 Position : SV_TARGET2;
	float4 Specular : SV_TARGET3;
};

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

	return output;
}
float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	return SkyMap.Sample(linearSampler, input.texCoord);
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
    } 
}