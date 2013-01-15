#ifndef MALOWFILEDEBUG_H
#define MALOWFILEDEBUG_H

#include <string>
#include <fstream>
#include "Vector.h"
#include "MaloW.h"

static float PCFreq;
static bool initTime = true;
static float prevTime = 0.0f;
static float totalTime = 0.0f;

namespace MaloW
{
	inline void InitTimeForDebug()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		PCFreq = float(li.QuadPart)/1000.0f;
		QueryPerformanceCounter(&li);
		prevTime = (li.QuadPart / PCFreq) * 0.001f;

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
		totalTime += (timer - prevTime);
		prevTime = timer;

		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);
		writeFile << totalTime << ": " << msg << endl;
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
		totalTime += (timer - prevTime);
		prevTime = timer;
		
		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);
		writeFile << totalTime << ": " << "Vector3 x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
		writeFile.close();
	}

	inline void ClearDebug()
	{
		ofstream writeFile;
		writeFile.open ("MaloWDebug.txt", ios::out | ios::trunc);
		writeFile << "";
		writeFile.close();
	}
}
#endif