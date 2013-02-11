#ifndef FPSCAMERA
#define FPSCAMERA

#include "Camera.h"

#pragma warning ( push )
#pragma warning ( disable : 4512 ) // warning C4512: 'FPSCamera' : assignment operator could not be generated


class FPSCamera : public Camera
{
public:
	FPSCamera(HWND g_hWnd, GraphicsEngineParams &params);
	virtual ~FPSCamera();

	virtual void UpdateSpecific(float delta);

	virtual void MoveForward(float diff);
	virtual void MoveBackward(float diff);
	virtual void MoveLeft(float diff);
	virtual void MoveRight(float diff);
	virtual void MoveUp(float diff);
	virtual void MoveDown(float diff);

	virtual CameraType GetCameraType() const { return FPS; }
};

#pragma warning ( pop )

#endif