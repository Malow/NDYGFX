//-----------------------------------------------------------------------------------------
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
//Texture2D<uint> AIMap;
Texture2D AIMap;
//Texture2D<int> AIMap; //**uint


//-----------------------------------------------------------------------------------------
// Constant buffers
//-----------------------------------------------------------------------------------------
cbuffer PerObject
{
	//Matrices
	matrix	WVP;
	matrix	worldMatrix;
	matrix	worldMatrixInverseTranspose;

	//Texture
	bool	textured;
	bool	blendMapped;
	float	textureScale;

	//Material
	float	specularPower;
	float3	specularColor;
	float3	diffuseColor;

	//AI(editor)
	bool useAIMap;
	float nodesPerSide;
	float AIGridThickness;
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
	float4 Texture			: SV_TARGET0;	//Texture XYZ, unused W
	float4 NormalAndDepth	: SV_TARGET1;	//Normal XYZ, depth W
	float4 Position			: SV_TARGET2;	//Position XYZ, unused W
	float4 Specular			: SV_TARGET3;	//Specular XYZ, specular power W
};

float3 RenderTextured(float scale, float2 tex, bool useBlendMap)
{
	//Sample R,G,B,A textures
	float2 texCoord = scale * tex;
	float3 tex1Color = tex0.Sample(LinearWrapSampler, texCoord).rgb; //**tillman opti, FORMAT = RGB och inte A**
	float3 tex2Color = tex1.Sample(LinearWrapSampler, texCoord).rgb; //**tillman opti, FORMAT = RGB och inte A**
	float3 tex3Color = tex2.Sample(LinearWrapSampler, texCoord).rgb; //**tillman opti, FORMAT = RGB och inte A**
	float3 tex4Color = tex3.Sample(LinearWrapSampler, texCoord).rgb; //**tillman opti, FORMAT = RGB och inte A**
		
	if(useBlendMap)
	{
		//Sample blend map texture
		float4 blendMapColor = normalize(blendMap.Sample(LinearClampSampler, tex)); //normalize (don't use texture scaling).
		
		//Inverse of all blend weights to scale final color to be in range [0,1]
		float inverseTotal = 1.0f / (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a);

		//Scale color for each texture by the weight in the blendmap and scale to [0,1]
		tex1Color *= blendMapColor.r * inverseTotal;
		tex2Color *= blendMapColor.g * inverseTotal;
		tex3Color *= blendMapColor.b * inverseTotal;
		tex4Color *= blendMapColor.a * inverseTotal;

		//Blendmapped color (normalize it)
		return (tex1Color + tex2Color + tex3Color + tex4Color) * diffuseColor;
	}
	else
	{
		//Saturated color
		return saturate((tex1Color + tex2Color + tex3Color + tex4Color) * diffuseColor);
	}
}

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
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

	if(useAIMap) //AIMap color
	{
		float boolColor = AIMap.Sample(PointClampSampler, input.tex).r;

		if(textured)
		{
			finalColor = RenderTextured(textureScale, input.tex, blendMapped);
		}
		//If node is blocked, add 50% redness (true = 1 = red).
		if(boolColor == 1.0f)
		{
			finalColor.r += 0.5f; //Render non-traversable/blocked nodes
			finalColor.r *= 0.66666666f;
		}

		//Render grid
		float deltaXY = 1.0f / nodesPerSide;
		float texX = fmod(input.tex.x, deltaXY);
		float texY = fmod(input.tex.y, deltaXY);
		/*if(	texX > 0.0f && texX < AIGridThickness || 
			texY >  0.0f && texY < AIGridThickness) 
		{*/
		if(texX > deltaXY - (AIGridThickness * 0.5f) || texX < (AIGridThickness * 0.5f) || 
			texY > deltaXY - (AIGridThickness * 0.5f) || texY < (AIGridThickness * 0.5f)) 
		{
			finalColor.rgb = 1.0f;
		}
	}
	else if(textured) //Texture color(s)
	{
		finalColor = RenderTextured(textureScale, input.tex, blendMapped);
	}
	else //Geometry color
	{
		finalColor = input.color; 
	}

	//RENDERTARGET DATA:
	//Texture RT
	output.Texture.xyz = finalColor;
	output.Texture.w = -1.0f;

	//NormalAndDepth RT
	output.NormalAndDepth = float4(input.norm, input.pos.z / input.pos.w);	
	float depth = length(CameraPosition.xyz - input.posW.xyz) / FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	//Position RT
	output.Position = input.posW;
	
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
}