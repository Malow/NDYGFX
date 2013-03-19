RasterizerState FrontCulling
{
	CullMode = Back;	// I think I have inverted the creation of the square, so using back instead of front
};

DepthStencilState DisableDepthWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
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


BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};


SamplerState DepthFilter
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
	AddressW = Clamp;
};

SamplerState DecalFilter
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
	AddressW = BORDER;
};


SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VsIn
{
	float4 Pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float3 color : COLOR;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct PsIn
{
	float4 Position : SV_POSITION;
	float4 WorldPos : POSITION;
};

//[Vertex shader]

matrix WorldViewProj;
matrix World;
float size;

PsIn VSScene(VsIn In)
{
	PsIn Out;
	In.Pos.xy *= size;
	Out.Position = mul(In.Pos, WorldViewProj);
	Out.WorldPos = mul(In.Pos, World);
	return Out;
}


//[Fragment shader]

Texture2D Depth;
Texture2D Decal;
float4x4 ScreenToLocal;
float2 PixelSize;
float opacity;


float2 CS2TS(float2 cs)
{
	return float2(0.5f*cs.x, -0.5f*cs.y) + 0.5f;
}

float4 PSScene(PsIn In) : SV_TARGET
{
    // Compute normalized screen position
	float2 texCoord = In.Position.xy * PixelSize;

    // Compute local position of scene geometry
	float depth = Depth.Sample(DepthFilter, texCoord).w;

	float3 pixelWorldPos = In.WorldPos.xyz;

	float4 pixelClipPosInTexSpace = mul(float4(pixelWorldPos, 1.0f), ScreenToLocal);
	pixelClipPosInTexSpace.xy /= pixelClipPosInTexSpace.w;
	pixelClipPosInTexSpace.xy /= size;

	float4 decal = Decal.Sample(linearSampler, CS2TS(pixelClipPosInTexSpace));
	if(decal.w < 0.5f)
		discard;
	decal.w *= opacity;
	return decal;
}


technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( DisableDepthWrite, 0 );
	    SetRasterizerState( FrontCulling );
		SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}