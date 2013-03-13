#pragma once

#include <Vector.h>

namespace WorldPhysics
{
	struct CollisionData 
	{
		bool collision;
		Vector3 pos;
		float distance;
	};

	inline bool RayVSTriangle(const Vector3& rayOrigin, const Vector3& rayDirection, const Vector3& v0, const Vector3& v1, const Vector3& v2, CollisionData& tempCD)
	{
		float eps = 0.000001f;
		Vector3 e1 = v1 - v0;
		Vector3 e2 = v2 - v0;
		Vector3 q = e2.GetCrossProduct(rayDirection);
		float a = e1.GetDotProduct(q);
	
		if(a > -eps && a < eps)
			return false;
		
		float f = 1.0f / a;
		Vector3 s = rayOrigin - v0;
		float u = f * (s.GetDotProduct(q));
	
		if(u < 0.0f)
			return false;
	
		Vector3 r = e1.GetCrossProduct(s);
		float v = f * rayDirection.GetDotProduct(r);
	
		if(v < 0.0f || u + v > 1.0f)
			return false;
	
		Vector3 collPos;
		float w = 1 - (u + v);
	
		collPos = (v0 * w) + (v1 * u) + (v2 * v);
				
		float nrOfdirs = 999999999.0f;
		if(rayDirection.x != 0.0f)
			nrOfdirs = (collPos.x - rayOrigin.x) / rayDirection.x;
		else if(rayDirection.y != 0.0f)
			nrOfdirs = (collPos.y - rayOrigin.y) / rayDirection.y;
		else if(rayDirection.z != 0.0f)
			nrOfdirs = (collPos.z - rayOrigin.z) / rayDirection.z;
	
		tempCD.collision = true;
		tempCD.pos.x = collPos.x;
		tempCD.pos.y = collPos.y;
		tempCD.pos.z = collPos.z;
		tempCD.distance = nrOfdirs;
	
		return true;
	}
}