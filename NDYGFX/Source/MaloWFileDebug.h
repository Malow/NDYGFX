#ifndef MALOWFILEDEBUG_H
#define MALOWFILEDEBUG_H

#include <string>
#include <fstream>
#include "Vector.h"
#include "MaloW.h"

static float PCFreq = 0.0f;
static bool initTime = true;

namespace MaloW
{
	inline void InitTimeForDebug()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		PCFreq = float(li.QuadPart)/1000.0f;
		QueryPerformanceCounter(&li);

		initTime = false;
	}

	inline void Debug(std::string msg)
	{
		if(initTime)
			InitTimeForDebug();

		float timer = 0;
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		timer = (li.QuadPart / PCFreq) * 0.001f;

		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);
		writeFile << timer << ": " << msg << endl;
		writeFile.close();
	}

	inline void Debug(float nr)
	{
		Debug(MaloW::convertNrToString(nr));
	}

	inline void Debug(Vector3 v)
	{
		if(initTime)
			InitTimeForDebug();
		
		float timer = 0;
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		timer = (li.QuadPart / PCFreq) * 0.001f;
		
		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);
		writeFile << timer << ": " << "Vector3 x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
		writeFile.close();
	}

	inline void ClearDebug()
	{
		InitTimeForDebug();

		float timer = 0;
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		timer = (li.QuadPart / PCFreq) * 0.001f;

		ofstream writeFile;
		writeFile.open ("MaloWDebug.txt", ios::out | ios::trunc);
		writeFile << "Starting program at timer: " << timer << endl;
		writeFile.close();
	}

	inline string GetHRESULTErrorCodeString(HRESULT hr)
	{
		switch (hr)
		{
			case E_OUTOFMEMORY:
				return "Out of memory.";
			case E_ACCESSDENIED:
				return "Access denied.";
			case E_INVALIDARG:
				return "Invalid parameter value.";
			case E_POINTER:
				return "NULL was passed incorrectly for a pointer value.";
			case E_UNEXPECTED:
				return "Unexpected condition.";
			case E_FAIL:
				return "Unspecified error.";
			default:
				return "Other error.";
		}	
	}
}
#endif