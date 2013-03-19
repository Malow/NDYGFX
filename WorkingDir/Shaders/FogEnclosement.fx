// Marcus Löwegren

//blend state
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

DepthStencilState DisableDepthWrite
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

Texture2D Position;
SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer everyFrame
{
	float3 center;
	float fogRadius;
	float fogFadeFactor;
	float overallFogFactor;
};

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

struct DummyIn
{
	
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 tex : TEXCOORD;
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
DummyIn VSScene(DummyIn input)
{
	return input;
}


// GS
[maxvertexcount(4)]
void GS( point DummyIn input[1], inout TriangleStream<PSSceneIn> triStream )
{
	PSSceneIn output;
	
	//bottom left
	output.Pos = float4(-1, -1, 0, 1);
	output.tex = float2(0,1);
	triStream.Append(output);
	
	//bottom right
	output.Pos = float4(1, -1, 0, 1);
	output.tex = float2(1,1);
	triStream.Append(output);

	//top left
	output.Pos = float4(-1, 1, 0, 1);
	output.tex = float2(0,0);
	triStream.Append(output);

	//top right
	output.Pos = float4(1, 1, 0, 1);
	output.tex = float2(1,0);
	triStream.Append(output);
}


//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	float fogfactor = 0.0f;
	float3 WorldPos = Position.Sample(linearSampler, input.tex).xyz;
	
	float3 toCenter = WorldPos - center;
	float distance = length(toCenter);
	
	if(distance > fogRadius)
	{
		float maxFog = fogRadius * fogFadeFactor;
		if(distance > fogRadius + maxFog)
			fogfactor = 1.0f;
		else
		{
			float curFog = distance - fogRadius;

			fogfactor = curFog / maxFog;

			fogfactor += fogfactor - (fogfactor * fogfactor);	
			// Exponential fog, making it intense quickly at first and slow at the end.
		}
	}

	if(WorldPos.x == -1.0f)
		fogfactor = 0.0f;
	
	return float4(0.45f, 0.45f, 0.45f, 1.0f * (fogfactor + overallFogFactor));
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
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( DisableDepthWrite, 0 );
	    SetRasterizerState( NoCulling );
		SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}