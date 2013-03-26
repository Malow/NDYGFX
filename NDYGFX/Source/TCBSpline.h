//Written by Markus Tillman.

/*
	Class for making smooth, adjustable splines (curves) along a set of controlpoints.
	Todo: use src-tangent (continuity parameter) (GetPoint()) & if necessary delete src and and dest tangents in Clear()**
*/

#pragma once

//#include "DirectX.h"
//#include <string>
class Vector3;

class TCBSpline
{
	private:
		//Tension: Controls how sharply the curve bends.
		//Bias: Controls the direction of the curve as it passes through the keypoint.
		//Continuity: Controls how rapid the change is in speed and direction.
		float mTension;		//Controls how sharp the curve is going to be at a joint by modifying the length of the tangent vector.
							//Higher values give sharper bends, and negative values give less taut curves near the joints.
							//If tension is greater than 1, there will be a loop at the joint.
		float mBias;		//Influences the direction of the tangent(and, indirectly, the length)
							//A positive bias gives a bend that is more directed toward the chord: Point(i) - Point(i - 1),
							//and a negative bias gives a bend that is more directed toward the other chord: Point(i + 1) - Point(i).
		float mContinuity;	//Controls how sharp the corners are going to be at the joints by using a source and destination tangents.
							//Lower values produces sharper corners while higher values have the opposite effect.
	
		bool			mEndsAreConnected;
		int				mNrOfControlPoints;
		int				mControlCap;
		Vector3**	mControlPoints;
		Vector3**	mSource;
		Vector3**	mDestination;

	private:
		void Expand();
		void CalculateTangents(int ctrlPointIndex);

	public:
		TCBSpline(bool connectEnds, float tension = 0.0f, float bias = 0.0f, float continuity = 0.0f);
		~TCBSpline();
		bool Init();

		bool AreEndsConnected() const;
		int GetNrOfControlPoints() const;
		float GetTension() const;
		float GetBias() const;
		float GetContinuity() const;

		void SetTension(float tension);
		void SetBias(float bias);
		void SetContinuity(float continuity);

		void AddControlPoint(Vector3& controlPoint);
		/*! Returns the interpolated position along the set of control points,
			Example:	t = 0 would return the first control point,
						t = 1 would return the last control point,
						and everything inbetween returns an interpolation between the 2 closest control points to t.
		*/
		Vector3 GetPoint(float t) const;
		/*! Return nrOfPoints of interpolated positions along the set of control points. */
		Vector3** CalculatePoints(int nrOfPoints);
		void WriteControlPointsToFile(const char* fileName); 
		bool ReadControlPointsFromFile(const char* fileName); 
		/*! Empties (clears) the arrays used. This function should be called prior to adding a new set of control points. */
		void Clear();
};