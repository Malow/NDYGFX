#pragma once

#include "MaloW.h"
#include "Array.h"
#include <string>
#include <fstream>
#include "GraphicsEngineParameters.h"

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
	MaloW::Array<PerformanceMeasurement> perfs;

public:
	MaloWPerformance();
	virtual ~MaloWPerformance();

	void PreMeasure(string perfName);
	void PostMeasure(string perfName);
	void GenerateReport(GraphicsEngineParams gep);
};