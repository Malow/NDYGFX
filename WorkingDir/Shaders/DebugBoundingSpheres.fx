//--------------------------------------------------------------------------------------
// Basic.fx
// Direct3D 11 Shader Model 4.0 Demo
// Copyright (c) Stefan Petersson, 2011
//--------------------------------------------------------------------------------------

// Marcus Löwegren

// For textures
Texture2D tex2D;
SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
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
//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
RasterizerState NoCulling
{
	CullMode = Back;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};


//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
cbuffer EveryObject
{
	matrix VP;
	matrix WVP;
	matrix worldMatrix;
	float width;
	float height;
	float AspectH;
};

cbuffer EveryStrip
{
	float3 SphereCenter;
	float SphereRadius;
};

struct VSIn
{
	float4 Pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 Color : COLOR;
};

struct GSIn
{
	float4 Pos : POSITION;
	float radius : RADIUS;
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
};

GSIn VSScene(VSIn input)
{
	GSIn output = (GSIn)0;
	output.Pos = mul(float4(SphereCenter, 1.0f), WVP);
	float3 v = float3(0, 1, 0) * SphereRadius;
	output.radius = length(v);
	return output;
}

[maxvertexcount(4)]
void GS(point GSIn input[1], inout TriangleStream<PSSceneIn> lineStream)
{
	PSSceneIn output;
	
	output.Pos = input[0].Pos + float4(1, 0, 0, 0) * input[0].radius * AspectH + float4(0, 1, 0, 0) * input[0].radius;
	lineStream.Append(output);
	output.Pos = input[0].Pos + float4(1, 0, 0, 0) * input[0].radius * AspectH + float4(0, -1, 0, 0) * input[0].radius;
	lineStream.Append(output);
	output.Pos = input[0].Pos + float4(-1, 0, 0, 0) * input[0].radius * AspectH + float4(0, 1, 0, 0) * input[0].radius;
	lineStream.Append(output);
	output.Pos = input[0].Pos + float4(-1, 0, 0, 0) * input[0].radius * AspectH + float4(0, -1, 0, 0) * input[0].radius;
	lineStream.Append(output);
}

float4 PSScene(PSSceneIn input) : SV_Target
{	
	float4 retColor = float4(1.0f, 1.0f, 1.0f, 0.0f);
	float4 pos = mul(float4(SphereCenter, 1.0f), WVP);
	float4 posSS = input.Pos;
	posSS.x = posSS.x / width;
	posSS.y = posSS.y / height;
	float2 to = pos.xy - posSS.xy;
	if(length(to) < SphereRadius)
	{
		retColor = float4(1.0f, 1.0f, 1.0f, 0.5f);
	}

	return retColor;
}

technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( NoCulling );
		SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}