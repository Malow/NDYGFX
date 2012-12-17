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
			virtual ~iTerrain() {}

		public:
			//Returns the Y-position in local space on the terrain at x,z in local space.
			virtual float GetYPositionAt(float x, float z) const throw(const char*) = 0; 

			//Sets the scale for the terrain.
			virtual void SetScale(const Vector3& scale) = 0;
			//Data = array of floats containing y-values for each vertex.
			virtual void SetHeightMap(float const* const data) = 0;
			//Set 0-4 textures to blend.
			virtual void SetTextures(char const* const* const fileNames) = 0;
			//Size(width or height) = dimension of blend map in texels, data = pointer of 1D array of floats (4 sequential floats in the array represent the R,B,G,A-channels). Range [0,1].
			virtual void SetBlendMap(unsigned int size, float const* const data) = 0;
			//Set diffuse color.
			virtual void SetDiffuseColor(const Vector3& color) = 0;
			//Set the number of times a texture shall repeat over a surface.
			virtual void SetTextureScale(float textureScale = 1.0f) = 0; //**tillman - default value**


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