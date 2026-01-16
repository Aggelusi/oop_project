#pragma once

struct Position {
    int x;
    int y;
};

enum directionState {
    NORTH, SOUTH, EAST, WEST
};

enum speedState {
    STOPPED, HALF_SPEED, FULL_SPEED
};

enum TrafficLightState {
    RED, GREEN, YELLOW
};
