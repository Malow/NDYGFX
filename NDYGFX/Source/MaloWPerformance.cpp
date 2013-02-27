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
	float Timer = (li.QuadPart / PCFreq);

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

void MaloWPerformance::GenerateReport(GraphicsEngineParams gep)
{
	fstream writeFile;
	writeFile.open ("MaloWPerformanceReport.txt", ios::out | ios::trunc);
	writeFile << "Performance report, times in milliseconds, IE 100 in the file = 0.1 seconds." << endl << endl;
	writeFile << "Settings: " << endl;
	writeFile << "Resolution: " << gep.WindowWidth << " x " << gep.WindowHeight << endl;
	writeFile << "Shadow Quality: " << gep.ShadowMapSettings << endl;
	writeFile << "FXAA Quality: " << gep.FXAAQuality << endl;
	writeFile << "Field of View: " << gep.FOV << endl;
	writeFile << "FarClip: " << gep.FarClip << endl;
	writeFile << "MaxFPS: " << gep.MaxFPS << endl;
	writeFile << "ShadowFit: " << gep.ShadowFit << endl;
	writeFile << "Billboard Range: " << gep.BillboardRange << endl; 
	writeFile << endl;

	if(gep.MaxFPS > 0)
		writeFile << "WARNING, MAXFPS IS SET!" << endl << endl;

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
	float Timer = (li.QuadPart / PCFreq);

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
