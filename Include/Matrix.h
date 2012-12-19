#pragma once

#include "Vector.h"

class Matrix4
{
public:
	float value[4][4];

	Matrix4()
	{
		for(int i = 0; i < 4; i++)
			for(int u = 0; u < 4; u++)
				this->value[i][u] = 0.0f;

		this->value[0][0] = 1.0f;
		this->value[1][1] = 1.0f;
		this->value[2][2] = 1.0f;
		this->value[3][3] = 1.0f;
	}
	virtual ~Matrix4() { }

	Matrix4 operator*(const Matrix4& m)
	{
		Matrix4 ret;
		for(int i = 0; i < 4; i++)
		{
			for(int u = 0; u < 4; u++)
			{
				ret.value[i][u] = this->value[i][0] * m.value[0][u] + this->value[i][1] * m.value[1][u] + 
					this->value[i][2] * m.value[2][u] + this->value[i][3] * m.value[3][u];
			}
		}
		return ret;
	}

	Vector3 operator*(const Vector3& vec)
	{
		Vector3 ret;
		ret.x = this->value[0][0] * vec.x + this->value[0][1] * vec.y + this->value[0][2] * vec.z + this->value[0][3];
		ret.y = this->value[1][0] * vec.x + this->value[1][1] * vec.y + this->value[1][2] * vec.z + this->value[1][3];
		ret.z = this->value[2][0] * vec.x + this->value[2][1] * vec.y + this->value[2][2] * vec.z + this->value[2][3];
		return ret;
	}

};