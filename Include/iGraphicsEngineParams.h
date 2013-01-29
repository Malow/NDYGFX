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
	public:
		iGraphicsEngineParams() {};
		virtual ~iGraphicsEngineParams() {};
		
		static int windowWidth;
		static int windowHeight;
		static bool Maximized;
		static int ShadowMapSettings;
		static int FXAAQuality;
		static CameraType CamType;
		static float FOV;
		static float NearClip;
		static float FarClip;
		static int RefreshRate;
		static int MaxFPS;
		static int ShadowFit;

		virtual void SaveToFile(const char* file) = 0;
	};
}