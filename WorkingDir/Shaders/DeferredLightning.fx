//-----------------------------------------------------------------------------------------------------
//
//	Originally made by MaloW.
//
//	Cascaded shadow maps written by Tillman for projekt Not Deat Yet at Blekinge tekniska h�gskola.
//
//-----------------------------------------------------------------------------------------------------
#include "stdafx.fx"
#include "SSAO.fx"
#include "PPHighlightArea.fx"

// Marcus L�wegren

// For textures
Texture2D Texture;
Texture2D NormalAndDepth;
Texture2D Position;
Texture2D Specular;
SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D ShadowMap[10];
Texture2D CascadedShadowMap[4];
SamplerState shadowMapSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

DepthStencilState DisableDepthWrite
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};
BlendState SrcAlphaBlendingAdd 
{ 
	BlendEnable[0] = TRUE; 
	SrcBlend = SRC_ALPHA; 
	DestBlend = INV_SRC_ALPHA; 
	BlendOp = ADD; 
	SrcBlendAlpha = ZERO; 
	DestBlendAlpha = ZERO; 
	BlendOpAlpha = ADD; 
	RenderTargetWriteMask[0] = 0x0F; 
}; 
//-----------------------------------------------------------------------------------------
// Input and Output Structures
//-----------------------------------------------------------------------------------------
cbuffer ef
{
	//Single sampling
	float SMAP_DX; 

	//PCF sampling
	float PCF_SIZE;
	float PCF_SIZE_SQUARED;

	//Multiple sampling
	bool blendCascades; //**ers�tta med blenddistance
	float blendDistance; //the number of pixels to blend.
	float blendStrength; //How much to blend. **to use?**

	//Both/other
	uint nrOfCascades;
	float4 cascadeFarPlanes;
	matrix cameraViewMatrix;
};




struct VSIn
{
	float4 Pos : POSITION;
	float2 tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 Color : COLOR;
};

struct PSSceneIn
{
	float4 Pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
RasterizerState NoCulling
{
	CullMode = NONE;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VSIn VSScene(VSIn input)
{
	return input;
}


// GS
[maxvertexcount(4)]
void GS( point VSIn input[1], inout TriangleStream<PSSceneIn> triStream )
{
	PSSceneIn output;

	//bottom left
	output.Pos = float4(-1, -1, 0, 1);
	output.tex = float2(0,1);
	triStream.Append(output);
	
	//bottom right
	output.Pos = float4(1, -1, 0, 1);
	output.tex = float2(1,1);
	triStream.Append(output);

	//top left
	output.Pos = float4(-1, 1, 0, 1);
	output.tex = float2(0,0);
	triStream.Append(output);

	//top right
	output.Pos = float4(1, 1, 0, 1);
	output.tex = float2(1,0);
	triStream.Append(output);
}

//Returns the index of the cascade(frustum slice) to use.
uint FindCascade(float pixelDepthCameraViewSpace)
{
	//pixelPosCameraViewSpace is the position of the pixel in camera view space.
	uint i = 0;
	for(; i < nrOfCascades; i++)
	{
		//cascadeFarPlanes is the z-distance of the frustum slices in view space.
		//If the depthposition of the pixel is less than the far plane, the pixel is inside that frustum slice.
		if(pixelDepthCameraViewSpace < cascadeFarPlanes[i])
		{
			return i; //return early if the pixel is close to the camera's position. (Origo).
		}
	}

	return -1; //the pixel is outside of the cameras frustum.
}
//Returns the index of the cascade(frustum slice) to blend with.
uint FindCascadeToBlendWith(uint cascadeIndex, float pixelDepthCameraViewSpace)
{
	//First, get the center position of the cascade.
	float cascadeCenterPos = 0.0f;
	
	if(cascadeIndex == -1)
	{
		return -1;
	}
	else if(cascadeIndex == 0)
	{
		cascadeCenterPos = cascadeFarPlanes[cascadeIndex] * 0.5f;
	}
	else
	{
		cascadeCenterPos = (cascadeFarPlanes[cascadeIndex] - cascadeFarPlanes[cascadeIndex - 1]) * 0.5f + cascadeFarPlanes[cascadeIndex - 1];
	}
	
	//Return the cascade the pixel is closest to. (One of the neighbouring cascades to the one the pixel is inside of).
	if(pixelDepthCameraViewSpace > cascadeCenterPos)
	{
		return cascadeIndex + 1; //OBS! Can return nrOfCascades!
	}
	else
	{
		return cascadeIndex - 1; //OBS! Can return -1! 
	}
}
//Return a floating point value representing the shadow factor. Range[0,1].
float SampleCascades(uint cascadeIndex, uint otherCascadeIndex, float2 pixelPosTexelSpace, float pixelDepth) 
{
	//**
	if(otherCascadeIndex < 0 || otherCascadeIndex > (nrOfCascades - 1))
	{
		//return 0.0f; //Debug
	}


	//**TILLMAN TODO: X3511: flatten/unroll, stoppa alla shadowmaps i en texture, etc**
	
	float CSM_SHADOW_EPSILON = 0.005f;// - (cascadeIndex * 0.0005f);// - (pixelDepth * 0.001f); //**finputsa, l�gga till pixelDepth?**
	float shadow = 0.0f; 
	if(blendCascades) //global variable
	{
		//Convert pixel depth to world units.
		float pixelDepthWorld = pixelDepth * cascadeFarPlanes[nrOfCascades - 1];
		//Check if pixel is within blending distance.
		//**todo CHECK** TILLMAN
		//if true, sample both and lerp samples**
		//else: kod under

		if(PCF_SIZE > 0) //**TILLMAN, ers�tta med if PCF_SIZE != 0??**
		{
			for(float s = 0; s < PCF_SIZE; s++) // error X3511: forced to unroll loop, but unrolling failed.
			{
				for(float q = 0; q < PCF_SIZE; q++)
				{
					//X3511: shadow += (CascadedShadowMap[cascadeIndex].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					if(cascadeIndex == 0)
					{
						shadow += (CascadedShadowMap[0].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					}
					else if(cascadeIndex == 1) 
					{
						shadow += (CascadedShadowMap[1].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					}
					else if(cascadeIndex == 2)
					{
						shadow += (CascadedShadowMap[2].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					}
					else if(cascadeIndex == 3)
					{
						shadow += (CascadedShadowMap[3].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					}
				}
			}
			shadow *= PCF_SIZE_SQUARED;
		}
		else
		{
			//X3511: shadow = (CascadedShadowMap[cascadeIndex].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			if(cascadeIndex == 0)
			{
				shadow = (CascadedShadowMap[0].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			}
			else if(cascadeIndex == 1) 
			{
				shadow = (CascadedShadowMap[1].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			}
			else if(cascadeIndex == 2)
			{
				shadow = (CascadedShadowMap[2].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			}
			else if(cascadeIndex == 3)
			{
				shadow = (CascadedShadowMap[3].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			}
		}
	}
	else
	{
		if(PCF_SIZE > 0)
		{
			for(float s = 0; s < PCF_SIZE; s++) // error X3511: forced to unroll loop, but unrolling failed.
			{
				for(float q = 0; q < PCF_SIZE; q++)
				{
					//X3511: shadow += (CascadedShadowMap[cascadeIndex].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					if(cascadeIndex == 0)
					{
						shadow += (CascadedShadowMap[0].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					}
					else if(cascadeIndex == 1) 
					{
						shadow += (CascadedShadowMap[1].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					}
					else if(cascadeIndex == 2)
					{
						shadow += (CascadedShadowMap[2].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					}
					else if(cascadeIndex == 3)
					{
						shadow += (CascadedShadowMap[3].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
					}
				}
			}
			shadow *= PCF_SIZE_SQUARED;
		}
		else
		{
			//X3511: shadow = (CascadedShadowMap[cascadeIndex].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			if(cascadeIndex == 0)
			{
				shadow = (CascadedShadowMap[0].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			}
			else if(cascadeIndex == 1) 
			{
				shadow = (CascadedShadowMap[1].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			}
			else if(cascadeIndex == 2)
			{
				shadow = (CascadedShadowMap[2].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			}
			else if(cascadeIndex == 3)
			{
				shadow = (CascadedShadowMap[3].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
			}
		}
	}

	/**ev: if(shadowMapTextureCoords.x < 0 || shadowMapTextureCoords.x > 1 || shadowMapTextureCoords.y < 0 || shadowMapTextureCoords.y > 1)
		{
			shadow = 1.0f;
		}
		else if(depth > 1.0f)
		{
			shadow = 1.0f;
		}***/
	

	return shadow;
}
		


//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	float3 DiffuseColor = Texture.Sample(linearSampler, input.tex).xyz;	

	float4 NormsAndDepth = NormalAndDepth.Sample(linearSampler, input.tex);
	
	float4 WorldPos = Position.Sample(linearSampler, input.tex);

	float4 AmbientLight = SceneAmbientLight;

	float SpecularPower = Specular.Sample(linearSampler, input.tex).w;
	float4 SpecularColor = float4(Specular.Sample(linearSampler, input.tex).xyz, 1.0f);

	float diffuseLighting = 0.0f;
	float specLighting = 0.0f;
	
	for(int i = 0; i < NrOfLights; i++)
	{
		float3 LightDirection = WorldPos.xyz - lights[i].LightPosition.xyz;
		float DistanceToLight = length(LightDirection);
		LightDirection = normalize(LightDirection);

		// Diff light
		float difflight = saturate(dot(NormsAndDepth.xyz, -LightDirection));

		// Spec Light
		float3 h = normalize(normalize(CameraPosition.xyz - WorldPos.xyz) - LightDirection);
		float speclight = pow(saturate(dot(h, NormsAndDepth.xyz)), SpecularPower);


		// Shadow Mappings
		float4 posLight = mul(WorldPos, lights[i].LightViewProj);
		posLight.xy /= posLight.w;


		//Compute shadow map tex coord
		float2 smTex = float2(0.5f*posLight.x, -0.5f*posLight.y) + 0.5f;

		// Compute pixel depth for shadowing.
		float depth = posLight.z / posLight.w;

		float SHADOW_EPSILON = 0.00001f;			////////////// PUT THIS WHERE?


		//float PCF_SIZE = 3.0f;								////// Not able to move this to cbuffer, why?


		// PCF
		float shadow = 0.0f;
		if(smTex.x < 0 || smTex.x > 1 || smTex.y < 0 || smTex.y > 1)
			shadow = 1.0f;
		else if(depth > 1.0f)
			shadow = 1.0f;
		else
		{
			for(float s = 0; s < PCF_SIZE; s++)
			{
				for(float q = 0; q < PCF_SIZE; q++)
				{
					shadow += (ShadowMap[i].SampleLevel(shadowMapSampler, smTex + float2(SMAP_DX * (s - PCF_SIZE/2) , SMAP_DX * (q - PCF_SIZE/2)), 0).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
				}
			}
			shadow *= PCF_SIZE_SQUARED;
		}


		// Fall off test 3
		float coef = (lights[i].LightIntensity / 1000.0f) + (DistanceToLight * DistanceToLight) / (lights[i].LightIntensity * lights[i].LightIntensity);

		difflight /= coef;

		/// For slower fall-off on spec light. Doesnt work with quad-based culling tho.
		//if(coef > 10.0f)
		//	speclight /= coef / 10.0f;
			
		speclight /= coef;
		
		difflight *= shadow;
		speclight *= shadow;
		
		diffuseLighting += difflight;
		specLighting += speclight;
	}
	
	if(NrOfLights > 0)
	{
		diffuseLighting = saturate(diffuseLighting / NrOfLights);
		specLighting = saturate(specLighting / NrOfLights);
	}
	
	//TILLMAN START OF CSM
	// Sun
	if(UseSun)
	{
		// Diff light
		float diffLight = saturate(dot(NormsAndDepth.xyz, -sun.Direction.xyz)) * sun.LightIntensity;
		// Spec Light
		float3 h = normalize(normalize(CameraPosition.xyz - WorldPos.xyz) - sun.Direction.xyz);
		float specLight = pow(saturate(dot(h, NormsAndDepth.xyz)), SpecularPower) * sun.LightIntensity;

		
		//I PS:
		//Determine the proper shadow map.
		//Transforms the texture coordinates if necessary.
		//Samples the cascade.
		//Lights the pixel.

		
		//Determine the shadow map to use:
		//First transform the pixel from world space to CAMERA view space.
		float pixelDepthCameraViewSpace = mul(WorldPos, cameraViewMatrix).z; 
		//Find the index of the cascade(frustum slice) it is inside of.
		uint cascadeIndex = -1;
		cascadeIndex = FindCascade(pixelDepthCameraViewSpace);

		//**TILLMAN TODO** early exit: if (CascadeIndex == -1), warning dock! kan reducera fps ist�llet**


		//Determine the second cascade to use if blending between cascades is enabled:
		uint otherCascadeIndex = -1; //**TILLMAN TODO**
		if(blendCascades)
		{
			otherCascadeIndex = FindCascadeToBlendWith(cascadeIndex, pixelDepthCameraViewSpace); 
		}

		//Sample the cascade(s):
		float shadow = 0.0f;
		
		//Get the texture coordinates to sample with by first transforming the pixel from world space to LIGHT's clip space xy[-w,w], z[0,w].
		float4 pixelPosTexelSpace = mul(WorldPos, cascades[cascadeIndex].viewProj); 
		//Then convert it to normalized device coordinates xy[-1,1], z[0,1].
		pixelPosTexelSpace.xyz /= pixelPosTexelSpace.w;
		//Finally convert it to texel space xy[0,1]. (Don't forget that the y-axis needs to be inverted).
		pixelPosTexelSpace.x = pixelPosTexelSpace.x * 0.5f;
		pixelPosTexelSpace.y = pixelPosTexelSpace.y * -0.5f;
		pixelPosTexelSpace.xy += 0.5f;
		
		//And finally sample the cascades(s).
		shadow = SampleCascades(cascadeIndex, otherCascadeIndex, pixelPosTexelSpace.xy, pixelPosTexelSpace.z); 
		
		//Multiply the shadow into the light.
		diffLight *= shadow;
		specLight *= shadow;
		
		
		//**tillman end of CSM
		
		diffuseLighting += diffLight;
		specLighting += specLight;

		diffuseLighting = saturate(diffuseLighting);
		specLighting = saturate(specLighting);
	}

	// Reduction of ambientlight if diffuse and spec is high to prevent oversaturation.
	AmbientLight /= 1.0f + diffuseLighting + specLighting;	

	float4 finalColor = float4((							
		AmbientLight.xyz * DiffuseColor + 
		DiffuseColor * diffuseLighting + 
		SpecularColor.xyz * specLighting), 
		1.0f);


	
	// Haxfix, want it above but I lose 75% of my FPS then (??!?!? :S:S:S:S:S)
	if(NormsAndDepth.w < -0.5f)		// All pixels that has a negative depth means that there is no geometry, therefor go without lightcalcs.
		finalColor = float4(DiffuseColor, 1.0f);

	if(NormsAndDepth.w > 1.0f)		// All pixels that has a greater than 1 depth means that there is no geometry and there is skybox, therefor go without lightcalcs.
		finalColor = float4(DiffuseColor, 1.0f);
	
	

	
	
	//temp:
	//ev. todo: if player is on red team, reduce redness and increase blueness**
	
	
	uint TeamColor = (uint)Texture.Sample(linearSampler, input.tex).w;
	//Exlude skybox
	if(NormsAndDepth.w < 1.00001f)
	{
		//Exclude nullColor
		if(TeamColor != 0)
		{
			switch(TeamColor)
			{
				case 1: finalColor.xyz += (WHITE.xyz * 0.2f); break;
				case 2: finalColor.xyz += (BLACK.xyz * 0.2f); break;
				case 3: finalColor.xyz += (RED.xyz * 0.2f); break;
				case 4: finalColor.xyz += (GREEN.xyz * 0.2f); break;
				case 5: finalColor.xyz += (BLUE.xyz * 0.2f); break;
				case 6: finalColor.xyz += (YELLOW.xyz * 0.2f); break;
				case 7: finalColor.xyz += (CYAN.xyz * 0.2f); break;
				case 8: finalColor.xyz += (MAGENTA.xyz * 0.2f); break;
			}
			finalColor.xyz *= (5.0f / 6.0f);
		}
	}


		
	//if(finalColor.a >= 0.00001f && finalColor.a <= 0.9999f) //**tillman - haxl�sning?**
	//{
		//finalColor = SSAO(input.tex, NormalAndDepth, Position); //**tillman opt(position tex)**
	
	finalColor.rgb = HighlightArea(WorldPos.xz, finalColor.rgb); //Position is in world space
	finalColor.a = 1.0f;
		
	//}



	///////////////////////////////////////////////////////////////////
	//							Basic fog:							//
	//////////////////////////////////////////////////////////////////
	finalColor = saturate(finalColor);
	float fogDepth = NormsAndDepth.w;
	if(fogDepth > 0.75f)
	{
		float fogfactor = (fogDepth - 0.75f) * 4.1f;	// Linear scale the last 25% of farclip, but a little more 
		finalColor = lerp(finalColor, float4(0.45f, 0.45f, 0.45f, 1.0f), saturate(fogfactor));
	}
			

	
	return saturate(finalColor);
}

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 BasicTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    

		SetDepthStencilState( DisableDepthWrite, 0 );
	    SetRasterizerState( NoCulling );
		
		SetBlendState( SrcAlphaBlendingAdd, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }  
}