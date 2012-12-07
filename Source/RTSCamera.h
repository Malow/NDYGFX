#ifndef RTSCAMERA
#define RTSCAMERA

#include "Camera.h"

class RTSCamera : public Camera
{
public:
	RTSCamera(HWND g_hWnd, GraphicsEngineParams params);
	virtual ~RTSCamera();

	virtual void UpdateSpecific(float delta);

	virtual void MoveForward(float diff);
	virtual void MoveBackward(float diff);
	virtual void MoveLeft(float diff);
	virtual void MoveRight(float diff);

	virtual CameraType GetCameraType() const { return RTS; }
};


#endif