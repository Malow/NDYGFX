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

float fogfactor;

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

struct DummyIn
{
	
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
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
	
	output.Pos = float4(-1, -1, 0, 1);
	triStream.Append(output);
	
	//bottom right
	output.Pos = float4(1, -1, 0, 1);
	triStream.Append(output);

	//top left
	output.Pos = float4(-1, 1, 0, 1);
	triStream.Append(output);

	//top right
	output.Pos = float4(1, 1, 0, 1);
	triStream.Append(output);
}


//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	return float4(0.45f, 0.45f, 0.45f, 1.0f * fogfactor);
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