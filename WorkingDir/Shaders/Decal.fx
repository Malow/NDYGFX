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
};

//[Vertex shader]

matrix WorldViewProj;

PsIn VSScene(VsIn In)
{
	PsIn Out;
	Out.Position = mul(In.Pos, WorldViewProj);
	return Out;
}


//[Fragment shader]

Texture2D Depth;
Texture2D Decal;
float4x4 ScreenToLocal;
float2 PixelSize;

float4 PSScene(PsIn In) : SV_TARGET
{
    // Compute normalized screen position
	float2 texCoord = In.Position.xy * PixelSize;

    // Compute local position of scene geometry
	float depth = Depth.Sample(DepthFilter, texCoord).w;
	float thisDepth = In.Position.z / In.Position.w;

	float4 scrPos = float4(texCoord, depth, 1.0f);
	float4 wPos = mul(ScreenToLocal, scrPos);

    // Sample decal
	float3 coord = wPos.xyz / wPos.w;
	float4 decal = Decal.Sample(linearSampler, coord);
	return float4(1, 0, 0, 1);
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