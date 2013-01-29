#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"

#pragma warning ( push )
#pragma warning ( disable : 4290 ) // C++ Exception Does Nothing

extern "C"
{
	class DECLDIR iTerrain// : public virtual iMesh
	{	
		protected:
			iTerrain() {}
			virtual ~iTerrain() {}

		public:
			//GET-FUNCTIONS.
			//Object data
			/*  Get Position Of Terrain. */
			virtual Vector3 GetPosition() const = 0;

			//Vertex data
			/* Returns the Y-position in local space on the terrain at x,z in local space. */
			virtual float GetYPositionAt(float x, float z) const throw(const char*) = 0; 

			//SET-FUNCTIONS.
			//Object data 
			/* Sets the scale for the terrain. */
			virtual void SetScale(const Vector3& scale) = 0;
			/* Set diffuse color. */
			virtual void SetDiffuseColor(const Vector3& color) = 0;

			//Vertex data
			/* Data = array of floats containing y-values for each vertex. */
			virtual void SetHeightMap(float const* const data) = 0;

			//Texture data
			/*	
				Set 0-8 textures to blend. 
				the parameter is assumed to contain 8 names.
			*/
			virtual void SetTextures(char const* const* const fileNames) = 0;
			/*	Size(width or height) = dimension of blend map in texels, data = pointer of 1D array of floats 
				(4 sequential floats in the array represent the R,B,G,A-channels). Range [0,1]. 
				
			*/
			virtual void SetBlendMaps(unsigned int nrOfBlendMaps, unsigned int* sizes, float const* const* const data) = 0;
			/* Set the number of times a texture shall repeat over a surface. */
			virtual void SetTextureScale(float textureScale = 1.0f) = 0; 
			
			//Editor
			/*	nrOfNodesPerSide^2 = total amount of AI nodes on the terrain. 
				data = array of BYTES indicating if a node is traversable or not.
				data is expected to be in left-right, top-down order.
				Keep in mind that char variable = 255 will be 1(true) in the shader.
			*/
			virtual void SetAIGrid(unsigned int nrOfAINodesPerSide, void* aiData) = 0;
			
			/*	Set how thick the grid will be. Measured in world units. */
			virtual void SetAIGridThickness(float thickness = 0.001f) = 0;
			/*	Determines if the AI map shall be used or not.	*/
			virtual void UseAIMap(bool use) = 0;


			// TODO: Height Extraction

			/*virtual Vector3 GetRotation() const = 0;
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

#pragma warning ( pop )