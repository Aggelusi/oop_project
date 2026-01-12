
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "vehicle.h"
using namespace std;

enum class FOV {
	STRAIGHT,
	SQUARE
};

class SensorReading {
private:
	string type;
	int distance;
	Position pos;
	string objectID;
	float confidence;

	optional<SpeedState> speed;
	optional<Direction> direction;
	optional<string> signText;
	optional<string> trafficLight;

public:
	SensorReading(
		const string& type,
		int distance,
		int pos_x,
		int pos_y,
		const string& objectID,
		float confidence);

	const string& getType() const;
	int getDistance() const;
	const Position& getPosition() const;
	const string& getObjectID() const;
	float getConfidence() const;

	const optional<SpeedState>& getSpeed() const;
	const optional<Direction>& getDirection() const;
	const optional<string>& getSignText() const;
	const optional<string>& getTrafficLight() const;

	void setSpeed(SpeedState s);
	void setDirection(Direction d);
	void setSignText(const string& text);
	void setTrafficLight(const string& color);
};

class Sensor {
private:
	int range;
	FOV fov;
	int distanceAccuracy;
	int objectAccuracy;
	bool detectStatic;
	bool detectMoving;
    bool signColorText;
public:
	Sensor(int range, FOV fov, int distanceAccuracy, int objectAccuracy, bool detectStatic, bool detectMoving, bool signColorText);
	virtual ~Sensor() = default;

	virtual vector<SensorReading> sense(const Position& vehiclePos, Direction vehicleDir) const = 0;

	int getRange() const;
	FOV getFOV() const;
	bool canDetectStatic() const;
	bool canDetectMoving() const;
};

class LidarSensor : public Sensor {
public:
	LidarSensor();
	vector<SensorReading> sense(const Position& vehiclePos, Direction vehicleDir) const override;
};

class RadarSensor : public Sensor {
public:
	RadarSensor();
	vector<SensorReading> sense(const Position& vehiclePos, Direction vehicleDir) const override;
};

class CameraSensor : public Sensor {
public:
	CameraSensor();
	vector<SensorReading> sense(const Position& vehiclePos, Direction vehicleDir) const override;
};
