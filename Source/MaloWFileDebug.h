#ifndef MALOWFILEDEBUG_H
#define MALOWFILEDEBUG_H

#include <string>
#include <fstream>
#include "Vector.h"
#include "MaloW.h"

static bool erase = false;

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
		

		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);

		writeFile << msg << endl;

		writeFile.close();
	}

	inline void Debug(float nr)
	{
		if(erase)
		{
			ofstream writeFile;
			writeFile.open ("MaloWDebug.txt", ios::out | ios::trunc);

			writeFile << "";

			writeFile.close();
			erase = false;
		}
		

		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);

		writeFile << MaloW::convertNrToString(nr) << endl;

		writeFile.close();
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


		fstream writeFile;
		writeFile.open ("MaloWDebug.txt", fstream::in | fstream::out | fstream::app);

		writeFile << "Vector3 x: " << v.x << ", y: " << v.y << ", z: " << v.z << endl;

		writeFile.close();
	}


	inline void ClearDebug()
	{
		//if(erase)
		//{
			ofstream writeFile;
			writeFile.open ("MaloWDebug.txt", ios::out | ios::trunc);

			writeFile << "";

			writeFile.close();
			erase = false;
		//}
	}
}
#endif