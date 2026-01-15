#include "../headers/vehicle.h"
#include <iostream>

SelfDrivingCar::SelfDrivingCar(int x, int y, directionState dir)
    : pos{ x, y }, dir(dir), speed(STOPPED) {}

SelfDrivingCar::~SelfDrivingCar() {
    std::cout << "SelfDrivingCar Destructed\n";
}

void SelfDrivingCar::accelerate() {
    if (speed == STOPPED) {
        speed = HALF_SPEED;
    } else if (speed == HALF_SPEED) {
        speed = FULL_SPEED;
    }
}

void SelfDrivingCar::decelerate() {
    if (speed == FULL_SPEED) {
        speed = HALF_SPEED;
    } else if (speed == HALF_SPEED) {
        speed = STOPPED;
    }
}

void SelfDrivingCar::turn(TurnAction action) {
    switch (action) {
        case RIGHT:
            switch (dir) {
                case NORTH: dir = EAST;  break;
                case EAST:  dir = SOUTH; break;
                case SOUTH: dir = WEST;  break;
                case WEST:  dir = NORTH; break;
            }
            break;

        case LEFT:
            switch (dir) {
                case NORTH: dir = WEST;  break;
                case WEST:  dir = SOUTH; break;
                case SOUTH: dir = EAST;  break;
                case EAST:  dir = NORTH; break;
            }
            break;

        case UTURN:
            switch (dir) {
                case NORTH: dir = SOUTH; break;
                case SOUTH: dir = NORTH; break;
                case EAST:  dir = WEST;  break;
                case WEST:  dir = EAST;  break;
            }
            break;
    }
}

void SelfDrivingCar::move() {
    int steps = 0;

    switch (speed) {
        case STOPPED:    steps = 0; break;
        case HALF_SPEED: steps = 1; break;
        case FULL_SPEED: steps = 2; break;
    }

    for (int i = 0; i < steps; i++) {
        switch (dir) {
            case NORTH: pos.y++; break;
            case SOUTH: pos.y--; break;
            case EAST:  pos.x++; break;
            case WEST:  pos.x--; break;
        }
    }
}

void SelfDrivingCar::collectSensorData() {}
void SelfDrivingCar::syncNavigationSystem() {}
void SelfDrivingCar::executeMovement() {
    move();
}

void SelfDrivingCar::attachToWorld(GridWorld* w) { world = w; }
void SelfDrivingCar::addSensor(Sensor* s) { sensors.push_back(s); }
void SelfDrivingCar::setGPSTargets(const std::vector<Position>& targets) { gpsTargets = targets; currentTargetIndex = 0; }
Position SelfDrivingCar::getPosition() const { return pos; }
directionState SelfDrivingCar::getDirection() const { return dir; }