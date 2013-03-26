//-----------------------------------------------------------------------------------------------------
//
//	Originally made by MaloW.
//
//	Cascaded shadow maps written by Tillman for projekt Not Deat Yet at Blekinge tekniska högskola.
//
//-----------------------------------------------------------------------------------------------------
#include "stdafx.fx"
#include "SSAO.fx"
#include "PPHighlightArea.fx"

// Marcus Löwegren

// For textures
Texture2D Texture;
Texture2D NormalAndDepth;
Texture2D Position;
Texture2D Specular;

struct Light
{
	float4 LightPosition;
	float4 LightColor;
	float LightIntensity;
	matrix LightViewProj;
};

struct Cascade
{
	matrix viewProj;
};

struct SunLight
{
	float4 Direction;
	float4 LightColor;
	float LightIntensity;
};

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
	float3		gCameraPosition;
	float4x4	gCameraVP;
	float		gNrOfLights;
	Light		gLights[10];
	float3		gSceneAmbientLight;
	bool		gUseSun;
	SunLight	gSun;
	Cascade		gCascades[10];

	//Single sampling
	float SMAP_DX; 

	//PCF sampling
	float PCF_SIZE;
	float PCF_SIZE_SQUARED;

	//Multiple sampling
	bool blendCascades; //**ersätta med blenddistance
	float blendDistance; //the number of pixels to blend.
	float blendStrength; //How much to blend. **to use?**

	//Both/other
	uint nrOfCascades;
	float4 cascadeFarPlanes;
	matrix cameraViewMatrix;


	bool useShadow;
};

struct DummyStruct //VS & GS input (unused)
{
	
};

struct PSIn
{
	float4 Pos : SV_POSITION;
	float2 tex : TEXCOORD;
};



//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
DummyStruct VSScene(DummyStruct input)
{
	return input;
}

// GS
[maxvertexcount(4)]
void GS(point DummyStruct dummy[1], inout TriangleStream<PSIn> triStream )
{
	PSIn output = (PSIn)0;

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
float BlendCascades(uint cascadeIndex, uint otherCascadeIndex, float2 pixelPosTexelSpace, float pixelDepth, float CSM_SHADOW_EPSILON)
{
	float shadow = 0.0f;

	if(cascadeIndex == 0)
	{
		shadow += (CascadedShadowMap[0].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
	}
	else if(cascadeIndex == 1)
	{
		shadow += (CascadedShadowMap[1].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
	}
	else if(cascadeIndex == 2)
	{
		shadow += (CascadedShadowMap[2].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
	}
	else if(cascadeIndex == 3)
	{
		shadow += (CascadedShadowMap[3].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
	}

	if(otherCascadeIndex == 0)
	{
		shadow += (CascadedShadowMap[0].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
	}
	else if(otherCascadeIndex == 1)
	{
		shadow += (CascadedShadowMap[1].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
	}
	else if(otherCascadeIndex == 2)
	{
		shadow += (CascadedShadowMap[2].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
	}
	else if(otherCascadeIndex == 3)
	{
		shadow += (CascadedShadowMap[3].SampleLevel(shadowMapSampler, pixelPosTexelSpace + float2(SMAP_DX, SMAP_DX), 0).r + CSM_SHADOW_EPSILON < pixelDepth) ? 0.0f : 1.0f;
	}

	shadow *= 0.5f;
						
	return shadow;
}
//Return a floating point value representing the shadow factor. Range[0,1].
float SampleCascades(uint cascadeIndex, uint otherCascadeIndex, float2 pixelPosTexelSpace, float pixelDepth) 
{
	//**
	if(otherCascadeIndex < 0 || otherCascadeIndex > (nrOfCascades - 1))
	{
		//return 0.0f; //Debug
	}


	float CSM_SHADOW_EPSILON = 0.004f;// - (cascadeIndex * 0.0005f);// - (pixelDepth * 0.001f); //**finputsa, lägga till pixelDepth?**
	float shadow = 0.0f; 
	bool withinBlendingDistance = false;
	if(blendCascades) //global variable
	{
		//**START OF BLENDING**

		//Convert pixel depth to view space units.
		/*float pixelDepthViewSpace = pixelDepth * cascadeFarPlanes[nrOfCascades - 1];
		
		//Check if otherCascadeIndex is less than cascadeIndex to know what plane to check the pixel with.
		if(otherCascadeIndex < cascadeIndex)
		{
			//If the depth of the pixel is within blending distance of the far plane, sample both cascades and blend.
			if(pixelDepthViewSpace > (cascadeFarPlanes[otherCascadeIndex] - 1.1f) && pixelDepthViewSpace < (cascadeFarPlanes[otherCascadeIndex] + 1.1f))
			{
				//**lerp result? eller djupdata?**
				return -1.0f;
				shadow = BlendCascades(cascadeIndex, otherCascadeIndex, pixelPosTexelSpace, pixelDepth, CSM_SHADOW_EPSILON);
				withinBlendingDistance = true;
			}
		}
		else
		{
			//If the depth of the pixel is within blending distance of the far plane, sample both cascades and blend.
			/*if(pixelDepthViewSpace > (cascadeFarPlanes[cascadeIndex] - 1.1f) && pixelDepthViewSpace < (cascadeFarPlanes[cascadeIndex] + 1.1f))
			{
				//**lerp result? eller djupdata?**
				return -1.0f;
				shadow = BlendCascades(cascadeIndex, otherCascadeIndex, pixelPosTexelSpace, pixelDepth, CSM_SHADOW_EPSILON);
				withinBlendingDistance = true;
			}*/
		//}*/

		//If pixel was not in blending distance (and therefore not blended), do normal PCF. //**kanske strunta i bool helt?**
		//if(!withinBlendingDistance) //**kanske strunta i bool helt?**

		//**END OF BLENDING**
		{


			//Todo: check
			//if true, sample both and lerp samples**
			//else: kod under**

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
float4 PSScene(PSIn input) : SV_Target
{	
	float3 DiffuseColor = Texture.Sample(linearSampler, input.tex).xyz;	

	float4 NormsAndDepth = NormalAndDepth.Sample(linearSampler, input.tex);
	
	float4 WorldPosAndObjectType = Position.Sample(linearSampler, input.tex);
	float4 WorldPos = float4(WorldPosAndObjectType.xyz, 1.0f);

	float4 AmbientLight = float4(gSceneAmbientLight, 1.0f);

	float4 SpecularColor = Specular.Sample(linearSampler, input.tex);
	float SpecularPower = SpecularColor.w;
	
	float diffuseLighting = 0.0f;
	float specLighting = 0.0f;

	float3 diffLightColor = float3(0, 0, 0);
	float3 specLightColor = float3(0, 0, 0);
	
	for(int i = 0; i < gNrOfLights; i++)
	{
		float3 LightDirection = WorldPos.xyz - gLights[i].LightPosition.xyz;
		float DistanceToLight = length(LightDirection);
		LightDirection = normalize(LightDirection);

		// Diff light
		float difflight = saturate(dot(NormsAndDepth.xyz, -LightDirection));

		// Spec Light
		float3 h = normalize(normalize(gCameraPosition.xyz - WorldPos.xyz) - LightDirection);
		float speclight = pow(saturate(dot(h, NormsAndDepth.xyz)), SpecularPower);


		// Shadow Mappings
		float4 posLight = mul(WorldPos, gLights[i].LightViewProj);
		posLight.xy /= posLight.w;


		//Compute shadow map tex coord
		float2 smTex = float2(0.5f*posLight.x, -0.5f*posLight.y) + 0.5f;

		// Compute pixel depth for shadowing.
		float depth = posLight.z / posLight.w;

		float SHADOW_EPSILON = 0.00001f;			////////////// PUT THIS WHERE?

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
		float coef = (gLights[i].LightIntensity / 1000.0f) + (DistanceToLight * DistanceToLight) / (gLights[i].LightIntensity * gLights[i].LightIntensity);

		difflight /= coef;

		/// For slower fall-off on spec light. Doesnt work with quad-based culling tho.
		//if(coef > 10.0f)
		//	speclight /= coef / 10.0f;
			
		speclight /= coef;
		
		difflight *= shadow;
		speclight *= shadow;
		
		diffuseLighting += difflight;
		specLighting += speclight;

		specLightColor += speclight * gLights[i].LightColor.xyz;
		diffLightColor += difflight * gLights[i].LightColor.xyz;
	}
	
	if(gNrOfLights > 0)
	{
		diffuseLighting = saturate(diffuseLighting / gNrOfLights);
		specLighting = saturate(specLighting / gNrOfLights);

		specLightColor = saturate(specLightColor / gNrOfLights);
		diffLightColor = saturate(diffLightColor / gNrOfLights);

		DiffuseColor.xyz += diffLightColor;
		SpecularColor.xyz += diffLightColor;
	}
	
	
	// Sun
	float diffShadow = 1.0f;
	if(gUseSun)
	{
		// Diff light
		float diffLight = saturate(dot(NormsAndDepth.xyz, -gSun.Direction.xyz)) * gSun.LightIntensity;
		// Spec Light
		float3 h = normalize(normalize(gCameraPosition.xyz - WorldPos.xyz) - gSun.Direction.xyz);
		float specLight = pow(saturate(dot(h, NormsAndDepth.xyz)), SpecularPower) * gSun.LightIntensity;

		
		//CSM SHADOW: 
		if(useShadow)
		{
			//Determine the shadow map to use:
			//First transform the pixel from world space to CAMERA view space.
			float pixelDepthCameraViewSpace = mul(WorldPos, cameraViewMatrix).z; 
			//Find the index of the cascade(frustum slice) it is inside of.
			uint cascadeIndex = -1;
			cascadeIndex = FindCascade(pixelDepthCameraViewSpace);

			//Todo: early exit: if (CascadeIndex == -1)


			//Determine the second cascade to use if blending between cascades is enabled:
			uint otherCascadeIndex = -1; 
			if(blendCascades)
			{
				otherCascadeIndex = FindCascadeToBlendWith(cascadeIndex, pixelDepthCameraViewSpace); 
			}

			//Sample the cascade(s):
			float shadow = 0.0f;
		
			//Get the texture coordinates to sample with by first transforming the pixel from world space to LIGHT's clip space xy[-w,w], z[0,w].
			float4 pixelPosTexelSpace = mul(WorldPos, gCascades[cascadeIndex].viewProj); 
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
			diffShadow = shadow; //Used for billboards.
		}
		
		//**end of CSM**
		
		diffuseLighting += diffLight;
		specLighting += specLight;
		
		diffuseLighting = saturate(diffuseLighting);
		specLighting = saturate(specLighting);

		//**DEBUG**
		/*if(shadow == -1.0f)
		{
			diffuseLighting = 1.0f;
		}*/
		/*if(otherCascadeIndex < cascadeIndex)
		{
			DiffuseColor = float3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			DiffuseColor = float3(0.0f, 0.0f, 1.0f);
		}*/
	}

	// Reduction of ambientlight if diffuse and spec is high to prevent oversaturation.
	AmbientLight /= 1.0f + diffuseLighting + specLighting;	

	float4 finalColor = float4((							
		AmbientLight.xyz * DiffuseColor + 
		DiffuseColor * diffuseLighting + 
		SpecularColor.xyz * specLighting), 
		1.0f);

	if(gUseSun)
		finalColor.xyz *= gSun.LightColor.xyz;

	
	
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
	

		
	//if(finalColor.a >= 0.00001f && finalColor.a <= 0.9999f)
	//{
		//finalColor = SSAO(input.tex, NormalAndDepth, Position); 
	
	finalColor.rgb = HighlightArea(WorldPos.xz, finalColor.rgb); //Position is in world space
	finalColor.a = 1.0f;
		
	//}

	
	//Skip shadow "lighting" and specular
	if(WorldPosAndObjectType.w == OBJECT_TYPE_BILLBOARD)
	{	
		/*
		finalColor = float4((							
		AmbientLight.xyz * DiffuseColor + 
		DiffuseColor * diffuseLighting + 
		SpecularColor.xyz * specLighting), 
		1.0f);

		if(gUseSun)
			finalColor.xyz *= gSun.LightColor.xyz;
			*/
		//finalColor = float4((float4(gSceneAmbientLight, 1.0f) * DiffuseColor) + (DiffuseColor * diffShadow), 1.0f);// = Texture.Sample(linearSampler, input.tex).xyz;	
	
		//finalColor = float4(AmbientLight.xyz * DiffuseColor + DiffuseColor * diffuseLighting, 1.0f);// = Texture.Sample(linearSampler, input.tex).xyz;	
	}
	
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
		
	
	

	//**DEBUG NORMAL TEST**
	/*if(WorldPosAndObjectType.w == OBJECT_TYPE_TERRAIN)
	{
		return float4(1.0f, 1.0f, 0.0f, 1.0f);
	}*/
	
	//return float4(NormsAndDepth.xyz, 1.0f);
	
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