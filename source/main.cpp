#include <iostream>
using namespace std;

void printHelp();

int main() {
    printHelp();

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