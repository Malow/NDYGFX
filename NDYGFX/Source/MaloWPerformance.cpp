#include "MaloWPerformance.h"


MaloWPerformance::MaloWPerformance()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	PCFreq = float(li.QuadPart)/1000.0f;
}

MaloWPerformance::~MaloWPerformance()
{

}


void MaloWPerformance::PostMeasure( string perfName )
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	float Timer = (li.QuadPart / PCFreq) * 0.001f;

	for(int i = 0; i < this->perfs.size(); i++)
	{
		if(this->perfs.get(i).name == perfName)
		{
			float timeDiff = Timer - this->perfs[i].lastClock;
			this->perfs[i].totalTime += timeDiff;
			this->perfs[i].measures++;
		}
	}
}

void MaloWPerformance::GenerateReport()
{
	fstream writeFile;
	writeFile.open ("MaloWPerformanceReport.txt", ios::out | ios::trunc);
	
	for(int i = 0; i < this->perfs.size(); i++)
	{
		writeFile << this->perfs[i].name << ": " << endl << 
			"Avg: " << this->perfs[i].totalTime / this->perfs[i].measures << "     Tot:" <<
			this->perfs[i].totalTime << ", Measures: " << this->perfs[i].measures << endl << endl;
	}

	writeFile.close();
}

void MaloWPerformance::PreMeasure( string perfName )
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	float Timer = (li.QuadPart / PCFreq) * 0.001f;

	bool found = false;
	for(int i = 0; i < this->perfs.size(); i++)
	{
		if(this->perfs.get(i).name == perfName)
		{
			this->perfs[i].lastClock = Timer;
			found = true;
		}
	}
	if(!found)
	{
		PerformanceMeasurement pm;
		pm.name = perfName;
		pm.totalTime = 0.0f;
		pm.measures = 1;
		pm.lastClock = Timer;
		this->perfs.add(pm);
	}
}
