//--------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinga Tekniska H�gskola.
//	//**TODO:implement**
//--------------------------------------------------------------------------------------
//#include "stdafx.fx"

//-----------------------------------------------------------------------------------------
//	Global variables (non-numeric values cannot be added to a constantbuffer.)
//-----------------------------------------------------------------------------------------
//Textures used to make the blend map
Texture2D tex1; //R-channel in blendmap. ex: grass
Texture2D tex2; //G-channel in blendmap. ex: dirt
Texture2D tex3; //B-channel in blendmap. ex: leaves
Texture2D<float4> blendMap;
//Texture2D tex4; //**extra, ex: blood, footprints**

//-----------------------------------------------------------------------------------------
// Constant buffers
//-----------------------------------------------------------------------------------------
cbuffer PerFrame
{
	float4 cameraPosition;
};
cbuffer PerObject
{
	matrix	WVP;
	matrix	worldMatrix;
	matrix	worldMatrixInverseTranspose;

	bool	textured;
	
	float	specularPower;
	float4	specularColor;
	float4	diffuseColor;
	float4	ambientLight;
};

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
struct VSIn
{
	float4 pos		: POSITION; //3 anv�nds**
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float4 color	: COLOR; //3 anv'nds**
};

struct PSSceneIn
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD;
	float3 norm		: NORMAL;
	float4 color	: COLOR;

	float4 posW		: POSITION;	//world position
};

struct PSOut			
{
	float4 Texture			: SV_TARGET0;	//Texture XYZ, unused W
	float4 NormalAndDepth	: SV_TARGET1;	//Normal XYZ, depth W
	float4 Position			: SV_TARGET2;	//Position XYZ, unused W
	float4 Specular			: SV_TARGET3;	//Specular XYZ, specular power W
};


//-----------------------------------------------------------------------------------------
// **states tmp**
//-----------------------------------------------------------------------------------------
RasterizerState BackCulling
{
	CullMode = Back;
};
SamplerState LinearWrapSampler
{
	Filter = MIN_MAG_MIP_LINEAR; 
	AddressU = Wrap;
	AddressV = Wrap;
};
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	input.color.w = 1.0f;
	input.pos.w = 1.0f;

	PSSceneIn output = (PSSceneIn)0;
	output.pos = mul(input.pos, WVP);
	output.posW = mul(input.pos, worldMatrix);
	output.tex = input.tex;
	output.norm = normalize(mul(input.norm, (float3x3)worldMatrixInverseTranspose));
	output.color = input.color;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSScene
//-----------------------------------------------------------------------------------------
PSOut PSScene(PSSceneIn input) : SV_Target
{	
	PSOut output = (PSOut)0; //**

	//Texture RT
	float3 tex1Color = float3(0.0f, 0.0f, 0.0f);
	float3 tex2Color = float3(0.0f, 0.0f, 0.0f);
	float3 tex3Color = float3(0.0f, 0.0f, 0.0f);
	float3 blendMapColor = float3(0.0f, 0.0f, 0.0f);
	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	if(textured) 
	{
		//finalColor = tex3.Sample(LinearWrapSampler, input.tex).xyz * diffuseColor; //debug
		//finalColor = blendMap.Sample(LinearWrapSampler, input.tex).rgb; //Debug
		
		//Sample textures
		tex1Color = tex1.Sample(LinearWrapSampler, input.tex).rgb; 
		tex2Color = tex2.Sample(LinearWrapSampler, input.tex).rgb;
		tex3Color = tex3.Sample(LinearWrapSampler, input.tex).rgb;
		blendMapColor = blendMap.Sample(LinearWrapSampler, input.tex).rgb;

		//Inverse of all blend weights to scale final color to be in range [0,1]
		float inverseTotal = 1.0f / (blendMapColor.r + blendMapColor.g + blendMapColor.b);

		//Scale color for each texture by the weight in the blendmap and scale to [0,1]
		tex1Color *= blendMapColor.r * inverseTotal;
		tex2Color *= blendMapColor.g * inverseTotal;
		tex3Color *= blendMapColor.b * inverseTotal;

		//Blendmapped color
		finalColor = (tex1Color + tex2Color + tex3Color) * diffuseColor.rgb;
		
	}
	else
	{
		finalColor = input.color.rgb; //Geometry color
	}
	output.Texture.xyz = finalColor;
	output.Texture.w = -1.0f;

	//NormalAndDepth RT
	output.NormalAndDepth = float4(input.norm.xyz, input.pos.z / input.pos.w);	
	float depth = length(cameraPosition.xyz - input.posW.xyz) / 200.0f;		// Haxfix
	output.NormalAndDepth.w = depth;

	//Position RT
	output.Position = input.posW;
	
	//Specular RT
	output.Specular = specularColor;
	output.Specular.w = specularPower;

	return output;
}


//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 DeferredGeometryBlendMapTech
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