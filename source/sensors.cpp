#include "../headers/sensors.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
using namespace std;

SensorReading::SensorReading(
    const string& type,
    int distance,
    int pos_x,
    int pos_y,
    const string& objectID,
    float confidence)
    : type(type), distance(distance), pos{pos_x, pos_y}, objectID(objectID), confidence(confidence) {}

const string& SensorReading::getType() const { return type; }
int SensorReading::getDistance() const { return distance; }
const Position& SensorReading::getPosition() const { return pos; }
const string& SensorReading::getObjectID() const { return objectID; }
float SensorReading::getConfidence() const { return confidence; }

const optional<SpeedState>& SensorReading::getSpeed() const { return speed; }
const optional<Direction>& SensorReading::getDirection() const { return direction; }
const optional<string>& SensorReading::getSignText() const { return signText; }
const optional<string>& SensorReading::getTrafficLight() const { return trafficLight; }

void SensorReading::setSpeed(SpeedState s) { speed = s; }
void SensorReading::setDirection(Direction d) { direction = d; }
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

LidarSensor::LidarSensor() : Sensor(9, FOV::SQUARE, 99, 87, true, true, false) {}
vector<SensorReading> LidarSensor::sense(const Position& vehiclePos, Direction vehicleDir) const {
    return {};
}

RadarSensor::RadarSensor() : Sensor(12, FOV::STRAIGHT, 95, 95, false, true, false) {}
vector<SensorReading> RadarSensor::sense(const Position& vehiclePos, Direction vehicleDir) const {
    return {};
}

CameraSensor::CameraSensor() : Sensor(7, FOV::SQUARE, 87, 95, true, false, true) {}
vector<SensorReading> CameraSensor::sense(const Position& vehiclePos, Direction vehicleDir) const {
    return {};
}