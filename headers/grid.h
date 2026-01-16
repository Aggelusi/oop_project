#pragma once

#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

using namespace std;

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

enum TrafficLightState { RED, GREEN, YELLOW };

class GridWorld;

class Object {
protected:
    Position pos;
    string id;
    char glyph;
    Object* next; // Pointer to next object in linked list
    Object* prev; // Pointer to previous object
    int priority;

public:
    Object(int x = 0, int y = 0, string id = "def", char glyph = 'X', int priority = 0, Object* next = nullptr, Object* prev = nullptr);
    virtual ~Object();

    int getX() const;
    int getY() const;
    string getID() const;
    char getGlyph() const;
    int getPriorityInv() const;
    Object* getNext() const;
    Object* getPrev() const;

    void setX(int x);
    void setY(int y);
    void setID(string input);
    void setGlyph(char g);
    void setNext(Object* n);
    void setPrev(Object* p);
    void setPriorityInv(int h);
};

class StaticObjects : public Object {
public:
    StaticObjects(int x, int y, string id, char glyph, int priority = 0);
    ~StaticObjects();
};

class StationaryVehicles : public StaticObjects {
public:
    StationaryVehicles(int x, int y, string id);
    ~StationaryVehicles();
};

class TrafficSigns : public StaticObjects {
private:
    string type; // Placeholder; Only valid value is STOP

public:
    TrafficSigns(int x, int y, string id, string type = "STOP");
    ~TrafficSigns();
};

class TrafficLights : public StaticObjects {
private:
    TrafficLightState state; // RED:4, YELLOW:2, GREEN:8
    int tickTracker;

public:
    TrafficLights(int x, int y, string id, TrafficLightState state = RED, int tickTracker = 0);
    ~TrafficLights();

    bool updateState(); // Updates state based on tickTracker
};

class MovingObjects : public Object {
protected:
    int speed;
    directionState direction;

public:
    MovingObjects(int x, int y, string id, char glyph, int speed, int priority = 0);
    ~MovingObjects();

    void move(GridWorld& world); // Moves object
    int getSpeed() const;
    directionState getDirection() const;
};

class Bikes : public MovingObjects {
public:
    Bikes(int x, int y, string id);
    ~Bikes();
};

class Cars : public MovingObjects {
public:
    Cars(int x, int y, string id);
    ~Cars();
};

class GridWorld {
private:
    int dimX;
    int dimY;
    Object** grid;
    char* displayGrid;
    int seed;

    TrafficLights** trafficLightsList;
    Cars** carsList;
    Bikes** bikesList;

    int bikes;
    int cars_mov;
    int cars_static;
    int signs;
    int lights;

public:
    GridWorld(int x = 40, int y = 40, int seed = time(nullptr), int bikes = 4, int cars_mov = 3, int cars_static = 5, int signs = 2, int lights = 2);
    ~GridWorld();

    int getDimX() const;
    int getDimY() const;
    int index(int x, int y) const;
    Object* getv(int x, int y) const;
    void setv(int x, int y, Object* obj);

    void generateWorld();
    void update();
    void displayWorld() const;
    void collisionHandler(Position pos, Object* objNew);
    void reorganize(Position pos, Object* obj);
};
