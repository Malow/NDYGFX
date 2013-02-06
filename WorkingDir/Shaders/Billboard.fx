//-----------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
//	
//	This shader renders billboards (deferred with 4 GBuffers).
//	Requirement(s): 
//		World position of camera.
//		Camera view & projection matrices.
//		Size of billboard (width & height) in **vilket space - tillman?**
//		World space position of billboard.
//	Optional:
//		Diffuse map (texture).
//		Color of billboard.
//-----------------------------------------------------------------------------------------

#include "stdafx.fx"


//-----------------------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------------------
Texture2D		g_DiffuseMap; 
//**TILLMAN
float2 g_TexCoords[4] = 
{
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f)
};

//-----------------------------------------------------------------------------------------
// Constant buffers
//----------------------------------------------------------------------------------------
cbuffer PerFrame
{
	float3		g_CameraPos;
	float4x4	g_CamViewProj;
};
cbuffer PerBillBoard
{
	float2		g_BillboardSize;
	bool		g_IsTextured;
};

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
struct VSIn
{
	float3 posW		: POSITION; //world space
	//float2 dummy1	: TEXCOORD;//unused TILLMAN
	//float3 dummy2	: NORMAL; //unused TILLMAN
	float3 color	: COLOR;
};

struct GSIn 
{
	float3 posW		: POSITION;
	float3 color	: COLOR;	
};

struct PSIn 
{
	float4	posH		: SV_Position;	//homogenous clip space
	float2	texCoords	: TEXCOORD;
	float3	color		: COLOR; //TILLMAN opt, def struct, if(!textured)
};



//-----------------------------------------------------------------------------------------
// Vertex shader
//-----------------------------------------------------------------------------------------
GSIn VS(VSIn input)
{
	//GSIn output = (GSIn)0;

	//output.posW = input.posW;
	//output.color = input.color;

	//return output;
	return (GSIn)input;
}




//-----------------------------------------------------------------------------------------
// Geometry shader 
//-----------------------------------------------------------------------------------------
//#define VERTEX_COUNT (4) //tillman todo test
[maxvertexcount(4)]
void GS(point GSIn input[1], inout TriangleStream<PSIn> triStream)
{	
	//Create world matrix to make the billboard face the camera.
	float3 forward = normalize(g_CameraPos - input[0].posW);
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), forward));
	float3 up = cross(forward, right);
	float4x4 W;
	W[0] = float4(right, 0.0f);
	W[1] = float4(up, 0.0f);
	W[2] = float4(forward, 0.0f);
	W[3] = float4(input[0].posW, 1.0f);
	float4x4 WVP = mul(W, g_CamViewProj); 
		
	//Create a quad in local space (facing down the z-axis)
	float halfWidth  = g_BillboardSize.x * 0.5f;
	float halfHeight = g_BillboardSize.y * 0.5f;
	float4 positions[4];
	positions[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f); //Top left
	positions[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);	//Top right
	positions[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom left
	positions[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom right

	//Transform quad to world space
	//Unroll to avoid warning x4715: emitting a system-interpreted value which may not be written in every execution path of the shader
	PSIn output = (PSIn)0;
	
	output.posH = mul(positions[0], WVP); //Transform positions to clip space [-w,-w]
	output.texCoords = g_TexCoords[0];
	output.color = input[0].color;
	triStream.Append(output); 

	output.posH = mul(positions[1], WVP); //Transform positions to clip space [-w,-w]
	output.texCoords = g_TexCoords[1];
	output.color = input[0].color;
	triStream.Append(output); 

	output.posH = mul(positions[2], WVP); //Transform positions to clip space [-w,-w]
	output.texCoords = g_TexCoords[2];
	output.color = input[0].color;
	triStream.Append(output); 

	output.posH = mul(positions[3], WVP); //Transform positions to clip space [-w,-w]
	output.texCoords = g_TexCoords[3];
	output.color = input[0].color;
	triStream.Append(output); 
}
//-----------------------------------------------------------------------------------------
// Pixel shader (draw)
//-----------------------------------------------------------------------------------------
float4 PS(PSIn input) : SV_TARGET
{
	if(g_IsTextured)
	{
		float4 finalColor = g_DiffuseMap.Sample(LinearWrapSampler, input.texCoords);
		if(finalColor.a < 0.5f)
		{
			discard;
		}

		return finalColor;
	}
	else
	{
		return float4(input.color, 1.0f);
	}
}

technique10 DrawBillboard
{
    pass P0
    { 
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
        
		SetDepthStencilState( EnableDepth, 0 ); //Disabeldepthwrite?**TILLMAN test
	    SetRasterizerState( BackCulling );
		//SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

