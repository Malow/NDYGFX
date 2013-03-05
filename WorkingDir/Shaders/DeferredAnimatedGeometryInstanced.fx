//--------------------------------------------------------------------------------------
//	Original Code taken from DeferredAnimatedGeometry.fx.
//
//	Written by Markus Tillman for projekt NotDeadYet at Blekinga tekniska högskola.
//
//	Shader for drawing (morph)animated geometry. Due to vertex shader input limitations, 
//	color interpolation is not supported.
//--------------------------------------------------------------------------------------

#include "stdafx.fx"
Texture2D g_DiffuseMap;
Texture2D g_NormalMap;


//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
cbuffer EveryFrame
{
	float		g_FarClip;
	float3		g_CamPos;
	float4x4	g_CamViewProj;
};

cbuffer EveryStrip //(every 2 strips)
{
	//Textures
	bool	g_Textured;
	bool	g_UseNormalMap;
	//Material
	float4	g_DiffuseColor; 
	float3	g_SpecularColor;
	float	g_SpecularPower;
};


struct VSIn
{
	//**TEST
	/*float3 pos :	POSITION;
	float2 texCoord : TEXCOORD;
	float3 norm	: NORMAL;
	float3 color : COLOR;

	float3 pos_morph :	POSITION_MORPH;
	float2 texCoord_morph : TEXCOORD_MORPH;
	float3 norm_morph	: NORMAL_MORPH;
	float3 color_morph : COLOR_MORPH;
	*/
	float3 pos :	POSITION;
	float2 texCoord : TEXCOORD;
	float3 norm	: NORMAL;
	float3 color : COLOR;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;

	float3 pos_morph :	POSITION_MORPH;
	float2 texCoord_morph : TEXCOORD_MORPH;
	float3 norm_morph	: NORMAL_MORPH;
	float3 color_morph : COLOR_MORPH;
	float3 tangent_morph : TANGENT_MORPH;
	float3 binormal_morph : BINORMAL_MORPH;
	


	//Buffer 1
	/*float4 pos_TexU		: POSITION_TEX_U;
	float4 texV_Norm	: TEX_V_NORMAL;
	float3 tangent		: TANGENT;
	float3 binormal		: BINORMAL;

	//Buffer 2
	float4 pos_TexU_Morph	: POSITION_TEX_U_MORPH;
	float4 texV_Norm_Morph	: TEX_V_NORMAL_MORPH;
	float3 tangent_Morph	: TANGENT_MORPH;
	float3 binormal_Morph	: BINORMAL_MORPH;*/


	//Buffer 3 - Instance data
	row_major float4x4 world					: WORLD; //[0][3] contains interpolation value
	//row_major float4x4 worldInverseTranspose	: WIT; //**kan ändra till 3x3(inte world dock)
};


struct PSSceneIn
{
	float4 worldPos : POSITION;
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal : BINORMAL;
};

struct PSout
{
	float4 Texture			: SV_TARGET0;
	float4 NormalAndDepth	: SV_TARGET1;
	float4 Position			: SV_TARGET2;
	float4 Specular			: SV_TARGET3;
};

/*
RTs:
1: Texture XYZ, W Special Color
2: Normal XYZ, W Depth
3: Position XYZ, W Type of object(unused)
4: Specular XYZ, W Specular Power


*/

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	/*float2 texCoords		= float2(input.pos_TexU.w,			input.texV_Norm.x);
	float2 texCoordsMorph	= float2(input.pos_TexU_Morph.w,	input.texV_Norm_Morph.x);
	

	//tillman obs; spaces **
	output.worldPos		=			mul(lerp(float4(input.pos_TexU.xyz, 1.0f),	float4(input.pos_TexU_Morph.xyz, 1.0f), g_InterpolationValue),	world);
	output.pos			= output.worldPos;//			mul(	 float4(output.worldPos.xyz, 1.0f),																	g_CamViewProj);
	output.tex			=				lerp(texCoords,							texCoordsMorph,							g_InterpolationValue);
	output.norm			= normalize(mul(lerp(input.texV_Norm.yzw,				input.texV_Norm_Morph.yzw,				g_InterpolationValue),	(float3x3)input.worldInverseTranspose));
	output.tangent		= normalize(mul(lerp(input.tangent,						input.tangent_Morph,					g_InterpolationValue),	(float3x3)input.worldInverseTranspose)); //**tillman, ev ta bort (float3x3)
	output.binormal		= normalize(mul(lerp(input.binormal,					input.binormal_Morph,					g_InterpolationValue),	(float3x3)input.worldInverseTranspose));
	*/
	float interpolationValue = input.world[0][3];
	output.worldPos = mul(lerp(float4(input.pos, 1.0f), float4(input.pos_morph, 1.0f), interpolationValue), input.world); //**TILLMAN OBS!, interpolation i .14
	output.pos	= mul(float4(output.worldPos.xyz, 1.0f), g_CamViewProj);
	output.tex		= lerp(input.texCoord, input.texCoord_morph, interpolationValue);
	output.norm	=	lerp(input.norm, input.norm_morph, interpolationValue);
	output.tangent	= lerp(input.tangent, input.tangent_morph, interpolationValue);
	output.binormal	= lerp(input.binormal, input.binormal_morph, interpolationValue);
	
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
PSout PSScene(PSSceneIn input)
{	
	PSout output;

	//Texture
	float4 textureColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if(g_Textured)
	{
		textureColor = g_DiffuseMap.Sample(LinearWrapSampler, input.tex);
		if ( textureColor.a < 0.5f )
			discard;
	}
	float4 finalColor = textureColor * g_DiffuseColor; 
	finalColor.w = 0.0f;
	output.Texture = finalColor; 

	//Normal and depth
	if(g_UseNormalMap)
	{
		float4 bumpMap = g_NormalMap.Sample(LinearWrapSampler, input.tex);
		// Expand the range of the normal value from (0, +1) to (-1, +1).
		bumpMap = (bumpMap * 2.0f) - 1.0f;
		// Calculate the normal from the data in the bump map.
		float3 bumpNormal = input.norm + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
		// Normalize the resulting bump normal.
		bumpNormal = normalize(bumpNormal);
		output.NormalAndDepth.xyz = bumpNormal.xyz;
	}
	else
	{
		output.NormalAndDepth = float4(input.norm.xyz, input.pos.z / input.pos.w);		// pos.z / pos.w should work?
	}
	float depth = length(CameraPosition.xyz - input.worldPos.xyz) / g_FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;
	
	//Position and object type(unused)
	output.Position.xyz = input.worldPos.xyz;
	output.Position.w = -1.0f;

	//Specular
	output.Specular.xyz = g_SpecularColor;
	output.Specular.w = g_SpecularPower;

	return output;
}


//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 DeferredAnimatedGeometryInstanced
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