#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "../headers/grid.h"
#include "../headers/vehicle.h"
#include "../headers/sensors.h"

using namespace std;

// Prints CLI usage information.
void printHelp();

// Entry point: parses CLI args, sets up world and car, runs simulation loop.
int main(int argc, char** argv) {
    int seed = static_cast<int>(time(nullptr));
    int dimX = 40;
    int dimY = 40;
    int numMovingCars = 3;
    int numMovingBikes = 4;
    int numParkedCars = 5;
    int numStopSigns = 2;
    int numTrafficLights = 2;
    int simulationTicks = 100;
    double minConfidenceThreshold = 0.40;
    vector<Position> gpsTargets;
    bool gpsProvided = false;

    int i = 1;
    // Parse CLI arguments.
    while (i < argc) {
        string arg = argv[i];
        if (arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "--seed") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --seed\n";
                return 1;
            }
            seed = stoi(argv[++i]);
        } else if (arg == "--dimX") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --dimX\n";
                return 1;
            }
            dimX = stoi(argv[++i]);
        } else if (arg == "--dimY") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --dimY\n";
                return 1;
            }
            dimY = stoi(argv[++i]);
        } else if (arg == "--numMovingCars") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --numMovingCars\n";
                return 1;
            }
            numMovingCars = stoi(argv[++i]);
        } else if (arg == "--numMovingBikes") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --numMovingBikes\n";
                return 1;
            }
            numMovingBikes = stoi(argv[++i]);
        } else if (arg == "--numParkedCars") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --numParkedCars\n";
                return 1;
            }
            numParkedCars = stoi(argv[++i]);
        } else if (arg == "--numStopSigns") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --numStopSigns\n";
                return 1;
            }
            numStopSigns = stoi(argv[++i]);
        } else if (arg == "--numTrafficLights") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --numTrafficLights\n";
                return 1;
            }
            numTrafficLights = stoi(argv[++i]);
        } else if (arg == "--simulationTicks") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --simulationTicks\n";
                return 1;
            }
            simulationTicks = stoi(argv[++i]);
        } else if (arg == "--minConfidenceThreshold") {
            if (i + 1 >= argc) {
                cerr << "Missing value for --minConfidenceThreshold\n";
                return 1;
            }
            double raw = stod(argv[++i]);
            minConfidenceThreshold = (raw > 1.0) ? (raw / 100.0) : raw;
            if (minConfidenceThreshold < 0.0) minConfidenceThreshold = 0.0;
            if (minConfidenceThreshold > 1.0) minConfidenceThreshold = 1.0;
        } else if (arg == "--gps") {
            i++;
            vector<int> coords;
            while (i < argc) {
                string val = argv[i];
                if (val.rfind("--", 0) == 0) break;
                coords.push_back(stoi(val));
                i++;
            }
            if (coords.empty() || coords.size() % 2 != 0) {
                cerr << "--gps requires pairs of coordinates\n";
                return 1;
            }
            gpsTargets.clear();
            for (size_t k = 0; k < coords.size(); k += 2) {
                int gx = coords[k];
                int gy = coords[k + 1];
                if (gx < 0 || gy < 0 || gx >= dimX || gy >= dimY) {
                    cerr << "GPS target out of bounds: (" << gx << ", " << gy << ")\n";
                    cerr << "World bounds are x:[0," << (dimX - 1) << "] y:[0," << (dimY - 1) << "]\n";
                    return 1;
                }
                gpsTargets.push_back(Position{gx, gy});
            }
            gpsProvided = true;
            continue;
        } else {
            cerr << "Unknown argument: " << arg << "\n";
            printHelp();
            return 1;
        }
        i++;
    }

    if (!gpsProvided) {
        cerr << "Missing required --gps argument\n";
        printHelp();
        return 1;
    }

    // Create world
    GridWorld world(dimX, dimY, seed, numMovingBikes, numMovingCars, numParkedCars, numStopSigns, numTrafficLights);
    world.generateWorld();
    
    // Create car
    SelfDrivingCar car(5, 5, NORTH);
    car.attachToWorld(&world);

    // Add sensors
    car.addSensor(new LidarSensor());
    car.addSensor(new RadarSensor());
    car.addSensor(new CameraSensor());

    car.setGPSTargets(gpsTargets);
    car.setMinConfidenceThreshold(minConfidenceThreshold);

    cout << "Starting simulation...\n";

    // Main simulation loop.
    for (int tick = 0; tick < simulationTicks; tick++) {
        cout << "\nTick " << tick << ":\n";

        Position p = car.getPosition();
        cout << "Car position: (" << p.x << ", " << p.y << ")\n";

        // Update world and car state.
        world.update();
        car.collectSensorData();
        car.syncNavigationSystem();
        car.executeMovement();

        if (car.isNavigationComplete()) {
            cout << "\nSimulation finished.\n";
            return 0;
        }
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