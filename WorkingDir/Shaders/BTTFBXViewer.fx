//--------------------------------------------------------------------------------------
// File: BTTFBXViewer.fx
//
//--------------------------------------------------------------------------------------

#define MaxBones 100

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
matrix gWVP;
matrix gWorld;			   //  World matrix
matrix gView;
matrix gProj;
matrix gViewProj;
matrix g_mScale;			   //  Scale matrix

matrix g_mBonesArray[MaxBones];
bool g_bSkinning = true;



cbuffer cbImmutable
{
	float3 g_vLight = normalize(float3( 0, 0, -1.0f ));
};

RasterizerState DefaultRasterizerState
{
	CullMode = Back;
	FillMode = Solid;
};

RasterizerState NoCullRasterizerState
{
	CullMode = None;
};

DepthStencilState EnableDepth
{
	DepthEnable = True;
	DepthWriteMask = ALL;
};

BlendState DefaultBlendState
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	RenderTargetWriteMask[0] = 0x0f;
};

Texture2D txNormal;
Texture2D txDiffuse;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
	AddressW = Wrap;
};

//--------------------------------------------------------------------------------------
// Shader output structures
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : SV_POSITION;
    float3 Normal	  : NORMAL;
	float3 Tangent	  : TANGENT;
	float2 TexCoord	  : TEXCOORD;
	float3 PosW  : POS_WORLD;
};

struct PS_OUTPUT
{
    float4 RGBColor : SV_TARGET;
};

//--------------------------------------------------------------------------------------
// Default shaders
//--------------------------------------------------------------------------------------

VS_OUTPUT DefaultVS( float3 vPos	     : SV_POSITION,
					 float3 vNormal      : NORMAL,
					 float3 vTangent     : TANGENT,
					 float2 vTexCoord    : TEXCOORD,
					 float4 vBoneIndices : BLENDINDICES,
					 float4 vBoneWeights : BLENDWEIGHT)
{
    VS_OUTPUT Output;

	float4 position = 3;
	float3 normal = 0, tangent = 0;
	position = float4(vPos,1.0f);

	if( g_bSkinning )
	{
		float4x4 skinTransform = 0;
		skinTransform += g_mBonesArray[vBoneIndices.x] * vBoneWeights.x;
		skinTransform += g_mBonesArray[vBoneIndices.y] * vBoneWeights.y;
		skinTransform += g_mBonesArray[vBoneIndices.z] * vBoneWeights.z;
		skinTransform += g_mBonesArray[vBoneIndices.w] * vBoneWeights.w;
		position = mul(position, skinTransform);
		normal = normalize(mul(vNormal, (float3x3)skinTransform));
		tangent = normalize(mul(vTangent, (float3x3)skinTransform));
	}
	else
	{
		position = mul(position, gWorld);
		normal = mul(vNormal, gWorld);
		tangent = mul(vTangent, gWorld);

	}

	position = mul(position, g_mScale);

	Output.PosW = position.xyz;
	Output.Position = mul(position, gViewProj);
	Output.Normal = mul(float4(normal, 0), g_mScale);
	Output.Tangent = mul(float4(tangent, 0), g_mScale);
	Output.TexCoord = vTexCoord;
    return Output;    
}

//--------------------------------------------------------------------------------------
PS_OUTPUT DefaultPS( VS_OUTPUT In ) 
{ 
	PS_OUTPUT Output;
	float4 vDiffuseTexture = txDiffuse.Sample( samLinear, In.TexCoord );

	float3 normalT = txNormal.Sample( samLinear, In.TexCoord );

	
	Output.RGBColor = vDiffuseTexture;
	return Output;
	

	// Uncompress each component from [0,1] to [-1,1].
	normalT = 2.0f*normalT - 1.0f;


	// build orthonormal basis
	float3 N = normalize(In.Normal);
	float3 T = normalize(In.Tangent - dot(In.Tangent, N)*N);
	float3 B = cross(N,T);
	
	float3x3 TBN = float3x3(T, B, N);
	
	// Transform from tangent space to world space.
	float3 bumpedNormalW = normalize(mul(normalT, TBN));

	float bumpFactor = dot(g_vLight, bumpedNormalW);
	Output.RGBColor = vDiffuseTexture * (saturate(dot(bumpedNormalW, g_vLight)) + 0.02f);
	Output.RGBColor.a = 1.0f;

	return Output;
}



/*
struct VS_OUTPUT
{
    float4 Position   : SV_POSITION;
    float3 Normal	  : NORMAL;
	float3 Tangent	  : TANGENT;
	float2 TexCoord	  : TEXCOORD;
};
*/

[maxvertexcount(8)]
void NormalGS( triangle VS_OUTPUT Input[3],
	inout LineStream<VS_OUTPUT> streamOut )
{	
	VS_OUTPUT Output = Input[0];
	
	for(int i = 0; i < 3; i++)
	{
		float3 Normal = normalize(Input[i].Normal);
		
		Output.Position = mul( float4(Input[i].PosW, 1.0), gViewProj );
		streamOut.Append(Output);
		
		Output.Position = mul( float4(Input[i].PosW + Normal * 0.15f, 1.0), gViewProj );
		streamOut.Append(Output);
		
		streamOut.RestartStrip();
	}
}

PS_OUTPUT NormalPS( VS_OUTPUT In ) 
{
	PS_OUTPUT Output;
	Output.RGBColor = float4(0,0,1,1);
	return Output;
}

//--------------------------------------------------------------------------------------
// D3D10 Techniques
//--------------------------------------------------------------------------------------

technique11 Default
{
    pass P0
    {          
        SetVertexShader( CompileShader( vs_4_0, DefaultVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, DefaultPS() ) );

		SetRasterizerState(DefaultRasterizerState);
        SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( DefaultBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }

	/*
    pass P1
    {          
        SetVertexShader( CompileShader( vs_4_0, DefaultVS() ) );
		SetGeometryShader(CompileShader(gs_4_0, NormalGS())); 
        SetPixelShader( CompileShader( ps_4_0, NormalPS() ) );

		SetRasterizerState(DefaultRasterizerState);
        SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( DefaultBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
	*/
}