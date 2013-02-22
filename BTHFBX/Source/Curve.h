#pragma once

#pragma warning(disable : 4995)

#include <vector>
#include <string>
#include "../Include/BTHFbxCurve.h"

class Curve : public IBTHFbxCurve
{
	std::vector<BTHFBX_VEC3>	m_ControlPoints;
	std::string m_Name;

public:
		virtual const char* GetName();

		virtual unsigned int GetNumControlPoints();
		
		virtual const BTHFBX_VEC3& GetControlPoint(unsigned int index);

		void SetName(const char* name);

		void AddControlPoint(const BTHFBX_VEC3& point);
};