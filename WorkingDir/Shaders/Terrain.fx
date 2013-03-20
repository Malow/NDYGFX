/*//-----------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
//	
//	This shader renders geometry (deferred with 4 GBuffers) with up to 4 textures(blendmap).
//-----------------------------------------------------------------------------------------
#include "stdafx.fx"

//-----------------------------------------------------------------------------------------
//	Global variables (non-numeric values cannot be added to a constantbuffer.)
//-----------------------------------------------------------------------------------------
//Textures used to make the blend map
Texture2D tex0; //R-channel in blendmap. ex: grass
Texture2D tex1; //G-channel in blendmap. ex: dirt
Texture2D tex2; //B-channel in blendmap. ex: leaves
Texture2D tex3; //A-channel in blendmap. ex: extra
Texture2D<float4> blendMap;


//-----------------------------------------------------------------------------------------
// Constant buffers
//-----------------------------------------------------------------------------------------
cbuffer PerFrame
{
	float gFarClip; //TODO: send from CPU

};
cbuffer PerObject
{
	//Matrices
	matrix	WVP;
	matrix	worldMatrix;
	matrix	worldMatrixInverseTranspose;

	//Texture
	bool	textured;
	bool	blendMapped;
	float	texScale;

	//Material
	float	specularPower;
	float3	specularColor;
	float3	diffuseColor;
};

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
struct VSIn
{
	float3 pos		: POSITION;
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float3 color	: COLOR;
};

struct PSSceneIn
{
	float4 pos		: SV_POSITION; 
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float3 color	: COLOR;

	float4 posW		: POSITION;	//world position 
};

struct PSOut			
{
	float4 Texture			: SV_TARGET0;	//Texture XYZ, Special color(unused in this shader)
	float4 NormalAndDepth	: SV_TARGET1;	//Normal XYZ, depth W
	float4 Position			: SV_TARGET2;	//Position XYZ, Type of object
	float4 Specular			: SV_TARGET3;	//Specular XYZ, specular power W
};



//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	//input.color.w = 1.0f;
	//input.pos.w = 1.0f; //**

	PSSceneIn output = (PSSceneIn)0;
	output.pos = mul(float4(input.pos, 1.0f), WVP);
	output.tex = input.tex;
	output.norm = normalize(mul(input.norm, (float3x3)worldMatrixInverseTranspose));
	output.color = input.color;
	output.posW = mul(float4(input.pos, 1.0f), worldMatrix);

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSScene
//-----------------------------------------------------------------------------------------
PSOut PSScene(PSSceneIn input) : SV_Target
{	
	PSOut output = (PSOut)0; 

	//Texture RT
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);
	if(textured) 
	{
		//finalColor = tex3.Sample(LinearWrapSampler, input.tex).xyz * diffuseColor; //debug
		//finalColor = blendMap.Sample(LinearWrapSampler, input.tex).rgb; //Debug
		
		//Sample R,G,B,A textures
		float2 texCoord = input.tex * texScale;
		float3 tex1Color = tex0.Sample(LinearWrapSampler, texCoord).rgb; 
		float3 tex2Color = tex1.Sample(LinearWrapSampler, texCoord).rgb; 
		float3 tex3Color = tex2.Sample(LinearWrapSampler, texCoord).rgb; 
		float3 tex4Color = tex3.Sample(LinearWrapSampler, texCoord).rgb; 
		
		if(blendMapped)
		{
			//Sample blend map texture
			float4 blendMapColor = normalize(blendMap.Sample(LinearClampSampler, input.tex)); //normalize
		
			//Inverse of all blend weights to scale final color to be in range [0,1]
			float inverseTotal = 1.0f / (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a);

			//Scale color for each texture by the weight in the blendmap and scale to [0,1]
			tex1Color *= blendMapColor.r * inverseTotal;
			tex2Color *= blendMapColor.g * inverseTotal;
			tex3Color *= blendMapColor.b * inverseTotal;
			tex4Color *= blendMapColor.a * inverseTotal;

			//Blendmapped color (normalize it)
			finalColor = (tex1Color + tex2Color + tex3Color + tex4Color) * diffuseColor;
		}
		else
		{
			//Saturated color
			finalColor = saturate((tex1Color + tex2Color + tex3Color + tex4Color) * diffuseColor);
		}
	}
	else
	{
		finalColor = input.color; //Geometry color
	}
	output.Texture.xyz = finalColor;
	output.Texture.w = -1.0f;

	//NormalAndDepth RT
	output.NormalAndDepth = float4(input.norm, input.pos.z / input.pos.w);	
	float depth = length(CameraPosition.xyz - input.posW.xyz) / gFarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	//Position RT
	output.Position.xyz = input.posW.xyz;
	output.Position.w = OBJECT_TYPE_TERRAIN; //See stdafx.fx for object types.
	
	//Specular RT
	output.Specular.xyz = specularColor;
	output.Specular.w = specularPower;

	return output;
}


//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 TerrainEditorTech
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
}*/