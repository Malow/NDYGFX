//-----------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
//	
//	This shader renders grass (deferred with 4 GBuffers).
//	Requirement(s): 
//		TODO
//		Pass 1
//		Pass 2
//-----------------------------------------------------------------------------------------

#include "stdafx.fx"


//-----------------------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------------------
//Texture2D	g_t_VertexPositions;
Texture2D	g_t_Texture; //Terrain texture <float3>? ** TILLMAN


//-----------------------------------------------------------------------------------------
// Constant buffers
//----------------------------------------------------------------------------------------
/*cbuffer PerFrame
{
	float3		g_CameraPos;
	float4x4	g_CamViewProj;
};*/
cbuffer PerTerrain
{
	float		g_t_MinGrassHeight;
	float		g_t_MaxGrassHeight;
	float4x4	g_t_WorldMatrix;
	//float3		g_t_Size;
};


/*terrain height 
sample texture

if(greenTexel)
	grassHeight = fractal value
	grassColor[index] = greenTexel
else grassHeight = 0;
*/

//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------

//Pass 1 - GenerateCanopy.
struct Pass1VSInput
{
	float3 posLocal	: POSITION;
	float2 texCoords: TEXCOORDS;
	//float3 normal	: NORMAL; //tillman: ev för sluttningar/vertikala
};
struct Pass1PSInput 
{
	float4 posWorld	: SV_Position;	
	float2 texCoords: TEXCOORDS;
	//float3 normal	: NORMAL;//tillman: ev för sluttningar/vertikala
};


struct Pass1PSOutput			
{
	float4 terrainWorldPos	: SV_TARGET0;	//Position XYZ, unused W.
	float4 colorAndHeight	: SV_TARGET1;	//Grass color XYZ, grass height W.
};



Pass1PSInput Pass1VS( Pass1VSInput input)
{
	Pass1PSInput output = (Pass1PSInput)0;

	output.posWorld = mul(float4(input.posLocal, 1.0f), g_t_WorldMatrix);
	output.texCoords = input.texCoords; //TILLMAN, ersätta med primitive ID?

	return output;
}

Pass1PSOutput Pass1PS( Pass1PSInput input)
{
	Pass1PSOutput output = (Pass1PSOutput)0;

	float3 terrainColor = g_t_Texture.Sample(PointWrapSampler, input.texCoords).xyz; //**<float3>//3 kanaler
	float3 grassColor = float3(0.0f, 0.0f, 0.0f);
	float grassHeight = 0.0f;
	//Check if texture color is green/-ish.
	if(terrainColor.g > 0.5f) //**tillman mer**
	{
		//**TODO: the less green it is, the lower the grass height should be.**

		//Set grass color
		grassColor = terrainColor; //**lägga till lite variation**

		//And generate grass height (in meters).
		grassHeight = 0.2f; //Generera med fraktaler**
		//g_t_MinGrassHeight;
		//g_t_MaxGrassHeight; ** noise()??, -1,1 in = float3*/

	}

	//Terrain world position render target.
	output.terrainWorldPos.xyz = input.posWorld.xyz;
	output.terrainWorldPos.w = -1.0f;
	
	//Grass color and height render target.
	output.colorAndHeight = float4(grassColor, grassHeight);
	
	return output;
}



/*





/*
struct DummyIn
{

};

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
}*/

technique10 DrawGrass
{
    pass GenerateCanopy //Pass 1
    { 
        SetVertexShader( CompileShader( vs_4_0, Pass1VS() )  );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, Pass1PS() ) );
		
        
		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
	/*pass GenerateGrass //Pass 2
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
        
		SetDepthStencilState( EnableDepth, 0 );
	    SetRasterizerState( BackCulling );
		SetBlendState(NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		//SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}*/
}