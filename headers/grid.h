#pragma once

#include <cstdlib>
#include <ctime>
#include <string>

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
    Object* next; //Pointer to next object in linked list for multiple objects in same grid cell
    Object* prev; //Pointer to previous object in linked list
    int priority;

public:
    Object(int x = 0, int y = 0, string id = "def", char glyph = 'X', int priority = 0, Object* next = NULL, Object* prev = NULL)
        : pos({ x, y }), id(id), glyph(glyph), next(next), prev(prev), priority(priority) {
    }
    virtual ~Object() {}

    int getX() const { return pos.x; }
    int getY() const { return pos.y; }
    string getID() const { return id; }
    char getGlyph() const { return glyph; }
    int getPriorityInv() const { return priority; }
    Object* getNext() const { return next; }
    Object* getPrev() const { return prev; }

    void setX(int x) { pos.x = x; }
    void setY(int y) { pos.y = y; }
    void setID(string input) { id = input; }
    void setGlyph(char g) { glyph = g; }
    void setNext(Object* n) { next = n; }
    void setPrev(Object* p) { prev = p; }
    void setPriorityInv(int h) { priority = h; }
};

class StaticObjects : public Object {
public:
    StaticObjects(int x, int y, string id, char glyph, int priority = 0)
        : Object(x, y, id, glyph, priority) {}
    ~StaticObjects() {}
};

class StationaryVehicles : public StaticObjects {
public:
    StationaryVehicles(int x, int y, string id)
        : StaticObjects(x, y, id, 'P', 8) {}
    ~StationaryVehicles() {}
};

class TrafficSigns : public StaticObjects {
private:
    string type; //Placeholder; Only valid value is STOP

public:
    TrafficSigns(int x, int y, string id, string type = "STOP")
        : StaticObjects(x, y, id, 'S', 4), type(type) {}
    ~TrafficSigns() {}
};

class TrafficLights : public StaticObjects {
private:
    TrafficLightState state; //Duration: RED: 4TICKS, YELLOW: 2TICKS, GREEN: 8TICKS
    int tickTracker; //Tracks ticks to change state

public:
    TrafficLights(int x, int y, string id, TrafficLightState state = RED, int tickTracker = 0)
        : StaticObjects(x, y, id, 'R', 2), state(state), tickTracker(tickTracker) {}
    ~TrafficLights() {}

    bool updateState();
};

class MovingObjects : public Object {
protected:
    int speed;
    int direction;

public:
    MovingObjects(int x, int y, string id, char glyph, int speed, int priority = 0)
        : Object(x, y, id, glyph, priority), speed(speed) {
        direction = rand() % 4;
    }
    ~MovingObjects() {}

    void move(GridWorld& world); //Moves object in [direction] by [speed]
    int getSpeed() const { return speed; }
    int getDirection() const { return direction; }
};

class Bikes : public MovingObjects {
public:
    Bikes(int x, int y, string id)
        : MovingObjects(x, y, id, 'B', HALF_SPEED, 5) {}
    ~Bikes() {}
};

class Cars : public MovingObjects {
public:
    Cars(int x, int y, string id)
        : MovingObjects(x, y, id, 'C', FULL_SPEED, 6) {}
    ~Cars() {}
};

class GridWorld {
private:
    int dimX;
    int dimY;
    Object** grid;
    char* displayGrid;
    int seed;

    TrafficLights** trafficLightsList; //Array of pointers to traffic lights for updating
    Cars** carsList; //Array of pointers to cars for updating
    Bikes** bikesList; //Array of pointers to bikes for updating

    int bikes;
    int cars_mov;
    int cars_static;
    int signs;
    int lights;

public:
    GridWorld(int x = 40, int y = 40, int seed = time(NULL), int bikes = 4, int cars_mov = 3, int cars_static = 5, int signs = 2, int lights = 2);
    ~GridWorld();

    int getDimX() const { return dimX; }
    int getDimY() const { return dimY; }
    int index(int x, int y) const { return x + (dimX * y); }
    Object* getv(int x, int y) const { return grid[index(x, y)]; }
    void setv(int x, int y, Object* obj);

    void generateWorld();
    void update();
    void displayWorld() const;
    void collisionHandler(Position pos, Object* objNew);
    void reorganize(Position pos, Object* obj);
};
