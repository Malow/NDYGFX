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
Texture2D		g_bb_DiffuseMap; 
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
	float3		g_bb_Position; //input for Geometry shader
	float2		g_bb_BillboardSize;
	float3		g_bb_Color;
	bool		g_bb_IsTextured;
};

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

struct DummyIn
{

};
/*struct VSIn
{
	//float3 posW		: POSITION; //world space
	//float2 dummy1	: TEXCOORD;//unused TILLMAN
	//float3 dummy2	: NORMAL; //unused TILLMAN
	//float3 color	: COLOR;
};*/

struct GSIn 
{
	float3 posW		: POSITION;
	//float3 color	: COLOR;	
};

struct PSIn 
{
	float4	posH		: SV_Position;	//homogenous clip space
	float3	normal		: NORMAL;
	float2	texCoords	: TEXCOORD;
	float3	color		: COLOR; //TILLMAN opt, def struct, if(!textured)
};

struct PSOut			
{
	float4 Texture			: SV_TARGET0;	//Texture XYZ, unused W
	float4 NormalAndDepth	: SV_TARGET1;	//Normal XYZ, depth W
	float4 Position			: SV_TARGET2;	//Position XYZ, unused W
	float4 Specular			: SV_TARGET3;	//Specular XYZ, specular power W
};

//-----------------------------------------------------------------------------------------
// Vertex shader
//-----------------------------------------------------------------------------------------
GSIn VS(DummyIn input)
{
	GSIn output = (GSIn)0;

	output.posW = g_bb_Position;

	return output;
	//return (GSIn)input;
}




//-----------------------------------------------------------------------------------------
// Geometry shader 
//-----------------------------------------------------------------------------------------
//#define VERTEX_COUNT (4) //tillman todo test
[maxvertexcount(4)]
void GS(point GSIn input[1], inout TriangleStream<PSIn> triStream)
{	
	//Create world matrix to make the billboard face the camera.
	float3 forward = normalize(g_CameraPos - input[0].posW); //Also normal
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), forward));
	float3 up = float3(0.0f, 1.0f, 0.0f);//cross(forward, right);
	float4x4 W;
	W[0] = float4(right, 0.0f);
	W[1] = float4(up, 0.0f);
	W[2] = float4(forward, 0.0f);
	W[3] = float4(input[0].posW, 1.0f);
	float4x4 WVP = mul(W, g_CamViewProj); 
		
	//Create a quad in local space (facing down the z-axis)
	float halfWidth  = g_bb_BillboardSize.x * 0.5f;
	float halfHeight = g_bb_BillboardSize.y * 0.5f;
	float4 positions[4];
	positions[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f); //Top left
	positions[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);	//Top right
	positions[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom left
	positions[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);	//Bottom right

	//Transform quad to world space
	//Unroll to avoid warning x4715: emitting a system-interpreted value which may not be written in every execution path of the shader
	PSIn output = (PSIn)0;
	output.posH = mul(positions[0], WVP); //Transform positions to clip space [-w,-w]
	output.normal = forward;
	output.texCoords = g_TexCoords[0];
	output.color = g_bb_Color;
	triStream.Append(output); 

	output.posH = mul(positions[1], WVP); //Transform positions to clip space [-w,-w]
	output.normal = forward;
	output.texCoords = g_TexCoords[1];
	output.color = g_bb_Color;
	triStream.Append(output); 

	output.posH = mul(positions[2], WVP); //Transform positions to clip space [-w,-w]
	output.normal = forward;
	output.texCoords = g_TexCoords[2];
	output.color = g_bb_Color;
	triStream.Append(output); 

	output.posH = mul(positions[3], WVP); //Transform positions to clip space [-w,-w]
	output.normal = forward;
	output.texCoords = g_TexCoords[3];
	output.color = g_bb_Color;
	triStream.Append(output); 
}
//-----------------------------------------------------------------------------------------
// Pixel shader 
//-----------------------------------------------------------------------------------------
PSOut PS(PSIn input) : SV_TARGET
{
	PSOut output = (PSOut)0; 

	//Texture RT
	if(g_bb_IsTextured)
	{
		float4 finalColor = g_bb_DiffuseMap.Sample(LinearWrapSampler, input.texCoords);
		if(finalColor.a < 0.5f)
		{
			discard;
		}

		output.Texture = finalColor;
		//return finalColor;
	}
	else
	{
		//return float4(input.color, 1.0f);
		output.Texture = float4(input.color, 1.0f);
	}

	//Normal and depth RT
	output.NormalAndDepth = float4(input.normal, input.posH.z / input.posH.w);	
	float depth = length(g_CameraPos - g_bb_Position) / 200.0f;		// Haxfix**tillman
	output.NormalAndDepth.w = depth;

	//Position RT
	output.Position = float4(g_bb_Position, 1.0f);
	
	//Specular RT
	output.Specular.xyzw = 0.0f;

	return output;
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
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}