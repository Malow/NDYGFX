#ifndef RTSCAMERA
#define RTSCAMERA

#include "Camera.h"

#pragma warning ( push )
#pragma warning ( disable : 4512 ) // warning C4512: 'RTSCamera' : assignment operator could not be generated

class RTSCamera : public Camera
{
public:
	RTSCamera(HWND g_hWnd, GraphicsEngineParams &params);
	virtual ~RTSCamera();

	virtual void UpdateSpecific(float delta);

	virtual void MoveForward(float diff);
	virtual void MoveBackward(float diff);
	virtual void MoveLeft(float diff);
	virtual void MoveRight(float diff);
	virtual void MoveUp(float diff);
	virtual void MoveDown(float diff);

	virtual CameraType GetCameraType() const { return RTS; }
};

#pragma warning ( pop )

#endif