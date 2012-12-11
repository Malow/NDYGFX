//------------------------------------------------------------------------------------------------------
//	Written by Markus Tillman at Blekinga Tekniska Högskola for project "Not dead yet".
//
//	Post-process effect: Highlight an area.
//	Draws a circle on existing geometry.
//	Requirements: World positions of pixels.
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
//	Global variables (non-numeric values cannot be added to a constantbuffer.)
//------------------------------------------------------------------------------------------------------
Texture2D previewTex;

//------------------------------------------------------------------------------------------------------
//	Constant buffers
//------------------------------------------------------------------------------------------------------
cbuffer EveryFrame
{
	float4 data; //x = inner radius, y = outer radius, z&w = target pos (x,z).
};
//------------------------------------------------------------------------------------------------------
//	Hightlighting function
//------------------------------------------------------------------------------------------------------
float3 HighlightArea(float2 pixelWorldPosition, float3 finalColor)
{
	float tinnerRadius = 1.0f;
	float touterRadius = 2.0f;
	float2 ttargetPos = float2(0,0);

	float dist = length(ttargetPos - pixelWorldPosition); //"transform" to origo
	if(dist < tinnerRadius * 1.02f && dist > tinnerRadius * 0.99f)
	{
		finalColor = float3(1,1,1); 
	}
	else if(dist < touterRadius * 1.01f && dist > touterRadius * 0.99f)
	{
		finalColor = float3(1,1,1);
	}
	/*if(dist < touterRadius)
	{
		finalColor += tareaColor; 
		if(dist < tinnerRadius)
		{
			finalColor += tareaColor; //add color again (double entensity of area color)
		}
	}*/

	/*
	float tinnerRadius = 1.0f;
	float touterRadius = 1.5f;
	float2 ttargetPos = float2(0,0);
	float3 tareaColor = float3(0.25f,0,0);

	float dist = length(ttargetPos - pixelWorldPosition);
	if(dist < touterRadius)
	{
		finalColor += tareaColor; 
		//if(dist < tinnerRadius)
		//{
		//	finalColor += tareaColor; //add color again (double entensity of area color)
		//}
	}
*/

	return saturate(finalColor); 
}

