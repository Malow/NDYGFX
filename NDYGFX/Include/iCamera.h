#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "Vector.h"
#include "iMesh.h"
#include "iGraphicsEngineParams.h"
//#include "iTerrain.h"

extern "C"
{
	class DECLDIR iCamera
	{	
	public:
		iCamera() {};
		virtual ~iCamera() {};

		virtual Vector3 GetPosition() = 0;
		virtual void SetPosition(Vector3 pos) = 0;
		virtual void Move(Vector3 moveBy) = 0;

		virtual void MoveForward(float diff) = 0;
		virtual void MoveBackward(float diff) = 0;
		virtual void MoveLeft(float diff) = 0;
		virtual void MoveRight(float diff) = 0;
		virtual void MoveUp(float diff) = 0;
		virtual void MoveDown(float diff) = 0;

		virtual Vector3 GetForward() const = 0;
		virtual void SetForward(Vector3 forward) = 0;
		virtual void LookAt(Vector3 target) = 0;
		virtual Vector3 GetUpVector() const = 0;
		virtual void SetUpVector(Vector3 up) = 0;
		virtual Vector3 GetRightVector() const = 0;

		virtual void Update(float delta) = 0;

		virtual void SetSpeed(float speed) = 0;
		virtual float GetSpeed() const = 0;
		virtual void SetSensitivity(float sens) = 0;
		virtual float GetSensitivity() const = 0;

		virtual void SetBoundries(Vector3 minBoundries, Vector3 maxBoundries) = 0;
		virtual void DisableBoundries() = 0;

		//virtual void WalkOnTerrain(iTerrain* terrain) = 0;
		virtual void StopWalkingOnTerrain() = 0;

		virtual void SetMesh(iMesh* target, Vector3 distanceFromCamera, Vector3 defaultMeshDirection = Vector3(0, 0, -1)) = 0;
		virtual void SetMesh(iMesh* target, const char* bone, Vector3 defaultMeshDirection = Vector3(0, 0, -1)) = 0;

		virtual void RemoveMesh() = 0;
		
		virtual void SetUpdateCamera(bool update) = 0;

		virtual void SetActiveWindowDisabling(bool dis) = 0;

		virtual Vector3 Get3DPickingRay() = 0;

		virtual CameraType GetCameraType() const = 0;
		virtual void MoveOnlyInXZ(bool DoItOrNotThatIsTheQuestion) = 0;
	};
}