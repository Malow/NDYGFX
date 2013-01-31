//--------------------------------------------------------------------------------------
// Basic.fx
// Direct3D 11 Shader Model 4.0 Demo
// Copyright (c) Stefan Petersson, 2011
//--------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

cbuffer EveryFrame
{
	matrix gWVP;
};

struct VSIn
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

struct PSSceneIn
{
	float4 Pos  : SV_Position;
	float3 Color : COLOR;
};

//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
RasterizerState NoCulling
{
	CullMode = NONE;
};


//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output = (PSSceneIn)0;

	output.Pos = mul(float4(input.Pos, 1), gWVP);
	
	output.Color = input.Color;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{		
	return float4(input.Color,1);
}

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    
	    SetRasterizerState( NoCulling );
    }  
}