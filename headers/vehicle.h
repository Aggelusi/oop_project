#pragma once
using namespace std;
#include "grid.h"
#include "sensors.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

class Sensor;          // ✅ forward declaration
class SensorReading;  // ✅ forward declaration
class GridWorld;

enum TurnAction {
	LEFT,
	RIGHT,
	UTURN
};

class SelfDrivingCar {
public:
	SelfDrivingCar(int x, int y, directionState dir);
	~SelfDrivingCar();

	void accelerate();
	void decelerate();
	void turn(TurnAction action);
	void move();

	void collectSensorData();
	void syncNavigationSystem();
	void executeMovement();

	void attachToWorld(GridWorld* w);
    void addSensor(Sensor* s);
    void setGPSTargets(const std::vector<Position>& targets);
    Position getPosition() const;
    directionState getDirection() const;

private:
	Position pos;
	directionState dir;
	speedState speed;

	GridWorld* world = nullptr;	
	vector<Sensor*> sensors;

	vector<SensorReading> rawReadings;
	vector<SensorReading> fusedReadings;

	vector<Position> gpsTargets;
	size_t currentTargetIndex = 0;

	double minConfidenceThreshold = 0.40;

	void fuseSensorData();
    int manhattanDistance(const Position& a, const Position& b) const;
    bool atCurrentTarget(int threshold = 5) const;
};