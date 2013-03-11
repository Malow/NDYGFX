//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Draws billboards pointing towards lightsource to shadowmap* 1 drawcall.
//	* As a circle instead of sampling a texture to improve performance.
// 
//--------------------------------------------------------------------------------------------------
#include "stdafx.fx"

//-----------------------------------------------------------------------------------------
// Global variables not put in buffers
//---------------------------------------------------------------------------------------
/*Texture2D gDiffuseMap;

float2 gTexCoords[4] = 
{
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f)
};*/

//-----------------------------------------------------------------------------------------
// Constant buffers
//----------------------------------------------------------------------------------------
cbuffer PerFrame
{
	float3		gSunDir;
};
cbuffer PerCascade
{
	float4x4	gLightViewProj;
};
/*cbuffer PerBillboardInstanceGroup
{
	bool		gIsTextured;
};*/

//-----------------------------------------------------------------------------------------
// Shader input structures
//----------------------------------------------------------------------------------------
struct VSIn
{
	//Reusing input layout(this is why dumm2 semantic name is still 'COLOR'. //TILLMAN
	float4 posWAndSizeX	: POSITION_AND_SIZE_X; 
	float sizeY			: SIZE_Y; 
	//float3 color		: COLOR; //= dummy
};

struct GSIn 
{
	float3 posCenterW	: POSITION0;
	float2 size			: SIZE;
};

struct PSIn
{
	float4 posCenterW	: POSITION1; 
	float4 posW			: POSITION2;
	float4 posH			: SV_POSITION;
	float radiusW		: RADIUS;
};/*
struct PSIn
{
	float4 posH		: SV_POSITION;
	float2 tex	: TEXCOORD;
};*/

//-----------------------------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------------------------
/*float4 gPositions[4];
void GeneratePositionsCircle(in float radius)
{
	float radius = 0.0f;
	angleDiff = 2.0f * PI / 4.0f;
	uint counter = 0;
	for(float radius = 0.0f; radius < 2.0f * PI; angle += angleDiff)
	{
		counter++; //**tillman todo
		if(counter % 3 == 0)
		{
			gPositions[counter] = float4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			gPositions[counter] = float4(radius * cos(angle), radius * sin(angle), 0.0f, 1.0f);
		}
	}
}*/

//-----------------------------------------------------------------------------------------
// Shaders
//---------------------------------------------------------------------------------------
GSIn VS(VSIn input)
{
	GSIn output = (GSIn)0;
	
	output.posCenterW = input.posWAndSizeX.xyz;	
	output.size = float2(input.posWAndSizeX.w, input.sizeY);

	return output;
}

[maxvertexcount(4)]
void GS(point GSIn input[1], inout TriangleStream<PSIn> triStream)
{	
	//Create world matrix to make the billboard face the light source.
	float3 forward = normalize(-gSunDir); 
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), forward));
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float4x4 W;
	W[0] = float4(right, 0.0f);
	W[1] = float4(up, 0.0f);
	W[2] = float4(forward, 0.0f);
	W[3] = float4(input[0].posCenterW, 1.0f);
	float4x4 lightWVP = mul(W, gLightViewProj); 
		
	//Create a quad in local space (facing down the z-axis)
	float halfWidth  = input[0].size.x * 0.5f;
	float halfHeight = input[0].size.y * 0.5f;
	float4 positions[4];
	positions[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f); //Top left
	positions[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);	//Top right
	positions[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom left
	positions[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom right

	//Transform quad to world space
	//Unroll to avoid warning x4715: emitting a system-interpreted value which may not be written in every execution path of the shader
	PSIn output = (PSIn)0;
	
	//float4 pixelW = mul(float4(input[0].posCenterW, 1.0f), W); //test
	
	float radius = halfWidth * 0.6666666f; //**Tillman - why not 0.5f*?
	output.posCenterW = float4(input[0].posCenterW, 1.0f);
	output.posW = mul(positions[0], W);
	output.posH = mul(positions[0], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.radiusW = radius;
	triStream.Append(output); 
	
	output.posCenterW = float4(input[0].posCenterW, 1.0f);
	output.posW = mul(positions[1], W);
	output.posH = mul(positions[1], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.radiusW = radius;
	triStream.Append(output); 
	
	output.posCenterW = float4(input[0].posCenterW, 1.0f);
	output.posW = mul(positions[2], W);
	output.posH = mul(positions[2], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.radiusW = radius;
	triStream.Append(output); 
	
	output.posCenterW = float4(input[0].posCenterW, 1.0f);
	output.posW = mul(positions[3], W);
	output.posH = mul(positions[3], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.radiusW = radius;
	triStream.Append(output); 
}

float PS(PSIn input) : SV_Depth
{
	float depth = 2.0f;
	//posCenterW = origin(middle of billboard) in world space.
	//posW = pixel in world space.
	//posH = pixel in screen space.
	if(length(input.posCenterW.xyz- input.posW.xyz) < input.radiusW)
	{
		depth = input.posH.z;
	}
	
	return depth;
	//return 0.0f;
}
/* 
[maxvertexcount(4)]
void GS(point GSIn input[1], inout TriangleStream<PSIn> triStream)
{	
	//Create world matrix to make the billboard face the light source.
	float3 forward = normalize(-gSunDir); 
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), forward));
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float4x4 W;
	W[0] = float4(right, 0.0f);
	W[1] = float4(up, 0.0f);
	W[2] = float4(forward, 0.0f);
	W[3] = float4(input[0].posCenterW, 1.0f);
	float4x4 lightWVP = mul(W, gLightViewProj); 
		
	//Create a quad in local space (facing down the z-axis)
	float halfWidth  = input[0].size.x * 0.5f;
	float halfHeight = input[0].size.y * 0.5f;
	float4 positions[4];
	positions[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f); //Top left
	positions[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);	//Top right
	positions[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom left
	positions[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom right

	//Transform quad to world space
	//Unroll to avoid warning x4715: emitting a system-interpreted value which may not be written in every execution path of the shader
	PSIn output = (PSIn)0;
	
	output.posH = mul(positions[0], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.tex = gTexCoords[0];
	triStream.Append(output); 
	
	output.posH = mul(positions[1], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.tex = gTexCoords[1];
	triStream.Append(output); 
	
	output.posH = mul(positions[2], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.tex = gTexCoords[2];
	triStream.Append(output); 
	
	output.posH = mul(positions[3], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.tex = gTexCoords[3];
	triStream.Append(output); 
}
float PS(PSIn input) : SV_Depth
{
	if(gIsTextured)
	{
		if(gDiffuseMap.Sample(PointWrapSampler, input.tex).a < 0.5f)
		{
			discard;
		}
	}

	return input.posH.z;
}*/

technique11 RenderShadowMapBillboardInstanced
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() )  );
		SetPixelShader( CompileShader ( ps_4_0, PS() ) );
		SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( NoCulling );
	}
}