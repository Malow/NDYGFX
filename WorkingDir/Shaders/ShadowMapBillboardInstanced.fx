//--------------------------------------------------------------------------------------------------
//	Code modified from ShadowMap.fx.
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//--------------------------------------------------------------------------------------------------
#include "stdafx.fx"

//-----------------------------------------------------------------------------------------
// Global variables not put in buffers
//---------------------------------------------------------------------------------------
//Texture2D gDiffuseMap;

/*float2 gTexCoords[4] = 
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
cbuffer PerBillboardInstanceGroup
{
	bool		gTextured;
};

//-----------------------------------------------------------------------------------------
// Shader input structures
//----------------------------------------------------------------------------------------
struct VSIn
{
	float3 posW		: POSITION;
	float2 size		: SIZE;
	float3 dummy1	: DUMMY; 
	float3 dummy2	: COLOR;
};

struct GSIn 
{
	float3 posW		: POSITION;
	float2 size		: SIZE;
	float3 color	: COLOR;	
};

struct PSIn
{
	float4 posW		: POSITION;
	float4 posH		: SV_POSITION;
	float radius	: RADIUS;
	float3 color	: COLOR;
	//float2 tex	: TEXCOORD;
};

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
	
	output.posW	 = input.posW;	
	output.size = input.size;

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
	W[3] = float4(input[0].posW, 1.0f);
	float4x4 lightWVP = mul(W, gLightViewProj); 
		
	//Create a quad in local space (facing down the z-axis)
	float halfWidth  = input[0].size.x * 0.5f;
	float halfHeight = input[0].size.y * 0.5f;
	float4 positions[4];
	positions[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f); //Top left
	positions[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);	//Top right
	positions[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom left
	positions[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom right

	float4 origin = mul(float4(input[0].posW, 1.0f), lightWVP);
	//Transform quad to world space
	//Unroll to avoid warning x4715: emitting a system-interpreted value which may not be written in every execution path of the shader
	PSIn output = (PSIn)0;

	output.posW = origin;
	output.posH = mul(positions[0], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.radius = halfWidth;
	//output.tex = gTexCoords[0];
	triStream.Append(output); 
	
	output.posW = origin;
	output.posH = mul(positions[1], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.radius = halfWidth;
	//output.tex = gTexCoords[1];
	triStream.Append(output); 
	
	output.posW = origin;
	output.posH = mul(positions[2], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.radius = halfWidth;
	//output.tex = gTexCoords[2];
	triStream.Append(output); 
	
	output.posW = origin;
	output.posH = mul(positions[3], lightWVP); //Transform positions to light's clip space [-w,-w]
	output.radius = halfWidth;
	//output.tex = gTexCoords[3];
	triStream.Append(output); 
}

float PS(PSIn input) : SV_Depth
{
	/*if(textured)
	{
		if(diffuseMap.Sample(PointWrapSampler, input.tex).a < 0.5f)
		{
			discard;
		}
	}*/

	float depth = -1.0f;
	//posW = origin in screen space.
	//posH = pixel in screen space.
	//if(length(input.posW.xy - input.posH.xy) < 5555)
	{
		depth = input.posH.z;
	}
	
	return depth;
	//return 0.0f;
}

technique11 RenderShadowMapBillboardInstanced
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() )  );
		//SetGeometryShader( NULL );
		SetPixelShader( CompileShader ( ps_4_0, PS() ) );
		SetDepthStencilState( EnableDepth, 0 );
		SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( NoCulling );
	}
}