#include "Curve.h"

const char* Curve::GetName()
{
	return m_Name.c_str();
}

unsigned int Curve::GetNumControlPoints()
{
	return m_ControlPoints.size();
}
		
const BTHFBX_VEC3& Curve::GetControlPoint(unsigned int index)
{
	return m_ControlPoints[index];
}

void Curve::SetName(const char* name)
{
	m_Name = name;
}

void Curve::AddControlPoint(const BTHFBX_VEC3& point)
{
	m_ControlPoints.push_back(point);
}