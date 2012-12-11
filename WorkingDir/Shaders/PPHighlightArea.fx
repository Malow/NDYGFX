//------------------------------------------------------------------------------------------------------
//	Post-process effect: Highlight an area.
//	Draws a circle on existing geometry.
//	Requirements: World positions of pixels.
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
//	Constant buffers
//------------------------------------------------------------------------------------------------------
cbuffer EveryFrame
{
	float innerRadius;
	float outerRadius;
	float2 targetPos; //in world
	float3 areaColor;
};
//------------------------------------------------------------------------------------------------------
//	Hightlighting function
//------------------------------------------------------------------------------------------------------
float3 HighlightArea(float2 pixelWorldPosition, float3 finalColor)
{
	float tinnerRadius = 1.0f;
	float touterRadius = 3.0f;
	float2 ttargetPos = float2(0,0);
	float3 tareaColor = float3(0.5f,0,0);

	if(length(ttargetPos - pixelWorldPosition) < tinnerRadius)
	{
		finalColor += tareaColor; 
	}

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

