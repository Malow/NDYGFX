//--------------------------------------------------------------------------------------
//	Original Code taken from DeferredGeometry.fx.
//
//	Written by Markus Tillman for projekt NotDeadYet at Blekinga tekniska högskola.
//
//--------------------------------------------------------------------------------------

// Marcus Löwegren
//#include "stdafx.fx"

// For textures
Texture2D g_DiffuseMap;
Texture2D g_NormalMap;

SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

cbuffer EveryFrame
{
	float3		g_CamPos;
	float4x4	g_CamViewProj;
	float		g_FarClip;
	
	float4x4 g_TestW;
	float4x4 g_TestWIT;
	//uint InstanceId : SV_InstanceID; använda för färg? eller använda position & sin()?
};

cbuffer EveryMesh
{
	uint specialColor;
};

cbuffer EveryStrip
{
	//matrix WVP;
	//matrix worldMatrix;
	//matrix worldMatrixInverseTranspose;
	bool g_Textured;
	bool g_UseNormalMap;
	float4 g_DiffuseColor;
	float  g_SpecularPower;
	float4 g_SpecularColor;
};



struct VSIn
{
	float3 Pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float3 color : COLOR;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	//instance data
	float4x4 world					: WORLD;
	//float4x4 worldInverseTranspose	: WIT;
	
	//uint InstanceId : SV_InstanceID;
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float3 color : COLOR;
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

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output = (PSSceneIn)0;

	//output.Pos = mul(float4(input.Pos, 1.0f), mul(g_TestW, g_CamViewProj));//input.world * g_CamViewProj);
	//output.WorldPos = mul(float4(input.Pos, 1.0f), g_TestW);//input.world);
	
	/*float4x4 testW2;
	testW2[0] = float4(1, 0, 0, 0);
	testW2[1] = float4(0, 1, 0, 0);
	testW2[2] = float4(0, 0, 1, 0);
	testW2[3] = float4(0, 0, 0, 1);
	float4x4 WVP = mul(testW2, g_CamViewProj); 
		
	output.Pos = mul(float4(input.Pos, 1.0f), WVP);
	output.WorldPos = mul(float4(input.Pos, 1.0f), testW2);
	
	*/

	output.Pos = mul(float4(input.Pos, 1.0f), mul(input.world, g_CamViewProj));
	output.WorldPos = mul(float4(input.Pos, 1.0f), input.world);

	output.tex = input.tex;
	output.norm = input.norm;
	output.Tangent = input.Tangent;
	output.Binormal = input.Binormal;
	/*output.norm = normalize(mul(input.norm, (float3x3)g_TestWIT));//input.worldInverseTranspose));
	output.Tangent = normalize(mul(input.Tangent, (float3x3)g_TestWIT));//input.worldInverseTranspose));
	output.Binormal = normalize(mul(input.Binormal, (float3x3)g_TestWIT));//input.worldInverseTranspose));
	*//*output.norm = normalize(mul(input.norm, (float3x3)input.worldInverseTranspose));
	output.Tangent = normalize(mul(input.Tangent, (float3x3)input.worldInverseTranspose));
	output.Binormal = normalize(mul(input.Binormal, (float3x3)input.worldInverseTranspose));
	*/output.color = input.color;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
PSout PSScene(PSSceneIn input) : SV_Target
{	
	float4 textureColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if(g_Textured)
	{
		textureColor = g_DiffuseMap.Sample(linearSampler, input.tex);
		
		if ( textureColor.a < 0.5f )
			discard;
	}
	float4 finalColor = (textureColor + float4(input.color, 0.0f)) * g_DiffuseColor;
	finalColor.w = (float)specialColor;



	PSout output;
	output.Texture = float4(1,0,0,1);//finalColor;
	output.NormalAndDepth = float4(input.norm.xyz, input.Pos.z / input.Pos.w);		// pos.z / pos.w should work?

	float depth = length(g_CamPos - input.WorldPos.xyz) / g_FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	output.Position.xyz = input.WorldPos.xyz;
	output.Position.w = -1.0f;

	output.Specular = g_SpecularColor;
	output.Specular.w = g_SpecularPower;
	
	if(g_UseNormalMap)
	{
		// NormalMap
		float4 bumpMap = g_NormalMap.Sample(linearSampler, input.tex);
		// Expand the range of the normal value from (0, +1) to (-1, +1).
		bumpMap = (bumpMap * 2.0f) - 1.0f;
		// Calculate the normal from the data in the bump map.
		float3 bumpNormal = input.norm + bumpMap.x * input.Tangent + bumpMap.y * input.Binormal;
		// Normalize the resulting bump normal.
		bumpNormal = normalize(bumpNormal);
		output.NormalAndDepth.xyz = bumpNormal.xyz;
		// NormalMap
	}
	
	return output;
}
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};
RasterizerState BackCulling
{
	CullMode = Back;
};

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 DeferredGeometryInstanced
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
    }  
}