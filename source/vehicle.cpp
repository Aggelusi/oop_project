#include "../headers/vehicle.h"
#include <iostream>

// Initialize car position, direction, and speed.
SelfDrivingCar::SelfDrivingCar(int x, int y, directionState dir)
    : pos{ x, y }, dir(dir), speed(STOPPED) {}

SelfDrivingCar::~SelfDrivingCar() {
    for (auto* s : sensors) delete s;
    sensors.clear();
    std::cout << "SelfDrivingCar Destructed\n";
}

// Increase speed one level.
void SelfDrivingCar::accelerate() {
    if (speed == STOPPED) {
        speed = HALF_SPEED;
    } else if (speed == HALF_SPEED) {
        speed = FULL_SPEED;
    }
}

// Decrease speed one level.
void SelfDrivingCar::decelerate() {
    if (speed == FULL_SPEED) {
        speed = HALF_SPEED;
    } else if (speed == HALF_SPEED) {
        speed = STOPPED;
    }
}

// Apply a turn action to update direction.
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

// Move forward based on current direction and speed.
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

// Query all sensors and append their readings.
void SelfDrivingCar::collectSensorData() {
    rawReadings.clear();
    if (!world) return;

    for (const auto& s : sensors) {
        if (!s) continue;
        vector<SensorReading> sensorOut = s->sense(*world, pos, dir);
        rawReadings.insert(rawReadings.end(), sensorOut.begin(), sensorOut.end());
    }
}
// Fuse raw readings into a cleaned set for navigation logic.
void SelfDrivingCar::syncNavigationSystem() {
    fusedReadings.clear();
    if (rawReadings.empty()) return;
    fuseSensorData();
}

// Combine multiple readings per object into a single fused reading.
void SelfDrivingCar::fuseSensorData() {
    vector<string> processedIDs;
    for (size_t i = 0; i < rawReadings.size(); i++) {
        const SensorReading& r = rawReadings[i];
        if (find(processedIDs.begin(), processedIDs.end(), r.getObjectID()) != processedIDs.end())
            continue;

        processedIDs.push_back(r.getObjectID());

        double weightedDist = 0.0;
        double confSum = 0.0;
        float maxConf = 0.0f;
        const SensorReading* best = nullptr;

        for (size_t j = 0; j < rawReadings.size(); j++) {
            if (rawReadings[j].getObjectID() == r.getObjectID()) {
                float c = rawReadings[j].getConfidence();
                weightedDist += c * rawReadings[j].getDistance();
                confSum += c;

                if (c > maxConf) {
                    maxConf = c;
                    best = &rawReadings[j];
                }
            }
        }

        if (!best || confSum == 0.0) continue;

        int fusedDistance = static_cast<int>(weightedDist / confSum);

        SensorReading fused(
            best->getType(),
            fusedDistance,
            best->getPosition(),
            best->getObjectID(),
            maxConf
        );

        if (best->getSpeed().has_value())        fused.setSpeed(*best->getSpeed());
        if (best->getDirection().has_value())    fused.setDirection(*best->getDirection());
        if (best->getSignText().has_value())     fused.setSignText(*best->getSignText());
        if (best->getTrafficLight().has_value()) fused.setTrafficLight(*best->getTrafficLight());

        if (fused.getConfidence() >= minConfidenceThreshold ||
            fused.getType() == "BIKE") {
            fusedReadings.push_back(fused);
        }
    }
}

// Compute Manhattan distance between two positions.
int SelfDrivingCar::manhattanDistance(const Position& a, const Position& b) const {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Check if car is within threshold of current GPS target.
bool SelfDrivingCar::atCurrentTarget(int threshold) const {
    if (gpsTargets.empty() || currentTargetIndex >= gpsTargets.size()) return false;
    return manhattanDistance(pos, gpsTargets[currentTargetIndex]) <= threshold;
}

// Navigation loop: evaluate hazards, steer to target, and move.
void SelfDrivingCar::executeMovement() {
    if (gpsTargets.empty()) {
        move();
        return;
    }

    if (currentTargetIndex >= gpsTargets.size()) {
        speed = STOPPED;
        return;
    }

    const int arrivalThreshold = 0;
    if (atCurrentTarget(arrivalThreshold)) {
        std::cout << "[NAV] Arrived target (" << gpsTargets[currentTargetIndex].x << "," << gpsTargets[currentTargetIndex].y << ")\n";
        if (currentTargetIndex + 1 < gpsTargets.size()) {
            currentTargetIndex++;
        } else {
            speed = STOPPED;
            currentTargetIndex = gpsTargets.size();
            std::cout << "[NAV] Final target reached.\n";
            return;
        }
    }

    bool hazardClose = false;
    bool needToSlowForLight = false;

    for (const auto& fr : fusedReadings) {
        int d = fr.getDistance();
        std::string type = fr.getType();
        if ((type == "MOVING_CAR" || type == "CAR" || type == "BIKE" || type == "MOVING_BIKE") && d <= 2) {
            hazardClose = true;
        }
        if (fr.getTrafficLight().has_value()) {
            std::string color = *fr.getTrafficLight();
            if ((color == "RED" || color == "YELLOW") && d <= 3) {
                needToSlowForLight = true;
            }
        }
    }

    Position target = gpsTargets[currentTargetIndex];
    directionState desired = dir;
    if (pos.y < target.y) {
        desired = NORTH;
    } else if (pos.y > target.y) {
        desired = SOUTH;
    } else if (pos.x < target.x) {
        desired = EAST;
    } else if (pos.x > target.x) {
        desired = WEST;
    }

    dir = desired;

    int dx = abs(pos.x - target.x);
    int dy = abs(pos.y - target.y);
    int axisDist = (dy != 0) ? dy : dx;

    if (hazardClose || needToSlowForLight) {
        decelerate();
    } else {
        if (axisDist > 0 && !atCurrentTarget(arrivalThreshold)) {
            accelerate();
        }
    }

    if (axisDist == 0) {
        speed = STOPPED;
    } else if (axisDist == 1 && speed == FULL_SPEED) {
        speed = HALF_SPEED;
    }

    move();
}

// Attach car to a world so sensors can query it.
void SelfDrivingCar::attachToWorld(GridWorld* w) { world = w; }

// Add a sensor to the car.
void SelfDrivingCar::addSensor(Sensor* s) { sensors.push_back(s); }

// Set the GPS targets and reset progress.
void SelfDrivingCar::setGPSTargets(const std::vector<Position>& targets) { gpsTargets = targets; currentTargetIndex = 0; }

// Configure minimum confidence for fused readings.
void SelfDrivingCar::setMinConfidenceThreshold(double threshold) { minConfidenceThreshold = threshold; }

Position SelfDrivingCar::getPosition() const { return pos; }
directionState SelfDrivingCar::getDirection() const { return dir; }

// Returns true when all targets are reached.
bool SelfDrivingCar::isNavigationComplete() const {
    if (gpsTargets.empty()) return false;
    return currentTargetIndex >= gpsTargets.size();
}