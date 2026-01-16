#pragma once

#include "grid.h"

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

private:
	Position pos;
	directionState dir;
	speedState speed;
};