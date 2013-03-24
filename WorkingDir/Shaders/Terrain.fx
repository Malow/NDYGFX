//-----------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
//	
//	This shader renders geometry (deferred with 4 GBuffers) with up to 8 textures with
//	2 blendmaps.
//-----------------------------------------------------------------------------------------
#include "stdafx.fx"

//-----------------------------------------------------------------------------------------
//	Global variables (non-numeric values cannot be added to a constantbuffer.)
//-----------------------------------------------------------------------------------------
//Textures used to make the blend map
Texture2D tex0 : register(t0); //R-channel in blendMap0.
Texture2D tex1 : register(t1); //G-channel in blendMap0.
Texture2D tex2 : register(t2); //B-channel in blendMap0.
Texture2D tex3 : register(t3); //A-channel in blendMap0
Texture2D tex4 : register(t4); //R-channel in blendMap1.
Texture2D tex5 : register(t5); //G-channel in blendMap1.
Texture2D tex6 : register(t6); //B-channel in blendMap1.
Texture2D tex7 : register(t7); //A-channel in blendMap1

Texture2D<float4> blendMap0; 
Texture2D<float4> blendMap1; 


//-----------------------------------------------------------------------------------------
// Constant buffers
//-----------------------------------------------------------------------------------------
cbuffer PerFrame
{
	float3 g_CamPos;
	float g_FarClip;
}
cbuffer PerObject
{
	//Matrices
	matrix	WVP;
	matrix	worldMatrix;

	//Texture
	bool	textured;
	bool	blendMapped;
	uint	nrOfBlendMaps;
	float	textureScale;

	//Material
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
	float4 Texture			: SV_TARGET0;	//Texture XYZ, Special Color W(not by this shader).
	float4 NormalAndDepth	: SV_TARGET1;	//Normal XYZ, depth W.
	float4 Position			: SV_TARGET2;	//Position XYZ, Type of object W.
	float4 Specular			: SV_TARGET3;	//Specular XYZ(unused by this shader), specular power W(unused by this shader).
};

//-----------------------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------------------
float3 RenderTextured(float scale, float2 tex, bool useBlendMap)
{
	//Sample R,G,B,A textures
	float2 texCoord = scale * tex;
	float3 tex0Color = tex0.Sample(AnisotropicWrapSampler, texCoord).rgb; 
	float3 tex1Color = tex1.Sample(AnisotropicWrapSampler, texCoord).rgb; 
	float3 tex2Color = tex2.Sample(AnisotropicWrapSampler, texCoord).rgb; 
	float3 tex3Color = tex3.Sample(AnisotropicWrapSampler, texCoord).rgb; 
	
	if(useBlendMap)
	{
		//Sample blend map texture
		float4 blendMap0Color = blendMap0.Sample(LinearClampSampler, tex); 
		
		// Is blendmap 0 empty?
		if ( length(blendMap0Color) > 0.0f )
		{	
			// Are we using multiple blendmaps?
			if(nrOfBlendMaps == 2)
			{
				float4 blendMap1Color = blendMap1.Sample(LinearClampSampler, tex);

				// Is blendmap 1 empty?
				if ( length(blendMap1Color) > 0.0f )
				{
					float blendSum0 = blendMap0Color.r + blendMap0Color.g + blendMap0Color.b + blendMap0Color.a;
					float blendSum1 = blendMap1Color.r + blendMap1Color.g + blendMap1Color.b + blendMap1Color.a;
					float4 blendValues0 = blendMap0Color / (blendSum0 + blendSum1); 
					float4 blendValues1 = blendMap1Color / (blendSum1 + blendSum0); 
					
					// Scale color for each texture by the weight in the blendmap.
					float3 tex0Color = tex0.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.r;
					float3 tex1Color = tex1.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.g;
					float3 tex2Color = tex2.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.b;
					float3 tex3Color = tex3.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.a;
				
					// Scale color for each texture by the weight in the blendmap.
					float3 tex4Color = tex4.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues1.r; 
					float3 tex5Color = tex5.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues1.g; 
					float3 tex6Color = tex6.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues1.b; 
					float3 tex7Color = tex7.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues1.a; 
		
					//Blendmapped color 
					return saturate(tex0Color + tex1Color + tex2Color + tex3Color + tex4Color + tex5Color + tex6Color + tex7Color) * diffuseColor;
				}
				else
				{
					// Only Blendmap 0 if Blendmap 1 is empty
					float blendSum0 = blendMap0Color.r + blendMap0Color.g + blendMap0Color.b + blendMap0Color.a;
					float4 blendValues0 = blendMap0Color / blendSum0; 
			
					float3 tex0Color = tex0.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.r;
					float3 tex1Color = tex1.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.g;
					float3 tex2Color = tex2.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.b;
					float3 tex3Color = tex3.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.a;
					
					// Blendmapped color 
					return saturate(tex0Color + tex1Color + tex2Color + tex3Color) * diffuseColor;
				}
			}
			else
			{
				// Only Blendmap 0 
				float blendSum0 = blendMap0Color.r + blendMap0Color.g + blendMap0Color.b + blendMap0Color.a;
				float4 blendValues0 = blendMap0Color / blendSum0;
			
				float3 tex0Color = tex0.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.r;
				float3 tex1Color = tex1.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.g;
				float3 tex2Color = tex2.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.b;
				float3 tex3Color = tex3.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues0.a;
				
				// Blendmapped color 
				return saturate(tex0Color + tex1Color + tex2Color + tex3Color) * diffuseColor;
			}
		}
		else
		{
			float4 blendMap1Color = blendMap1.Sample(LinearClampSampler, tex); 

			// Only Blendmap 1 if Blendmap 0 is empty
			if ( length(blendMap1Color) > 0.0 )
			{
				float blendSum1 = blendMap1Color.r + blendMap1Color.g + blendMap1Color.b + blendMap1Color.a;
				float4 blendValues1 = blendMap1Color / blendSum1; 
			
				float3 tex4Color = tex4.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues1.r;
				float3 tex5Color = tex5.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues1.g;
				float3 tex6Color = tex6.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues1.b;
				float3 tex7Color = tex7.Sample(AnisotropicWrapSampler, texCoord).rgb * blendValues1.a;
				
				// Blendmapped color 
				return saturate(tex4Color + tex5Color + tex6Color + tex7Color) * diffuseColor;
			}
		}
	}
	
	//if blendmapping is not used OR both blendmaps are empty, saturate texture colors.
	return saturate((tex0Color + tex1Color + tex2Color + tex3Color) * 0.25f) * diffuseColor;
}



//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output = (PSSceneIn)0;
	output.pos = mul(float4(input.pos, 1.0f), WVP);
	output.tex = input.tex;
	output.color = input.color;
	output.posW = mul(float4(input.pos, 1.0f), worldMatrix);
	output.norm = normalize(input.norm);

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

	if(textured) //Texture color(s)
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
	output.NormalAndDepth.xyz = input.norm;
	float depth = length(g_CamPos - input.posW.xyz) / g_FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	//Position RT
	output.Position.xyz = input.posW.xyz;
	output.Position.w = OBJECT_TYPE_TERRAIN; //See stdafx.fx for object types.
	
	//Specular RT(unused)
	output.Specular.xyzw = 0.0f;
	
	return output;
}


//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 TerrainTech
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