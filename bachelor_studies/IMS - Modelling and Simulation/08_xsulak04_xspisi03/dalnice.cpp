#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <cmath>

#include "simlib.h"

#include "data.hpp"

Histogram Tabulka("Tabulka",0,3,20);
Histogram AvgVehicleSpeed("Avg Vehicle Speed",0,10,15);
Histogram VehicleSpeed("Vehicle Speed",0,10,14);
Histogram WaitTime("Wait Time",0,30,10);

//std::vector<Histogram> CapacityInHour("CapacityInHour", 0, 1, 24);

#define KMH_TO_MS(x) (x / 3.6)
#define MS_TO_KMH(x) (x * 3.6)

#define PARTIAL_REPAIR	28800 //About 4 partial repairs dayly

//Almost 2 accidents dayly. In 1 direction 335 per year - every 22.01hours
#define ACCIDENT		79244/2

std::vector<std::array<long, 24>>* perDay;

class Vehicle;

class RoadSection : public Store {
	public:
		int lanes = 2;
		double chanceToLeave = -1.0;
		// road limitation
		double maxSpeed = KMH_TO_MS(130);
		int position;

		const double capacityPerLane = 71;
		const double optimalPerLane = 12.5;

		//using Store::Store;
		RoadSection() {
			SetCapacity(lanes * capacityPerLane);
		}

		RoadSection(int position) : position(position) {
			SetCapacity(lanes * capacityPerLane);
		}

		// not used in current model
		double OptimalCapacity() {
			return lanes * optimalPerLane;
		}

		double MaxSpeed() { return maxSpeed; }
		void SetMaxSpeed(double _maxSpeed) { maxSpeed = _maxSpeed; }

		double Position() { return position; }
		void SetPosition(double _position) { position = _position; }

		double ChanceToLeave() { return chanceToLeave; }
		void SetChanceToLeave(double _chanceToLeave) {
			chanceToLeave = _chanceToLeave;
		}

		int Lanes() { return lanes; }
		void SetLanes(int _lanes) {
			lanes = (_lanes > 0) ? _lanes : 0;
			SetCapacity(lanes * capacityPerLane);
		}
};

std::vector<RoadSection*> roadSections[2];

class Vehicle : public Process {
	const double length = 1000.0;
	double generated;

	bool direction;
	int startingAt;
	double driver;
	double maxSpeed;

	double speed;

	public:

	Vehicle(bool direction, int startingAt, double driver, double maxSpeed)
		: direction(direction), startingAt(startingAt), driver(driver), maxSpeed(maxSpeed) {};

	void Behavior() {
		generated = Time;

		double sumSpeed = 0.0;
		long distance = 0;
		// We should iterate over roadSections from startingAt
		Enter(**(roadSections[direction].begin() + startingAt), 1);
		for (auto roadSection = roadSections[direction].begin() + startingAt;
				roadSection != roadSections[direction].end(); ++roadSection) {
			auto rs = *roadSection;
			Tabulka(rs->Used());
			//std::cout << roadSection - roadSections[direction].begin() << std::endl;
			int hour = std::fmod(Time / (60*60), 24);
			
			perDay->at(rs->Position())[hour] += 1;
			
			double n = 2.0 * (double)rs->Used() / (double)rs->Capacity();
			// check for road limitations
			double rsms = std::min(rs->MaxSpeed(), maxSpeed);
			// calculate the speed
			speed = driver * rsms * std::pow( 1 - (double)rs->Used() / (double)rs->Capacity(), n);

			VehicleSpeed(MS_TO_KMH(speed));
			//std::cout << speed << "=" << maxSpeed << "*pow(1-" << rs->Used() << "/" << rs->Capacity() << "," << n << ")" << std::endl;

			++distance;
			sumSpeed += speed;
			

			WaitTime(length / speed);
			Wait(length / speed);

			// Is it possible to leave
			if (rs->ChanceToLeave() > 0.0 && Random() < rs->ChanceToLeave()) {
				Leave(*rs);
				break;
			}

			// Wait until next section is free
			if (roadSection != roadSections[direction].end() - 1)
				Enter(**(roadSection + 1), 1);

			// Leave the old section
			Leave(*rs);
		}

		AvgVehicleSpeed(MS_TO_KMH(sumSpeed / distance));
	}
};

Histogram MaxSpeedDistribution("Max Speed", 70, 7, 10);
Histogram VehiclesInHour("VehiclesInHour", 0, 1, 24);

class VehicleGenerator : public Event {
	// Position of the generator at roadSections
	bool direction;
	int position;
	double rate;

	const double chanceToTruck = 0.267;
	const double maxTruckSpeed = KMH_TO_MS(80.0);
	const double maxPersonalSpeed = KMH_TO_MS(125.0);

	void Behavior() {
		// Maximum speed of vehicle
		double maxSpeed = 0.0;
		// Driver coeficient of the speed
		double driver = Normal(1.0, .05);

		// Truck or personal car
		if (Random() < chanceToTruck) {
			maxSpeed = maxTruckSpeed;
		} else {
			maxSpeed = maxPersonalSpeed;
		}
		MaxSpeedDistribution(MS_TO_KMH(driver * maxSpeed));

		(new Vehicle(direction, position, driver, maxSpeed))->Activate();
		int hour = std::fmod(Time / (60*60), 24);
		VehiclesInHour(hour);

		Activate(Time+Exponential(rate / (did[hour].absi / (100.0 / 24.0))));
		//Activate(Time+Exponential(rate));
	}

	public:
		VehicleGenerator(bool direction, int position, double rate)
			: direction(direction), position(position), rate(rate) {
				//std::cout << direction << " " << position << " " << rate << std::endl;
			};

};

class PartialRepair : public Process
{
	void Behavior()
	{
		int position = static_cast<int>(Uniform(0, rsds.back().end) + 0.5);
		if(position >= 205)
			return;

		int length = static_cast<int>(Uniform(2, 5) + 0.5);	//Partial repairs are length from 2-5 km 
		double pRepairTime = Uniform(14400, 36000); //From 4h to 10h;
		int direction = static_cast<int>(Random() + 0.5);
 
		auto rss = roadSections[direction];

		Print("Parial repair >> sectors: %d->%d | at %.1f h | time to repair: %2.1f h\n", position,position + length, Time/3600, pRepairTime/3600);
		//std::cout << std::endl << "Parial repair length: " << length << "km, " << std::endl;
		//std::cout << std::endl << "Actual time (h): " << Time / 3600 << std::endl;
		//std::cout << "Partial repair time: " << pRepairTime << std::endl;

		if(direction == 0 || direction == 1)
		{
			for (auto rsIt = rss.begin() + position; rsIt != rss.end() && rsIt < rss.begin() + position + length; ++rsIt) {
				auto rs = *rsIt;
				rs->SetMaxSpeed(80);
				if(rs->Lanes() > 1)
					rs->SetLanes(rs->Lanes() - 1);
			}

			Wait(pRepairTime);
			for (auto rsIt = rss.begin() + position; rsIt != rss.end() && rsIt < rss.begin() + position + length; ++rsIt) {
				auto rs = *rsIt;
				rs->SetMaxSpeed(130);
				if(rs->Lanes() < 4)
					rs->SetLanes(rs->Lanes() + 1);
			}
		}
	}
};

class GeneratorPartialRepair : public Event
{
	void Behavior()
	{
		(new PartialRepair)->Activate();
		Activate(Time + Exponential(PARTIAL_REPAIR));
	}
};

class Accident : public Process
{
	void Behavior()
	{
		int position = static_cast<int>(Uniform(0, rsds.back().end-1) + 0.5 );
		if(position >= 205)
			return;
		double accidentTime = Uniform(3600, 7200); //From 1h to 2h;
		int direction = static_cast<int>(Random() + 0.5);
		
		auto rsIt = *(roadSections[direction].begin() + position);
 
		//Print("Time to remove: %d\n", accidentTime);
	//	std::cout << std::endl << "Parial repair on: " << position << "km, " << std::endl;
	//	std::cout << std::endl << "Actual time (h): " << Time / 3600 << std::endl;
	//	std::cout << "Partial repair time: " << accidentTime << std::endl;

		if(direction == 0 || direction == 1)
		{
			if(rsIt->Lanes() > 1 && rsIt->Lanes() < 4)
			{
				Print("Accident      >> sector:   %-3d   | at %.1f h | time to remove: %2.1f h\n", position, Time/3600, accidentTime/3600);
				rsIt->SetMaxSpeed(80);
				rsIt->SetLanes(rsIt->Lanes() - 1);

				Wait(accidentTime);

				rsIt->SetMaxSpeed(130);
				rsIt->SetLanes(rsIt->Lanes() + 1);
			}
		}
	}
};

class GeneratorAccident : public Event
{
	void Behavior()
	{
		(new Accident)->Activate();
		Activate(Time + Exponential(ACCIDENT));
	}
};

std::vector<VehicleGenerator*> vehicleGenerators[2];

#define TO_LEFT true
#define TO_RIGHT false

int main()
{
	//Print("Init\n");
	// Pseudo-random number generator initialization
	RandomSeed(time(NULL));

	// roadSections initialization
	const long roadLength = rsds.back().end;
	perDay = new std::vector<std::array<long, 24>>(roadLength);
	for ( auto rsdIt = rsds.begin(); rsdIt != rsds.end(); ++rsdIt) {
		auto rsd = *rsdIt;
		// Sections between exits
		unsigned long sections = rsd.end - rsd.start;

		// Init those
		for (unsigned long k = 0; k < sections; ++k) {
			int position = rsd.start + k;
			//std::cout << sections << " " << position << std::endl;
			auto rs = new RoadSection(position);
			rs->SetLanes(rsd.lanes);
			if (rsd.lanes == 1)
				rs->SetMaxSpeed(80);
			roadSections[TO_RIGHT].push_back(rs);

			rs = new RoadSection(position);
			rs->SetLanes(rsd.lanes);
			if (rsd.lanes == 1)
				rs->SetMaxSpeed(80);
			roadSections[TO_LEFT].insert(roadSections[TO_LEFT].begin(), rs);
		}

		// Difference between daily intesities
		if ( rsdIt < rsds.end() - 1) {
			double diff = (rsdIt->all - (rsdIt + 1)->all) / 2.0;
			// More in the next section?
			if (diff < 0.0) {
				// We need to add vehicles going right
				auto vg = new VehicleGenerator(TO_RIGHT, rsd.end - 1, (60 * 60 * 24) / (double)-diff);
				vehicleGenerators[TO_RIGHT].push_back(vg);
				// Remove vehicles going left
				roadSections[TO_LEFT].front()->SetChanceToLeave( -diff / ((double)rsd.all / 2));
			} else {
				// We need to add vehicles going left
				auto vg = new VehicleGenerator(TO_LEFT, roadLength - rsd.end, (60 * 60 * 24) / (double)diff);
				vehicleGenerators[TO_LEFT].push_back(vg);
				// Remove vehicles going right
				roadSections[TO_RIGHT].back()->SetChanceToLeave( diff / ((double)rsd.all / 2));
			}
		}
	}
	//std::cout << roadSections[TO_RIGHT].size() << " " << roadSections[TO_LEFT].size() << std::endl;
	//return 0;

	Print("*************** IMS - DIALNICA D1 Praha-Brno ***************\n\n");
	Init(0, 86400);

	// Activate generators
	for ( int i = 0; i <= 1; ++i )
		for ( auto &vg : vehicleGenerators[i] ) {
			vg->Activate();
		}
		
	(new GeneratorPartialRepair)->Activate();
	(new GeneratorAccident)->Activate();

	Run();

	//for ( auto &i : roadSections ) {
	//	i->Output();
	//}

	//Tabulka.Output();
	VehiclesInHour.Output();
	VehicleSpeed.Output();
	WaitTime.Output();
	//AvgVehicleSpeed.Output();
	MaxSpeedDistribution.Output();
	Print("\n------------------STATISTIC : 1KM SECTORS-------------------\n\n");
	int l = 0;
	for (auto rspdIt = perDay->begin(); rspdIt - 1 != perDay->end(); ++rspdIt) {
		auto i = *rspdIt;
		//Print("------------------Sector: %d-------------------------\n", l);
		Print("\n+----------------------------------------------------------+\n"
			"| Sector %-3d                                               |\n"
			"+----------------------------------------------------------+\n"
			"| STATISTIC:     Number of Vehicles every hour             |\n"
			"+----------------------------------------------------------+\n"
			"|    from    |     to     |           vehicles             |\n"
			"+------------+------------+----------+----------+----------+\n", l);
		for(int b = 0; b < 24; b++)
				Print("|      %-3dh  |     %-3dh   |             %-10d         |\n", b, b+1, i[b]);
		Print("+----------+--------+------------+------------+------------+\n");
		l++;
	}


	return 0;
}
