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
		Matrix4 r;
		r.value[0][0] = value[0][0] * m.value[0][0] + value[0][1] * m.value[1][0] + value[0][2] * m.value[2][0] + value[0][3] * m.value[3][0];
		r.value[0][1] = value[0][0] * m.value[0][1] + value[0][1] * m.value[1][1] + value[0][2] * m.value[2][1] + value[0][3] * m.value[3][1];
		r.value[0][2] = value[0][0] * m.value[0][2] + value[0][1] * m.value[1][2] + value[0][2] * m.value[2][2] + value[0][3] * m.value[3][2];
		r.value[0][3] = value[0][0] * m.value[0][3] + value[0][1] * m.value[1][3] + value[0][2] * m.value[2][3] + value[0][3] * m.value[3][3];
		
		r.value[1][0] = value[1][0] * m.value[0][0] + value[1][1] * m.value[1][0] + value[1][2] * m.value[2][0] + value[1][3] * m.value[3][0];
		r.value[1][1] = value[1][0] * m.value[0][1] + value[1][1] * m.value[1][1] + value[1][2] * m.value[2][1] + value[1][3] * m.value[3][1];
		r.value[1][2] = value[1][0] * m.value[0][2] + value[1][1] * m.value[1][2] + value[1][2] * m.value[2][2] + value[1][3] * m.value[3][2];
		r.value[1][3] = value[1][0] * m.value[0][3] + value[1][1] * m.value[1][3] + value[1][2] * m.value[2][3] + value[1][3] * m.value[3][3];
		
		r.value[2][0] = value[2][0] * m.value[0][0] + value[2][1] * m.value[1][0] + value[2][2] * m.value[2][0] + value[2][3] * m.value[3][0];
		r.value[2][1] = value[2][0] * m.value[0][1] + value[2][1] * m.value[1][1] + value[2][2] * m.value[2][1] + value[2][3] * m.value[3][1];
		r.value[2][2] = value[2][0] * m.value[0][2] + value[2][1] * m.value[1][2] + value[2][2] * m.value[2][2] + value[2][3] * m.value[3][2];
		r.value[2][3] = value[2][0] * m.value[0][3] + value[2][1] * m.value[1][3] + value[2][2] * m.value[2][3] + value[2][3] * m.value[3][3];
		
		r.value[3][0] = value[3][0] * m.value[0][0] + value[3][1] * m.value[1][0] + value[3][2] * m.value[2][0] + value[3][3] * m.value[3][0];
		r.value[3][1] = value[3][0] * m.value[0][1] + value[3][1] * m.value[1][1] + value[3][2] * m.value[2][1] + value[3][3] * m.value[3][1];
		r.value[3][2] = value[3][0] * m.value[0][2] + value[3][1] * m.value[1][2] + value[3][2] * m.value[2][2] + value[3][3] * m.value[3][2];
		r.value[3][3] = value[3][0] * m.value[0][3] + value[3][1] * m.value[1][3] + value[3][2] * m.value[2][3] + value[3][3] * m.value[3][3]; 

		return r;
	}

	Vector3 operator*(const Vector3& vec)
	{
		Vector4 temp = Vector4(vec.x, vec.y, vec.z, 1.0f);
		temp = this->operator*(temp);
		return Vector3(temp.x, temp.y, temp.z);
	}

	Vector4 operator*(const Vector4& vec)
	{
		Vector4 pos;
		float x = vec.x * this->value[0][0] + vec.y * this->value[0][1] +
			vec.z * this->value[0][2] + this->value[0][3];
		float y = vec.x * this->value[1][0] + vec.y * this->value[1][1] +
			vec.z * this->value[1][2] + this->value[1][3];
		float z = vec.x * this->value[2][0] + vec.y * this->value[2][1] +
			vec.z * this->value[2][2] + this->value[2][3];
		float w = vec.x * this->value[3][0] + vec.y * this->value[3][1] +
			vec.z * this->value[3][2] + this->value[3][3];
		pos.x = x/w;
		pos.y = y/w;
		pos.z = z/w;
		return pos;
	}

};