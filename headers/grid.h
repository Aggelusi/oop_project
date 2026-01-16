#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <typeinfo>
#include <ctime>
#include "types.h"
using namespace std;

class GridWorld; //Forward declaration

class Object{
    protected:
        Position pos;
        string id;
        char glyph;
        Object* next; //Pointer to next object in linked list for multiple objects in same grid cell
        Object* prev; //Pointer to previous object in linked list
        int priority;
    public: 
        Object(int x, int y, string id, char glyph, int priority, Object* next, Object* prev);
        virtual ~Object();
        int getX() const;
        int getY() const;
        string getID() const;
        char getGlyph() const;
        int getPriority() const;
        Object* getNext() const;  //Returns pointer to next object in list
        Object * getPrev() const;  //Returns pointer to previous object in list

        void setX(int x);
        void setY(int y);
        void setID(string input);
        void setGlyph(char g);
        void setNext(Object* n); //Sets pointer to next object in list
        void setPrev(Object* p); //Sets pointer to previous object in list
        void setPriority(int h);
};

class StaticObjects : public Object{
    public:
        StaticObjects(int x, int y, string id, char glyph, int priority);
        ~StaticObjects();
};

class StationaryVehicles : public StaticObjects{
    public:
        StationaryVehicles(int x, int y, string id);
        ~StationaryVehicles();
};

class TrafficSigns : public StaticObjects{
    private:
        string type; //Placeholder; Only valid value is STOP
    public:
        TrafficSigns(int x, int y, string id, string type);
        ~TrafficSigns();
};

class TrafficLights : public StaticObjects{
    private:
        TrafficLightState state; //Duration: RED: 4TICKS, YELLOW: 2TICKS, GREEN: 8TICKS
        int tickTracker; //Tracks ticks to change state
    public:
        TrafficLights(int x, int y, string id, TrafficLightState state, int tickTracker);
        ~TrafficLights();

        bool updateState(); //Increments tickTracker to account for tick incrementation and changes state if duration has passed (>=).
  
};

class MovingObjects : public Object{
    protected:
        int speed;
        int direction;
    public:
        MovingObjects(int x, int y, string id, char glyph, int speed, int priority);
        ~MovingObjects();

        int getSpeed() const;
        int getDirection() const;
        void move(GridWorld& world); //Moves object in [direction] by [speed]
};

class Bikes : public MovingObjects{
    public:
        Bikes(int x, int y, string id);
        ~Bikes();
};

class Cars : public MovingObjects{
    public:
        Cars(int x, int y, string id);
        ~Cars();

};

class GridWorld{
    private:
        int dimX;
        int dimY;
        Object** grid;
        char* displayGrid;
        int seed;

        TrafficLights** trafficLightsList; //Array of pointers to traffic lights for updating
        Cars** carsList; //Array of pointers to cars for updating
        StationaryVehicles** stationaryVehiclesList; //Array of pointers to stationary vehicles for updating
        TrafficSigns** trafficSignsList; //Array of pointers to traffic signs for updating
        Bikes** bikesList; //Array of pointers to bikes for updating

        //Number of each object type
        int bikes;
        int cars_mov;
        int cars_static;
        int signs;
        int lights;
    public:
        GridWorld(int x, int y, int seed, int bikes, int cars_mov, int cars_static, int signs, int lights);
        ~GridWorld();
        int getDimX() const;
        int getDimY() const;
        int index(int x, int y) const; //Returns index to access grid at (x,y), (equivalent to grid[x][y] if array was 2dim)
        Object* getv(int x, int y) const;//Returns value of grid at (x,y)
        void setv(int x, int y, Object* obj); //Sets value of grid at (x,y) to pointer to object
        void generateWorld(); //Generates world with objects
        void update(); //Updates world state by one tick
        void displayWorld(Position vehiclePos) const; //Prints displayGrid
        void displaySurroundings(Position vehiclePos, int viewRadius) const; //Prints grid around vehicle position
        void collisionHandler(Position pos, Object* objNew); //Handles object collisions to properly set visibility
        void reorganize(Position pos, Object* obj); //Reorganize object's old position in grid
        void paramChecker(); //Ensures parameters are within acceptable ranges
};
