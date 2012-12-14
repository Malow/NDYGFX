//------------------------------------------------------------------------------------------------------
//	Written by Markus Tillman at Blekinga Tekniska Högskola for project "Not dead yet".
//
//	Post-process effect: Highlight an area.
//	Draws a circle on existing geometry.
//	Requirements: World positions of pixels.
//------------------------------------------------------------------------------------------------------
#include "stdafx.fx"

//------------------------------------------------------------------------------------------------------
//	Constant buffers
//------------------------------------------------------------------------------------------------------
cbuffer EveryFramePPHA
{
	float4 dataPPHA; //x = inner radius, y = outer radius, z&w = target pos (x,z).
};
//------------------------------------------------------------------------------------------------------
//	Hightlighting function
//------------------------------------------------------------------------------------------------------
float3 HighlightArea(float2 pixelWorldPosition, float3 finalColor)
{
	//Transform to origo
	float dist = length(dataPPHA.zw - pixelWorldPosition);
	//Draw circles
	if(	(dist < dataPPHA.x * 1.02f && dist > dataPPHA.x * 0.99f) 
	||	(dist < dataPPHA.y * 1.01f && dist > dataPPHA.y * 0.99f))
	{
		finalColor = float3(1,1,1); 
	}

	return saturate(finalColor); 
}

