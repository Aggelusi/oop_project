#include <iostream>
#include <vector>
#include "../headers/grid.h"
#include "../headers/vehicle.h"
#include "../headers/sensors.h"

using namespace std;

int main() {
    // Create world
    GridWorld world(40, 40);

    // Create car
    SelfDrivingCar car(5, 5, NORTH);
    car.attachToWorld(&world);

    // Add sensors
    car.addSensor(new LidarSensor());
    car.addSensor(new RadarSensor());
    car.addSensor(new CameraSensor());

    // GPS targets
    vector<Position> gpsTargets = { {10, 10}, {20, 5} };
    car.setGPSTargets(gpsTargets);

    cout << "Starting simulation...\n";

    for (int tick = 0; tick < 20; tick++) {
        cout << "\nTick " << tick << ":\n";

        world.update();
        car.collectSensorData();
        car.syncNavigationSystem();
        car.executeMovement();

        Position p = car.getPosition();
        cout << "Car position: (" << p.x << ", " << p.y << ")\n";
    }

    cout << "\nSimulation finished.\n";
    return 0;
}


void printHelp() {
    cout << "Self-Driving Car Simulation\n"
         << "   Usage:\n"
         << "      --seed <n>                   | Random seed (default: current time)\n"
         << "      --dimX <n>                   | World width (default: 40)\n"
         << "      --dimY <n>                   | World height (default: 40)\n"
         << "      --numMovingCars <n>          | Number of moving cars (default: 3)\n"
         << "      --numMovingBikes <n>         | Number of moving bikes (default: 4)\n"
         << "      --numParkedCars <n>          | Number of parked cars (default: 5)\n"
         << "      --numStopSigns <n>           | Number of stop signs (default: 2)\n"
         << "      --numTrafficLights <n>       | Number of traffic lights (default: 2)\n"
         << "      --simulationTicks <n>        | Maximum simulation ticks (default: 100)\n"
         << "      --minConfidenceThreshold <n> | Minimum confidence cutofff (default: 40)\n"
         << "      --gps <x1> <y1> [x2 y2 ...]  | GPS target coordinates (required)\n"
         << "      --help                       | Show this help message\n\n"
         << "   Example Usage:\n"
         << "   ./oopproj_2025 --seed 12 --dimY 50 --gps 10 20 32 15" << endl;
}