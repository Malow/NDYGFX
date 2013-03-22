#ifndef CAMERA_H
#define CAMERA_H

#include "Terrain.h"
#include "GraphicsEngineParameters.h"
#include "Mesh.h"
#include "FBXMesh.h"
#include "CursorControl.h"
#include "iCamera.h"
#include "MaloW.h"
#include "MaloWFileDebug.h"

#pragma warning ( push )
#pragma warning ( disable : 4512 ) // warning C4512: 'Camera' : assignment operator could not be generated

class Camera : public iCamera
{
protected:
	D3DXMATRIX view;
	D3DXMATRIX projection;
	D3DXMATRIX viewProj;

	D3DXVECTOR3 pos;
	D3DXVECTOR3 oldpos;

	D3DXVECTOR3 forward;
	D3DXVECTOR3 up;
	Terrain* terrain;
	Mesh* followTarget;
	
	HWND g_hWnd;

	float angleX;
	float angleY;

	void MoveToTerrain();
	void MoveFollowingMesh();

	float speed;
	Vector3 distanceFromMesh;
	Vector3 defaultMeshDirection;
	string bone;
	bool useFBXPosition;
	D3DXVECTOR3 FBXPosition;

	GraphicsEngineParams &params;

	D3DXVECTOR3 minBoundries;
	D3DXVECTOR3 maxBoundries;
	bool forceBoundries;

	bool updateCamera;
	bool activeWindowDisabling;
	bool moveOnlyInXZ;

	inline D3DXVECTOR3& NormalizeVector(D3DXVECTOR3& vec);

public:
	Camera(HWND g_hWnd, GraphicsEngineParams &params);
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

	virtual Vector3 GetForward() const;
	virtual void SetForward(Vector3 forward);
	virtual void LookAt(Vector3 target);
	virtual Vector3 GetUpVector() const;
	virtual void SetUpVector(Vector3 up);
	virtual Vector3 GetRightVector() const;

	virtual void Update(float delta);

	virtual void SetSpeed(float speed) { this->speed = speed; }
	virtual float GetSpeed() const { return this->speed; }
	virtual void SetSensitivity(float sens) { this->params.MouseSensativity = sens; }
	virtual float GetSensitivity() const { return this->params.MouseSensativity; }

	virtual void SetBoundries(Vector3 minBoundries, Vector3 maxBoundries);
	virtual void DisableBoundries();
	
	//virtual void WalkOnTerrain(iTerrain* terrain) { this->terrain = terrain; }
	virtual void StopWalkingOnTerrain() { this->terrain = NULL; }
	virtual void SetMesh(iMesh* target, Vector3 distanceFromCamera, Vector3 defaultMeshDirection = Vector3(0, 0, -1));
	virtual void SetMesh(iMesh* target, const char* bone, Vector3 defaultMeshDirection = Vector3(0, 0, -1));
	virtual void RemoveMesh() { this->followTarget = NULL; }

	virtual void SetActiveWindowDisabling(bool dis) { this->activeWindowDisabling = dis; }
	virtual void SetUpdateCamera(bool update);

	virtual Vector3 Get3DPickingRay();
	virtual void MoveOnlyInXZ(bool DoItOrNotThatIsTheQuestion);

	/* Not inherited */
	D3DXMATRIX& GetViewMatrix();
	D3DXMATRIX& GetProjectionMatrix();
	D3DXMATRIX& GetViewProjMatrix();
	void RecreateProjectionMatrix();

	D3DXVECTOR3 GetPositionD3DX() const { return this->pos; }
	D3DXVECTOR3 GetForwardD3DX() const { return this->forward; }
	D3DXVECTOR3 GetRightVectorD3DX() const;
	D3DXVECTOR3 GetUpVectorD3DX() const { return this->up; }
	D3DXVECTOR3 GetOldPos() const;
	void SetPosition(D3DXVECTOR3 pos) { this->pos = pos; }
	void LookAt(D3DXVECTOR3 at) { this->LookAt(Vector3(at.x, at.y, at.z)); }

	virtual void UpdateSpecific(float delta) = 0;

	bool GetActiveWindowDisabling() const { return this->activeWindowDisabling; }
	bool GetUpdatingCamera() const { return this->updateCamera; }
	virtual CameraType GetCameraType() const = 0;
};

#pragma warning ( pop )

#endif