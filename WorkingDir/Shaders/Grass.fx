//-----------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
//	
//	This shader renders grass (deferred with 4 GBuffers) using a a canopy generated from Terrain.fx.
//
//-----------------------------------------------------------------------------------------
#include "stdafx.fx"

//-----------------------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------------------
Texture2D g_Canopy; //XYZ = grass color, W = grass height


//-----------------------------------------------------------------------------------------
// Constant buffers
//----------------------------------------------------------------------------------------
cbuffer PerFrame
{
	float		g_FarClip;
	float3		g_CamPos;
	float4x4	g_CamViewProj;
};
cbuffer PerTerrain
{
	float4x4	g_World;
};



//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
struct VSIn
{
	float3 pos		: POSITION;
	//float2 tex		: TEXCOORD;
	//float3 norm		: NORMAL;
	//float3 color	: COLOR;
};

struct GSIn 
{
	float3 posW		: POSITION;
	//float3 color	: COLOR;	
};

struct PSIn 
{
	float3	posW		: POSITION;
	float4	posH		: SV_Position;	//homogenous clip space
	float3	normal		: NORMAL;
	float3	color		: COLOR; //TILLMAN opt, def struct, if(!textured)
};

struct PSOut			
{
	float4 Texture			: SV_TARGET0;	//Texture XYZ, unused W
	float4 NormalAndDepth	: SV_TARGET1;	//Normal XYZ, depth W
	float4 Position			: SV_TARGET2;	//Position XYZ, object type W(unused by this shader)
	float4 Specular			: SV_TARGET3;	//Specular XYZ(unused by this shader), specular power W(unused by this shader)
};

//-----------------------------------------------------------------------------------------
// Vertex shader
//-----------------------------------------------------------------------------------------
GSIn VS(VSIn input)
{
	GSIn output = (GSIn)0;

	output.posW = mul(float4(input.pos, 1.0f), g_World).xyz;

	return output;
}




//-----------------------------------------------------------------------------------------
// Geometry shader 
//-----------------------------------------------------------------------------------------

[maxvertexcount(4)]
void GS(point GSIn input[1], inout TriangleStream<PSIn> triStream)
{	
	//Create world matrix to make the billboard face the camera.
	float3 forward = normalize(g_CamPos - input[0].posW); //Also normal
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), forward));
	float3 up = float3(0.0f, 1.0f, 0.0f);//cross(forward, right);
	float4x4 W;
	W[0] = float4(right, 0.0f);
	W[1] = float4(up, 0.0f);
	W[2] = float4(forward, 0.0f);
	W[3] = float4(input[0].posW, 1.0f);
	float4x4 WVP = mul(W, g_CamViewProj); 
	
	//Convert from world to homogeneous clip space [-w,w].
	float4 pos = mul(input[0].posW, g_CamViewProj);
	//Convert to normalized device coordinates [-1,1].
	pos.xy /= pos.w;
	//Convert to texture coordinates [0,1]
	float2 texCoords = float2(pos.x * 0.5f, pos.y * -0.5f) + 0.5f;

	float4 grassColorAndHeight = g_Canopy.SampleLevel(PointWrapSampler, texCoords, 0);
	//float4 grassColorAndHeight = float4(0,1,0,0.5f);
	float3 grassColor = grassColorAndHeight.xyz;
	float grassHeight = grassColorAndHeight.w;

	//Create a quad in local space (facing down the z-axis)
	float halfWidth  = 0.25f; //**TILLMAN - variabel?**
	float halfHeight = grassHeight * 0.5f;
	float4 positions[4];
	positions[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f); //Top left
	positions[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);	//Top right
	positions[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom left
	positions[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom right

	

	//Transform quad to world space
	//Unroll to avoid warning x4715: emitting a system-interpreted value which may not be written in every execution path of the shader
	PSIn output = (PSIn)0;
	output.posW = input[0].posW;
	output.posH = mul(positions[0], WVP); //Transform positions to clip space [-w,-w]
	output.normal = float3(0.0f, 1.0f, 0.0f);
	output.color = grassColor;
	triStream.Append(output); 
	
	output.posW = input[0].posW;
	output.posH = mul(positions[1], WVP); //Transform positions to clip space [-w,-w]
	output.normal = float3(0.0f, 1.0f, 0.0f);
	output.color = grassColor;
	triStream.Append(output); 
	
	output.posW = input[0].posW;
	output.posH = mul(positions[2], WVP); //Transform positions to clip space [-w,-w]
	output.normal = float3(0.0f, 1.0f, 0.0f);
	output.color = grassColor;
	triStream.Append(output); 
	
	output.posW = input[0].posW;
	output.posH = mul(positions[3], WVP); //Transform positions to clip space [-w,-w]
	output.normal = float3(0.0f, 1.0f, 0.0f);
	output.color = grassColor;
	triStream.Append(output); 
}
//-----------------------------------------------------------------------------------------
// Pixel shader 
//-----------------------------------------------------------------------------------------
PSOut PS(PSIn input) 
{
	PSOut output = (PSOut)0; 

	//Texture RT
	output.Texture = float4(input.color, -1.0f);

	//Normal and depth RT
	//output.NormalAndDepth = float4(input.normal, input.posH.z / input.posH.w);	
	output.NormalAndDepth.xyz = input.normal;
	float depth = length(g_CamPos - input.posW) / g_FarClip;		// Haxfix
	output.NormalAndDepth.w = depth;

	//Position RT
	output.Position = float4(input.posW, -1.0f);
	
	//Specular RT(unused by thi shader)
	output.Specular.xyzw = 0.0f;

	return output;
}

technique10 DrawGrass
{
    pass p0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}