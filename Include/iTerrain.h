#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"


extern "C"
{
	class DECLDIR iTerrain// : public virtual iMesh
	{	
		protected:
			iTerrain() {}

		public:
			//Returns the Y-position on the terrain at x,z.
			virtual float GetYPositionAt(float x, float z) = 0; 

			//Sets the scale for the terrain.
			virtual void SetScale(Vector3& scale) = 0;
			//Data = array of floats containing y-values for each vertex. Range [0,1].
			virtual void SetHeightMap(float* data) = 0;
			//Set 0-3 textures to blend.
			virtual void SetTextures(const char** fileNames) = 0;
			//Width & height = dimension of blend map in texels, data = pointer of 1D array of floats (3 sequential floats in the array represent the R,B&G-channels).
			virtual bool SetBlendMap(unsigned int size, float* data) = 0;


			/*virtual Vector3 GetPosition() const = 0;
			virtual Vector3 GetRotation() const = 0;
			virtual Vector3 GetScaling() const = 0;

			virtual void SetPosition(const Vector3& pos) = 0;
			virtual void SetQuaternion(const Vector4& quat) = 0;
			virtual void MoveBy(const Vector3& moveby) = 0;
			virtual void Rotate(const Vector3& radians) = 0;
			virtual void Rotate(const Vector4& quat) = 0;
			virtual void RotateAxis(const Vector3& around, float angle) = 0;
			virtual void Scale(const Vector3& scale) = 0;
			virtual void Scale(float scale) = 0;*/
	};
}