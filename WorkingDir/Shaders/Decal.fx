RasterizerState FrontCulling
{
	CullMode = Front;
};

DepthStencilState DisableDepthWrite
{
    DepthEnable = FALSE;
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



struct VsIn
{
	float4 Position : POSITION;
};

struct PsIn
{
	float4 Position : SV_POSITION;
};

//[Vertex shader]

matrix ViewProj;
float3 Pos;
float size;

PsIn VSScene(VsIn In)
{
	PsIn Out;

	float4 position = In.Position;
	position.xyz *= size;
	position.xyz += Pos;

	Out.Position = mul(ViewProj, position);

	return Out;
}


//[Fragment shader]

Texture2D Depth;
Texture3D <float4> Decal;
float4x4 ScreenToLocal;
float3 Color;
float2 PixelSize;

float4 PSScene(PsIn In) : SV_TARGET
{
    // Compute normalized screen position
	float2 texCoord = In.Position.xy * PixelSize;

    // Compute local position of scene geometry
	float depth = Depth.Sample(DepthFilter, texCoord).w;
	float4 scrPos = float4(texCoord, depth, 1.0f);
	float4 wPos = mul(ScreenToLocal, scrPos);

    // Sample decal
	float3 coord = wPos.xyz / wPos.w;
	float decal = Decal.Sample(DecalFilter, coord).r;

	return float4(Color, decal);
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
    }  
}