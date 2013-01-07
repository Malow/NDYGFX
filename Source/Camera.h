#ifndef CAMERA_H
#define CAMERA_H

#include "Terrain.h"
#include "GraphicsEngineParameters.h"
#include "Mesh.h"
#include "CursorControl.h"
#include "iCamera.h"
#include "MaloW.h"
#include "MaloWFileDebug.h"

class Camera : public iCamera
{
protected:
	D3DXMATRIX view;
	D3DXMATRIX projection;

	D3DXVECTOR3 pos;
	D3DXVECTOR3 forward;
	D3DXVECTOR3 up;
	Terrain* terrain;
	Mesh* followTarget;
	
	HWND g_hWnd;

	float angleX;
	float angleY;

	void MoveToTerrain();
	void MoveToFollowPosition();

	float speed;
	float sensitivity;
	float DistanceFromTarget;

	GraphicsEngineParams params;

	D3DXVECTOR3 minBoundries;
	D3DXVECTOR3 maxBoundries;
	bool forceBoundries;

	bool updateCamera;
	bool activeWindowDisabling;
	
	D3DXVECTOR3 NormalizeVector(D3DXVECTOR3 vec);

public:
	Camera(HWND g_hWnd, GraphicsEngineParams params);
	virtual ~Camera();


	/* Inherited from iCamera */
	virtual Vector3 GetPosition();
	virtual void SetPosition(Vector3 pos);
	virtual void Move(Vector3 moveBy);

	virtual void MoveForward(float diff) = 0;
	virtual void MoveBackward(float diff) = 0;
	virtual void MoveLeft(float diff) = 0;
	virtual void MoveRight(float diff) = 0;
	virtual void MoveUp(float diff) = 0;
	virtual void MoveDown(float diff) = 0;

	virtual Vector3 GetForward();
	virtual void SetForward(Vector3 forward);
	virtual void LookAt(Vector3 target);
	virtual Vector3 GetUpVector();
	virtual void SetUpVector(Vector3 up);
	virtual Vector3 GetRightVector();

	virtual void Update(float delta);

	virtual void SetSpeed(float speed) { this->speed = speed; }
	virtual float GetSpeed() const { return this->speed; }
	virtual void SetSensitivity(float sens) { this->sensitivity = sens; }
	virtual float GetSensitivity() const { return this->sensitivity; }

	virtual void SetBoundries(Vector3 minBoundries, Vector3 maxBoundries);
	virtual void DisableBoundries();
	
	//virtual void WalkOnTerrain(iTerrain* terrain) { this->terrain = terrain; }
	virtual void StopWalkingOnTerrain() { this->terrain = NULL; }
	virtual void FollowMesh(iMesh* target);
	virtual void StopFollowingMesh() { this->followTarget = NULL; }
	virtual void SetDistanceFromTarget(float distance) { this->DistanceFromTarget = distance; }
	virtual float GetDistanceFromTarget() const { return this->DistanceFromTarget; }

	virtual void SetActiveWindowDisabling(bool dis) { this->activeWindowDisabling = dis; }
	virtual void SetUpdateCamera(bool update) { this->updateCamera = update; }

	virtual Vector3 Get3DPickingRay();

	/* Not inherited */
	D3DXMATRIX GetViewMatrix();
	D3DXMATRIX GetProjectionMatrix();
	void RecreateProjectionMatrix();

	D3DXVECTOR3 GetPositionD3DX() const { return this->pos; }
	D3DXVECTOR3 GetForwardD3DX() const { return this->forward; }
	void SetPosition(D3DXVECTOR3 pos) { this->pos = pos; }
	void LookAt(D3DXVECTOR3 at) { this->LookAt(Vector3(at.x, at.y, at.z)); }

	virtual void UpdateSpecific(float delta) = 0;

	bool GetActiveWindowDisabling() const { return this->activeWindowDisabling; }
	bool GetUpdatingCamera() const { return this->updateCamera; }
	virtual CameraType GetCameraType() const = 0;
};

#endif