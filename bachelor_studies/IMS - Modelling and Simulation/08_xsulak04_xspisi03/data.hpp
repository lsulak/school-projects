#ifndef DATA_HPP
#define DATA_HPP

#include <vector>

typedef struct {
	int start;
	int end;
	unsigned short lanes;
	const char* sectionName;
	long heavy;
	long normal;
	long motocycle;
	long all;
	const char* startName;
	const char* startEnd;
} RoadSectionData;

extern std::vector<RoadSectionData> rsds;

typedef struct {
	unsigned short hour;
	double absi;
} DayIntensityData;

extern std::vector<DayIntensityData> did;

#endif
