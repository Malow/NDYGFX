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
	virtual ~Matrix4();

	Matrix4 operator*(const Matrix4& m) const
	{
		Matrix4 ret;
		for(int i = 0; i < 4; i++)
		{
			for(int u = 0; u < 4; u++)
			{
				ret.value[i * 4 + u] = this->value[i * 4] * m.value[u] + this->value[i * 4 + 1] * m.value[u+4] + 
					this->value[i * 4 + 2] * m.value[u + 8] + this->value[i * 4 + 3] * m.value[u + 12];
			}
		}
		return ret;
	}

	Vector3 operator*(const Vector3& vec) const
	{
		Vector3 ret;
		ret.x = this->value[0] * vec.x + this->value[1] * vec.y + this->value[2] * vec.z + this->value[3];
		ret.y = this->value[4] * vec.x + this->value[5] * vec.y + this->value[6] * vec.z + this->value[7];
		ret.z = this->value[8] * vec.x + this->value[9] * vec.y + this->value[10] * vec.z + this->value[11];
		return ret;
	}

};