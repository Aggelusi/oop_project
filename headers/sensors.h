
#pragma once

#include <optional>
#include <string>
#include <vector>
#include "types.h"
#include "vehicle.h"
#include "grid.h"
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

	optional<speedState> speed;
	optional<directionState> direction;
	optional<string> signText;
	optional<string> trafficLight;

public:
	SensorReading(
		const std::string& type,
		int distance,
		const Position& pos,
		const std::string& objectID,
		float confidence
	);

	const string& getType() const;
	int getDistance() const;
	const Position& getPosition() const;
	const string& getObjectID() const;
	float getConfidence() const;

	const optional<speedState>& getSpeed() const;
	const optional<directionState>& getDirection() const;
	const optional<string>& getSignText() const;
	const optional<string>& getTrafficLight() const;

	void setSpeed(speedState s);
	void setDirection(directionState d);
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

	virtual vector<SensorReading> sense(const GridWorld& world, const Position& vehiclePos, directionState vehicleDir) const = 0;

	// Which optional SensorReading fields this sensor can populate.
	virtual bool providesSpeed() const { return false; }
	virtual bool providesDirection() const { return false; }
	virtual bool providesSignText() const { return false; }
	virtual bool providesTrafficLight() const { return false; }

	int getRange() const;
	FOV getFOV() const;
	bool canDetectStatic() const;
	bool canDetectMoving() const;
	int getDistanceAccuracy() const;
	int getObjectAccuracy() const;
};

class LidarSensor : public Sensor {
public:
	LidarSensor();
	vector<SensorReading> sense(const GridWorld& world, const Position& vehiclePos, directionState vehicleDir) const override;

	bool providesSpeed() const override { return false; }
	bool providesDirection() const override { return false; }
	bool providesSignText() const override { return false; }
	bool providesTrafficLight() const override { return false; }
};

class RadarSensor : public Sensor {
public:
	RadarSensor();
	vector<SensorReading> sense(const GridWorld& world, const Position& vehiclePos, directionState vehicleDir) const override;

	bool providesSpeed() const override { return true; }
	bool providesDirection() const override { return true; }
	bool providesSignText() const override { return false; }
	bool providesTrafficLight() const override { return false; }
};

class CameraSensor : public Sensor {
public:
	CameraSensor();
	vector<SensorReading> sense(const GridWorld& world, const Position& vehiclePos, directionState vehicleDir) const override;

	bool providesSpeed() const override { return true; }
	bool providesDirection() const override { return true; }
	bool providesSignText() const override { return true; }
	bool providesTrafficLight() const override { return true; }
};
