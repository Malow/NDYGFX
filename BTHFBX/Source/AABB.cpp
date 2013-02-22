#include "AABB.h"

bool ClipSegment(double min, double max, double a, double b, double d, double& t0, double& t1)
{
	const double threshold = 1.0e-6f;

	if (fabs(d) < threshold)
	{
		if (d > 0.0f)
		{
			return !(b < min || a > max);
		}
		else
		{
			return !(a < min || b > max);
		}
	}

	double u0, u1;

	u0 = (min - a) / (d);
	u1 = (max - a) / (d);

	if (u0 > u1) 
	{
		double tmp = u0;
		u0 = u1;
		u1 = tmp;
	}

	if (u1 < t0 || u0 > t1)
	{
		return false;
	}

	t0 = u0 > t0 ? u0 : t0; //max
	t1 = u1 < t1 ? u1 : t1; //min

	if (t1 < t0)
	{
		return false;
	}

	return true;
}

bool ClipSegment(const BTHFBX_AABB_DATA& box, FbxVector4 start, FbxVector4 end, float& dist1, float& dist2)
{
	FbxVector4 S = start;
	FbxVector4 D = end - start;

	double t0 = 0.0f, t1 = 1.0f;

	if (!ClipSegment(box.Min.x, box.Max.x, start[0], end[0], D[0], t0, t1)) 
	{
		return false;
	}

	if (!ClipSegment(box.Min.y, box.Max.y, start[1], end[1], D[1], t0, t1))
	{
		return false;
	}

	if (!ClipSegment(box.Min.z, box.Max.z, start[2], end[2], D[2], t0, t1)) 
	{
		return false;
	}

	dist1 *= (float)t0;
	dist2 *= (float)t1;

	return true;
}

float BoxVsRay(FbxVector4 rayOrigin, FbxVector4 rayDir, const BTHFBX_AABB_DATA& box)
{
	float d = 1.0f, d2 = 1.0f;

	if(ClipSegment(box, rayOrigin, rayOrigin + rayDir * 10000,d,d2))
	{
		return (d < d2) ? d : d2;
	}

	return -1.0f;
}