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

//
//	00	01	02
//	10	11	12
//	20	21	22
//
//http://stackoverflow.com/questions/983999/simple-3x3-matrix-inverse-code-c TILLE
/*

	double A11, A12, A13;
    double A21, A22, A23;
    double A31, A32, A33;

    double B11, B12, B13;
    double B21, B22, B23;
    double B31, B32, B33;


    B11 = 1 / ((A22 * A33) - (A23 * A32));
    B12 = 1 / ((A13 * A32) - (A12 * A33));
    B13 = 1 / ((A12 * A23) - (A13 * A22));
    B21 = 1 / ((A23 * A31) - (A21 * A33));
    B22 = 1 / ((A11 * A33) - (A13 * A31));
    B23 = 1 / ((A13 * A21) - (A11 * A23));
    B31 = 1 / ((A21 * A32) - (A22 * A31));
    B32 = 1 / ((A12 * A31) - (A11 * A32));
    B33 = 1 / ((A11 * A22) - (A12 * A21));


float3x3 MatrixInverse(in float3x3 A)
{
	/*float det = 
	  A[0][2] * A[2][1] * A[1][0]
	+ A[0][1] * A[1][2] * A[2][0]
	+ A[0][0] * A[1][1] * A[2][2]
	- A[0][0] * A[2][1] * A[1][2]
	- A[1][0] * A[0][1] * A[2][2]
	- A[2][0] * A[1][1] * A[0][2];
	*/
	/*float det = determinant(A);
	float3x3 tmp;
	tmp[0][0] = A[1][2] * A[2][2] - A[2][1] * A[1][2];
	tmp[1][0] = A[1][2] * A[2][0] - A[2][2] * A[1][0];
	tmp[2][0] = A[1][0] * A[2][1] - A[2][0] * A[1][1];
	
	tmp[0][1] = A[0][2] * A[2][1] - A[2][2] * A[0][1];
	tmp[1][1] = A[0][0] * A[2][2] - A[2][0] * A[0][2];
	tmp[2][1] = A[0][1] * A[2][0] - A[2][1] * A[0][0];

	tmp[0][2] = A[0][1] * A[1][2] - A[1][1] * A[0][2];
	tmp[1][2] = A[0][2] * A[1][0] - A[1][2] * A[0][0];
	tmp[2][2] = A[0][0] * A[1][1] - A[1][0] * A[0][1];

	float3x3 AInv = 1.0f / det * tmp;

	return AInv;
}*/

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
	/*float4 pos_texU		: POSITION_AND_TEXU;
	float4 texV_norm	: TEXV_AND_NORM;
	float3 color		: COLOR;
	float3 tangent		: TANGENT;
	//float3 binormal : BINORMAL;
	
	float4 pos_texU_morph	: POSITION_AND_TEXU_MORPH;
	float4 texV_norm_morph	: TEXV_AND_NORM_MORPH;
	float3 color_morph		: COLOR_MORPH;
	float3 tangent_morph	: TANGENT_MORPH;
	//float3 binormal_morph : BINORMAL_MORPH;

	
	row_major float4x4 world					: WORLD; //[0][3] contains interpolation value
	row_major float4x4 worldInverseTranspose	: WIT; //**kan ändra till 3x3(inte world dock)
	*/
	
	
	
	//Buffer 1
	float3 pos :	POSITION;
	float2 texCoord : TEXCOORD;
	float3 norm	: NORMAL;
	float3 color : COLOR;
	float3 tangent : TANGENT;
	//float3 binormal : BINORMAL;
	
	//Buffer 2
	float3 pos_morph :	POSITION_MORPH;
	float2 texCoord_morph : TEXCOORD_MORPH;
	float3 norm_morph	: NORMAL_MORPH;
	float3 color_morph : COLOR_MORPH;
	float3 tangent_morph : TANGENT_MORPH;
	//float3 binormal_morph : BINORMAL_MORPH;

	//Buffer 3 - Instance data
	row_major float4x4 world					: WORLD; //[0][3] contains interpolation value


	//TEST
	/*
	//Buffer 1
	float3 pos :	POSITION;
	float2 texCoord : TEXCOORD;
	float3 norm	: NORMAL;
	//float3 color : COLOR;
	float3 tangent : TANGENT;
	//float3 binormal : BINORMAL;
	
	//Buffer 2
	float3 pos_morph :	POSITION_MORPH;
	float2 texCoord_morph : TEXCOORD_MORPH;
	float3 norm_morph	: NORMAL_MORPH;
	//float3 color_morph : COLOR_MORPH;
	float3 tangent_morph : TANGENT_MORPH;
	//float3 binormal_morph : BINORMAL_MORPH;

	//Buffer 3 - Instance data
	row_major float4x4 world					: WORLD; //[0][3] contains interpolation value
	row_major float4x4 worldInverseTranspose	: WIT; //**kan ändra till 3x3(inte world dock)
	*/
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
	
	//Fetch interpolation value stored in last element.
	float interpolationValue = input.world[3][3];
	//Set last element to 1 again
	input.world[3][3] = 1;

	/*output.worldPos = mul(lerp(float4(input.pos_texU.xyz, 1.0f), float4(input.pos_texU_morph.xyz, 1.0f), interpolationValue), input.world); 
	output.pos		= mul(float4(output.worldPos.xyz, 1.0f), g_CamViewProj);
	output.tex		= lerp(float2(input.pos_texU.w, input.texV_norm.x), float2(input.pos_texU_morph.w, input.texV_norm_morph.x), interpolationValue);
	output.norm		= normalize(lerp(input.texV_norm.yzw, input.texV_norm_morph.yzw, interpolationValue));
	output.tangent	= normalize(lerp(input.tangent, input.tangent_morph, interpolationValue));
	output.binormal = cross(output.norm, output.tangent);
	*/
	//float3x3 test = MatrixInverse((float3x3)input.world);
	//float3x3 test2 = transpose(test);



	output.worldPos = mul(lerp(float4(input.pos, 1.0f), float4(input.pos_morph, 1.0f), interpolationValue), input.world); 
	output.pos		= mul(float4(output.worldPos.xyz, 1.0f), g_CamViewProj);
	output.tex		= lerp(input.texCoord, input.texCoord_morph, interpolationValue);
	output.norm		= normalize(mul(lerp(input.norm, input.norm_morph, input.norm_morph), input.world));//normalize(mul(lerp(input.norm, input.norm_morph, interpolationValue), (float3x3)test2));
	output.tangent	= normalize(mul(lerp(input.tangent, input.tangent_morph, interpolationValue), input.world)); //normalize(mul(lerp(input.tangent, input.tangent_morph, interpolationValue), (float3x3)test2));
	output.binormal = cross(output.norm, output.tangent);
	
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

	float depth = length(g_CamPos.xyz - input.worldPos.xyz) / g_FarClip;		// Haxfix

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
	
	output.NormalAndDepth.w = depth;//0.001f; 
	
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
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}