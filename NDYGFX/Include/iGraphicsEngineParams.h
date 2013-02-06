#pragma once

#if defined DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

enum CameraType
{
	FPS = 0,
	RTS = 1,
	TRD = 2
};

extern "C"
{
	class DECLDIR iGraphicsEngineParams
	{	
	protected:
		iGraphicsEngineParams() {}
		virtual ~iGraphicsEngineParams() {}

	public:
		int WindowWidth;
		int WindowHeight;
		bool Maximized;
		int ShadowMapSettings;
		int FXAAQuality;
		CameraType CamType;
		float FOV;
		float NearClip;
		float FarClip;
		int RefreshRate;
		int MaxFPS;
		int ShadowFit;
		float BillboardRange;

		virtual void LoadFromFile(const char* file) = 0;
		virtual void SaveToFile(const char* file) = 0;
	};
}