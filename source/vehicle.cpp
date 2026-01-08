#include <iostream>

typedef enum {
    NORTH, SOUTH, EAST, WEST
} directionState;

typedef enum {
    STOPPED, HALF_SPEED, FULL_SPEED
} speedState;

typedef enum {
    LEFT, RIGHT, UTURN
} turnAction;

class SelfDrivingCar {
public:
    SelfDrivingCar(int pos_x, int pos_y, direction dir);
    ~SelfDrivingCar();
    
    void accelerate();
    void decelerate();
    void turn(turnAction action);
    void move();

    void collectSensorData();
    void syncNavigationSystem();
    void executeMovement();
private:
    int pos_x, pos_y;
    direction dir;
    speedState speed;
};

SelfDrivingCar::SelfDrivingCar(int pos_x, int pos_y, direction dir) {
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->dir   = dir;
    this->speed = STOPPED;
}

SelfDrivingCar::~SelfDrivingCar() {
    cout << "SelfDrivingCar Destructed\n";
}

SelfDrivingCar::accelerate() {
    if (speed == STOPPED)
        speed = HALF_SPEED;
    else if (speed == HALF_SPEED)
        speed = FULL_SPEED;
}

SelfDrivingCar::decelerate() {
    if (speed == FULL_SPEED)
        speed = HALF_SPEED
    else if (speed == HALF_SPEED)
        speed = STOPPED
}

SelfDrivingCar::turn(turnAction action) {
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

SelfDrivingCar::move() {
    int steps, i;

    switch (speed) {
        case STOPPED:     steps = 0; break;
        case HALF_SPEED:  steps = 1; break;
        case FULL_SPEED:  steps = 2; break;
    }

    for (i = 0; i < steps; i++) {
        switch (dir) {
            case NORTH: pos_y++; break;
            case SOUTH: pos_y--; break;
            case EAST:  pos_x++; break;
            case WEST:  pos_x--; break;
        }
    }
}