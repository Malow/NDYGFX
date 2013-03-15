// Marcus Löwegren

// For textures
Texture2D tex2D;
SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

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

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

cbuffer EveryObject
{
	float opacity;
	float posx;
	float posy;
	float dimx;
	float dimy;
	float strata;
}

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
	float opacity : OPACITY;
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
	output.opacity = 1.0f;
	
	//create sprite quad
	float4 basepos = float4(posx, posy, strata, 1);

	//bottom left
	output.Pos = basepos + float4(0, dimy, 0, 0);
	output.tex = float2(0,1);
	triStream.Append(output);
	
	//bottom right
	output.Pos = basepos + float4(dimx, dimy, 0, 0);
	output.tex = float2(1,1);
	triStream.Append(output);

	//top left
	output.Pos = basepos;
	output.tex = float2(0,0);
	triStream.Append(output);

	//top right
	output.Pos = basepos + float4(dimx, 0, 0, 0);
	output.tex = float2(1,0);
	triStream.Append(output);
}



//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	float4 tex = tex2D.Sample(linearSampler, input.tex);
	tex.w *= opacity;

	if(tex.w < 0.15f)	// Make transperance correct.
		discard;

	return tex;
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
	    

		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( NoCulling );
		SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}