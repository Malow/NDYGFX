#pragma once

#include "MaloW.h"
#include "Array.h"
#include <string>
#include <fstream>
#include "GraphicsEngineParameters.h"

#define NR_OF_TIERS 6

struct PerformanceMeasurement
{
	string name;
	float lastClock;
	float totalTime;
	int measures;

	PerformanceMeasurement()
	{
		lastClock = 0.0f;
		name = "";
		totalTime = 0.0f;
		measures = 0;
	}
};

class MaloWPerformance
{
private:
	float PCFreq;
	MaloW::Array<PerformanceMeasurement> perfs[NR_OF_TIERS];

public:
	MaloWPerformance();
	virtual ~MaloWPerformance();

	void PreMeasure(string perfName, int tier);
	void PostMeasure(string perfName, int tier);
	void GenerateReport(GraphicsEngineParams gep);
	void ResetAll();
};