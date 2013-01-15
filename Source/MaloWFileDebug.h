#ifndef MALOWFILEDEBUG_H
#define MALOWFILEDEBUG_H

#include <string>
#include <fstream>
#include "Vector.h"
#include "MaloW.h"

static bool erase = false;
static float PCFreq;
static bool useTime = false;
static float prevTime = 0.0f;
static float totalTime = 0.0f;

namespace MaloW
{
	inline void Debug(std::string msg)
	{
		if(erase)
		{
			ofstream writeFile;
			writeFile.open ("MaloWDebug.txt", ios::out | ios::trunc);
			writeFile << "";
			writeFile.close();
			erase = false;
		}

		float timer = 0;
		if(useTime)
		{
			LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			timer = (li.QuadPart / PCFreq) * 0.001f;
			totalTime += (timer - prevTime);
			prevTime = timer;
		}

		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);

		if(useTime)
			writeFile << totalTime << ": " << msg << endl;
		else
			writeFile << msg << endl;

		writeFile.close();
	}

	inline void Debug(float nr)
	{
		Debug(MaloW::convertNrToString(nr));
	}

	inline void Debug(Vector3 v)
	{
		if(erase)
		{
			ofstream writeFile;
			writeFile.open ("MaloWDebug.txt", ios::out | ios::trunc);
			writeFile << "";
			writeFile.close();
			erase = false;
		}

		float timer = 0;
		if(useTime)
		{
			LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			timer = (li.QuadPart / PCFreq) * 0.001f;
			totalTime += (timer - prevTime);
			prevTime = timer;
		}

		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);

		if(useTime)
			writeFile << totalTime << ": " << "Vector3 x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;
		else
			writeFile << "Vector3 x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;

		writeFile.close();
	}


	inline void ClearDebug()
	{
		LARGE_INTEGER li;
		if(!QueryPerformanceFrequency(&li))
			MaloW::Debug("QueryPerformanceFrequency Failed! in MalDebug, High resolution performance counter not available?");

		PCFreq = float(li.QuadPart)/1000.0f;
		useTime = true;
		QueryPerformanceCounter(&li);
		prevTime = (li.QuadPart / PCFreq) * 0.001f;

		ofstream writeFile;
		writeFile.open ("MaloWDebug.txt", ios::out | ios::trunc);

		writeFile << "";

		writeFile.close();
		erase = false;
	}
}
#endif