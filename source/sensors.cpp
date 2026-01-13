#include "../headers/sensors.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
using namespace std;

SensorReading::SensorReading(
    const std::string& type,
    int distance,
    const Position& pos,
    const std::string& objectID,
    float confidence
) : type(type), distance(distance), pos(pos), objectID(objectID), confidence(confidence) {}


const string& SensorReading::getType() const { return type; }
int SensorReading::getDistance() const { return distance; }
const Position& SensorReading::getPosition() const { return pos; }
const string& SensorReading::getObjectID() const { return objectID; }
float SensorReading::getConfidence() const { return confidence; }

const optional<speedState>& SensorReading::getSpeed() const { return speed; }
const optional<directionState>& SensorReading::getDirection() const { return direction; }
const optional<string>& SensorReading::getSignText() const { return signText; }
const optional<string>& SensorReading::getTrafficLight() const { return trafficLight; }

void SensorReading::setSpeed(speedState s) { speed = s; }
void SensorReading::setDirection(directionState d) { direction = d; }
void SensorReading::setSignText(const string& text) { signText = text; }
void SensorReading::setTrafficLight(const string& color) { trafficLight = color; }


Sensor::Sensor(int range, FOV fov, int distanceAccuracy, int objectAccuracy, bool detectStatic, bool detectMoving, bool signColorText)
    : range(range),
      fov(fov),
      distanceAccuracy(distanceAccuracy),
      objectAccuracy(objectAccuracy),
      detectStatic(detectStatic),
    detectMoving(detectMoving),
    signColorText(signColorText) {}

int Sensor::getRange() const { return range; }
FOV Sensor::getFOV() const { return fov; }
bool Sensor::canDetectStatic() const { return detectStatic; }
bool Sensor::canDetectMoving() const { return detectMoving; }
int Sensor::getDistanceAccuracy() const { return distanceAccuracy; }
int Sensor::getObjectAccuracy() const { return objectAccuracy; }

LidarSensor::LidarSensor() : Sensor(9, FOV::SQUARE, 99, 87, true, true, false) {}
vector<SensorReading> LidarSensor::sense(const GridWorld& world, const Position& vehiclePos, directionState vehicleDir) const {
    vector<SensorReading> readings;

    int dimX = world.getDimX();
    int dimY = world.getDimY();
    int halfRange = getRange() / 2;

    int startX = max(0, vehiclePos.x - halfRange);
    int endX   = min(dimX - 1, vehiclePos.x + halfRange);
    int startY = max(0, vehiclePos.y - halfRange);
    int endY   = min(dimY - 1, vehiclePos.y + halfRange);

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            Object* obj = world.getv(x, y);
            if (!obj) continue;

            Object* current = obj;
            while (current) {
                char g = current->getGlyph();
                bool valid =
                    g == 'C' || g == 'B' || // moving objects
                    g == 'P' || g == 'S' || g == 'R' || g == 'G' || g == 'Y'; // static objects / lights / signs

                if (valid) {
                    int distance = abs(x - vehiclePos.x) + abs(y - vehiclePos.y);
                    float confidence = getObjectAccuracy() / 100.0f;
                    confidence *= 1.0f - (float)distance / (float)getRange();
                    if (confidence < 0.1f) confidence = 0.1f;
                    
                    float noise = ((rand() % 101) / 100.0f - 0.5f) * 0.1f;
                    confidence += noise;

                    if (confidence < 0.0f) confidence = 0.0f;
                    if (confidence > 1.0f) confidence = 1.0f;

                    SensorReading r(
                        string(1, g),
                        distance,
                        Position{x, y},
                        current->getID(),
                        confidence
                    );

                    readings.push_back(r);
                }

                current = current->getNext();
            }
        }
    }
    return readings;
}


RadarSensor::RadarSensor() : Sensor(12, FOV::STRAIGHT, 95, 95, false, true, false) {}
vector<SensorReading> RadarSensor::sense(const GridWorld& world, const Position& vehiclePos, directionState vehicleDir) const {
    vector<SensorReading> readings;
    int range = getRange();

    for (int i =1; i <= range; ++i) {
        int x = vehiclePos.x;
        int y = vehiclePos.y;

        switch(vehicleDir) {
            case NORTH: y = vehiclePos.y - i; break;
            case SOUTH: y = vehiclePos.y + i; break;
            case EAST:  x = vehiclePos.x + i; break;
            case WEST:  x = vehiclePos.x - i; break;
        }

        if (x < 0 || x >= world.getDimX() || y < 0 || y >= world.getDimY()) break;
        Object* obj = world.getv(x,y);
        if (!obj) continue;

        Object* current = obj;
        while (current) {
            char g = current->getGlyph();
            if (g == 'C' || g == 'B') {
                int distance = i;
                float confidence = getObjectAccuracy() / 100.0f;
                confidence *= 1.0f - (float)distance / (float)range;
                if (confidence < 0.1f) confidence = 0.1f;
                float noise = ((rand() % 101) / 100.0f - 0.5f) * 0.1f;
                confidence += noise;
                if (confidence < 0.0f) confidence = 0.0f;
                if (confidence > 1.0f) confidence = 1.0f;

                SensorReading r(
                    string(1, g),
                    distance,
                    Position{x, y},
                    current->getID(),
                    confidence
                );
                MovingObjects* mo = dynamic_cast<MovingObjects*>(current);
                if (mo) {
                    r.setSpeed(static_cast<speedState>(mo->getSpeed()));
                    r.setDirection(static_cast<directionState>(mo->getDirection()));
                }

                readings.push_back(r);

            }
            current = current->getNext();
        }
    }
    return readings;
}

CameraSensor::CameraSensor() : Sensor(7, FOV::SQUARE, 87, 95, true, false, true) {}
vector<SensorReading> CameraSensor::sense(const GridWorld& world, const Position& vehiclePos, directionState vehicleDir) const {
    vector<SensorReading> readings;
    int range = getRange();
    int startX, endX, startY, endY;

    switch(vehicleDir) {
        case NORTH:
            startX = max(0, vehiclePos.x - 3);
            endX   = min(world.getDimX() - 1, vehiclePos.x + 3);
            startY = max(0, vehiclePos.y - range);
            endY   = vehiclePos.y - 1;
            break;
        case SOUTH:
            startX = max(0, vehiclePos.x - 3);
            endX   = min(world.getDimX() - 1, vehiclePos.x + 3);
            startY = vehiclePos.y + 1;
            endY   = min(world.getDimY() - 1, vehiclePos.y + range);
            break;
        case EAST:
            startX = vehiclePos.x + 1;
            endX   = min(world.getDimX() - 1, vehiclePos.x + range);
            startY = max(0, vehiclePos.y - 3);
            endY   = min(world.getDimY() - 1, vehiclePos.y + 3);
            break;
        case WEST:
            startX = max(0, vehiclePos.x - range);
            endX   = vehiclePos.x - 1;
            startY = max(0, vehiclePos.y - 3);
            endY   = min(world.getDimY() - 1, vehiclePos.y + 3);
            break;
    }

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            Object* obj = world.getv(x, y);
            if (!obj) continue;

            Object* current = obj;
            while (current) {
                char g = current->getGlyph();
                bool valid =
                    g == 'C' || g == 'B' ||
                    g == 'P' || g == 'S' || g == 'R' || g == 'G' || g == 'Y';

                if (valid) {
                    int distance = abs(x - vehiclePos.x) + abs(y - vehiclePos.y);

                    float confidence = getObjectAccuracy() / 100.0f;
                    confidence *= 1.0f - (float)distance / (float)range;
                    if (confidence < 0.1f) confidence = 0.1f;

                    float noise = ((rand() % 101)/100.0f - 0.5f) * 0.1f;
                    confidence += noise;
                    if (confidence < 0.0f) confidence = 0.0f;
                    if (confidence > 1.0f) confidence = 1.0f;

                    SensorReading r(
                        string(1, g),
                        distance,
                        Position{x, y},
                        current->getID(),
                        confidence
                    );

                    MovingObjects* mo = dynamic_cast<MovingObjects*>(current);
                    if (mo) {
                        r.setSpeed(static_cast<speedState>(mo->getSpeed()));
                        r.setDirection(static_cast<directionState>(mo->getDirection()));
                    }
                    if (providesSignText() && (g == 'S')) r.setSignText("STOP");
                    if (providesTrafficLight() && (g == 'R' || g == 'G' || g == 'Y')) r.setTrafficLight(string(1, g));

                    readings.push_back(r);
                }

                current = current->getNext();
            }
        }
    }

    return readings;
}