//--------------------------------------------------------------------------------------
// File: BTTFBXViewer.fx
//--------------------------------------------------------------------------------------
#define MaxBones 100

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
matrix gWorld;					// World matrix
matrix gViewProj;				// View * Project
matrix g_mScale;				// Scale matrix

// Bones
matrix g_mBonesArray[MaxBones];
bool g_bSkinning = true;

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
	CullMode = Front;	// Should be front but FBX is inverted.
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
};

//--------------------------------------------------------------------------------------
// Default shaders
//--------------------------------------------------------------------------------------

PSSceneIn DefaultVS(VSIn input)
{
    PSSceneIn Output;

	float4 position = float4(input.vPos, 1.0f);

	if( g_bSkinning )
	{
		float4x4 skinTransform = 0;
		skinTransform += g_mBonesArray[input.vBoneIndices.x] * input.vBoneWeights.x;
		skinTransform += g_mBonesArray[input.vBoneIndices.y] * input.vBoneWeights.y;
		skinTransform += g_mBonesArray[input.vBoneIndices.z] * input.vBoneWeights.z;
		skinTransform += g_mBonesArray[input.vBoneIndices.w] * input.vBoneWeights.w;

		position = mul(position, skinTransform);
	}
	else
	{
		position = mul(position, gWorld);
	}

	// Ininput.vert X
	position.x *= -1.0f;

	// Scale Position
	position = mul(position, g_mScale);
	
	// Output
	Output.Pos = mul(position, gViewProj);
    return Output;
}

//--------------------------------------------------------------------------------------
float DefaultPS( PSSceneIn input ) : SV_Depth
{ 
	return input.Pos.z;
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