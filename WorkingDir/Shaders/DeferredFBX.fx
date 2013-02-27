//--------------------------------------------------------------------------------------
// File: BTTFBXViewer.fx
//--------------------------------------------------------------------------------------
#define MaxBones 100

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
matrix gWVP;					// Model * View * Projection
matrix gWorld;					// World matrix
matrix gView;					// View
matrix gProj;					// Projection
matrix gViewProj;				// View * Project
matrix g_mScale;				// Scale matrix

// Bones
matrix g_mBonesArray[MaxBones];
bool g_bSkinning = true;

cbuffer buff
{
	float4 CameraPosition;
	float FarClip;
	bool useNormalMap;

	uint specialColor;
	float4 AmbientLight; //**tillman opt - används inte
	float SpecularPower;
	float4 SpecularColor;
	float4 DiffuseColor;
};


cbuffer cbImmutable
{
	float3 g_vLight = normalize(float3( 0, 0, -1.0f ));
};

RasterizerState DefaultRasterizerState
{
	CullMode = Front;
	FillMode = Solid;
};

RasterizerState NoCullRasterizerState
{
	CullMode = None;
};

BlendState DefaultBlendState
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	RenderTargetWriteMask[0] = 0x0f;
};


SamplerState AnisotropicWrapSampler 
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
	MipLODBias = 0.0f;//Unused
	MaxAnisotropy = 16;
	ComparisonFunc = ALWAYS; 
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //Unused
	MinLOD = 0.0f;
	MaxLOD = 16.0f; //Max
};
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};
BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};
RasterizerState BackCulling
{
	CullMode = Front;	// Should be back but FBX is inverted.
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

struct VSIn
{
	float3 vPos : SV_POSITION;
	float3 vNormal : NORMAL;
	float3 vTangent : TANGENT;
	float2 vTexCoord : TEXCOORD;
	float4 vBoneIndices : BLENDINDICES;
	float4 vBoneWeights : BLENDWEIGHT;
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;

	float4 WorldPos : POSITION;
};

struct PSout
{
	float4 Texture : SV_TARGET0;
	float4 NormalAndDepth : SV_TARGET1;
	float4 Position : SV_TARGET2;
	float4 Specular : SV_TARGET3;
};

/*
RTs:
1: Texture XYZ, W Special Color
2: Normal XYZ, W Depth
3: Position XYZ, W unused
4: Specular XYZ, W Specular Power

*/

//--------------------------------------------------------------------------------------
// Default shaders
//--------------------------------------------------------------------------------------

PSSceneIn DefaultVS(VSIn input)
{
    PSSceneIn Output;

	float4 position = float4(input.vPos, 1.0f);
	float3 normal = 0;
	float3 tangent = 0;

	if( g_bSkinning )
	{
		float4x4 skinTransform = 0;
		skinTransform += g_mBonesArray[input.vBoneIndices.x] * input.vBoneWeights.x;
		skinTransform += g_mBonesArray[input.vBoneIndices.y] * input.vBoneWeights.y;
		skinTransform += g_mBonesArray[input.vBoneIndices.z] * input.vBoneWeights.z;
		skinTransform += g_mBonesArray[input.vBoneIndices.w] * input.vBoneWeights.w;

		position = mul(position, skinTransform);
		normal = normalize(mul(input.vNormal, (float3x3)skinTransform));
		tangent = normalize(mul(input.vTangent, (float3x3)skinTransform));
	}
	else
	{
		position = mul(position, gWorld);
		normal = mul(input.vNormal, gWorld);
		tangent = mul(input.vTangent, gWorld);

		//input.vTexCoord.x = ( 1.0f - input.vTexCoord.x );
	}

	// Ininput.vert X
	position.x *= -1.0f;

	// Scale Position
	position = mul(position, g_mScale);
	
	// Output
	Output.WorldPos = position;
	Output.Pos = mul(position, gViewProj);
	Output.norm = normal; //mul(float4(normal, 0), g_mScale);
	Output.Tangent = mul(float4(tangent, 0), g_mScale);
	Output.tex = input.vTexCoord;
    return Output;
}

//--------------------------------------------------------------------------------------
PSout DefaultPS( PSSceneIn input ) 
{ 
	float4 textureColor = txDiffuse.Sample(AnisotropicWrapSampler, input.tex);
	if ( textureColor.a < 0.5f )
		discard;
	
	float4 finalColor = textureColor * DiffuseColor;
	finalColor.w = (float)specialColor;

	PSout output;
	output.Texture = finalColor;
	output.NormalAndDepth = float4(input.norm.xyz, input.Pos.z / input.Pos.w);		// pos.z / pos.w should work?

	float depth = length(CameraPosition.xyz - input.WorldPos.xyz) / FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	output.Position.xyz = input.WorldPos.xyz;
	output.Position.w = -1.0f;

	output.Specular = SpecularColor;
	output.Specular.w = SpecularPower;

	//output.Texture.xyz = output.NormalAndDepth.xyz;
	
	

	// NormalMap
	float4 bumpMap = txNormal.Sample(AnisotropicWrapSampler, input.tex);
	// Expand the range of the normal value from (0, +1) to (-1, +1).
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	// Calculate the normal from the data in the bump map.
	float3 binormal = cross(input.norm, input.Tangent);
	float3 bumpNormal = input.norm + bumpMap.x * input.Tangent + bumpMap.y * binormal;
	// Normalize the resulting bump normal.
	bumpNormal = normalize(bumpNormal);
	output.NormalAndDepth.xyz = bumpNormal.xyz;
	// NormalMap
	
	

	return output;
}



technique11 Default
{
    pass P0
    {          
        SetVertexShader( CompileShader( vs_4_0, DefaultVS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, DefaultPS() ) );

		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}