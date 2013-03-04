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

void MaloWPerformance::PreMeasure( string perfName, int tier )
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	float Timer = (li.QuadPart / PCFreq);

	bool found = false;
	for(int i = 0; i < this->perfs[tier].size(); i++)
	{
		if(this->perfs[tier].get(i).name == perfName)
		{
			this->perfs[tier][i].lastClock = Timer;
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
		this->perfs[tier].add(pm);
	}
}

void MaloWPerformance::PostMeasure( string perfName, int tier )
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	float Timer = (li.QuadPart / PCFreq);

	for(int i = 0; i < this->perfs[tier].size(); i++)
	{
		if(this->perfs[tier].get(i).name == perfName)
		{
			float timeDiff = Timer - this->perfs[tier][i].lastClock;
			this->perfs[tier][i].totalTime += timeDiff;
			this->perfs[tier][i].measures++;
		}
	}
}

void MaloWPerformance::GenerateReport(GraphicsEngineParams gep)
{
	fstream writeFile;
	writeFile.open ("MPR.txt", ios::out | ios::trunc);
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
	else
		writeFile << endl;

	for(int u = 0; u < NR_OF_TIERS; u++)
	{
		writeFile << "           Tier " << u + 1 << ": " << endl;
		for(int i = 0; i < this->perfs[u].size(); i++)
		{
			writeFile << this->perfs[u][i].name << ": " << endl << 
				"Avg: " << this->perfs[u][i].totalTime / this->perfs[u][i].measures << "     Tot:" <<
				this->perfs[u][i].totalTime << ", Measures: " << this->perfs[u][i].measures << endl << endl;
		}
		writeFile << endl;
	}

	writeFile.close();
}

void MaloWPerformance::ResetAll()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	float Timer = (li.QuadPart / PCFreq);

	for(int u = 0; u < NR_OF_TIERS; u++)
	{
		for(int i = 0; i < this->perfs[u].size(); i++)
		{
			this->perfs[u][i].measures = 1;
			this->perfs[u][i].totalTime = 0.0f;
			this->perfs[u][i].lastClock = Timer;
		}
	}
}

