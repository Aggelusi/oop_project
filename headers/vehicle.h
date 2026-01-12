#pragma once

enum Direction {
	NORTH,
	SOUTH,
	EAST,
	WEST
};

enum SpeedState {
	STOPPED,
	HALF_SPEED,
	FULL_SPEED
};

enum TurnAction {
	LEFT,
	RIGHT,
	UTURN
};

struct Position {
	int x;
	int y;
};

class SelfDrivingCar {
public:
	SelfDrivingCar(int x, int y, Direction dir);
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
	Direction dir;
	SpeedState speed;
};

