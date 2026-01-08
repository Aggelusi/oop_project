#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

enum directionState {
    NORTH, SOUTH, EAST, WEST
};

enum TrafficLightState {RED, GREEN, YELLOW};

class Object{
    protected:
        int xpos;
        int ypos;
        string id;
        char glyph;
    public: 
        Object(int x = 0, int y = 0, string id = "def", char glyph = 'X') : xpos(x), ypos(y), id(id), glyph(glyph) {cout << "Object " << id << " created at (" << xpos << "," << ypos << ")\n";} //Constructor
        ~Object() {cout << "Object " << id << " destroyed.\n";}
        int getX() const{return xpos;};
        int getY() const{return ypos;};
        string getID() const{return id;};
        char getGlyph() const{return glyph;};

        void setX(int x) {xpos = x;};
        void setY(int y) {ypos = y;};
        void setID(string input) {id = input;};
        void setGlyph(char g) {glyph = g;};
};

class StaticObjects : public Object{
    public:
        StaticObjects(int x, int y, string id, char glyph) : Object(x, y, id, glyph) {cout << "StaticObjects " << id << " created.\n";} //Constructor
        ~StaticObjects() {cout << "StaticObjects " << id << " destroyed.\n";}
};

class StationaryVehicles : public StaticObjects{
    public:
        StationaryVehicles(int x, int y, string id, char glyph) : StaticObjects(x, y, id, glyph) {cout << "StationaryVehicles " << id << " created.\n";} //Constructor
        ~StationaryVehicles() {cout << "StationaryVehicles " << id << " destroyed.\n";}
};

class TrafficSigns : public StaticObjects{
    private:
        string type; //Placeholder; Only valid value is STOP
    public:
        TrafficSigns(int x, int y, string id, char glyph, string type= "STOP") : StaticObjects(x, y, id, glyph), type(type) {cout << "TrafficSigns " << id << " created.\n";} //Constructor
        ~TrafficSigns() {cout << "TrafficSigns " << id << " destroyed.\n";}
};

class TrafficLights : public StaticObjects{
    private:
        static TrafficLightState state; //Duration: RED: 4TICKS, YELLOW: 2TICKS, GREEN: 8TICKS
        static int tickTracker; //Tracks ticks to change state
    public:
        TrafficLights(int x, int y, string id, char glyph) : StaticObjects(x, y, id, glyph) {state = RED; tickTracker = 0; cout << "TrafficLights " << id << " created.\n";} //Constructor
        ~TrafficLights() {cout << "TrafficLights " << id << " destroyed.\n";}

        void updateState(){ //Increments tickTracker to account for tick incrementation and changes state if duration has passed (>=).
            tickTracker++;
            if (state == RED && tickTracker >= 4){
                state = GREEN;
                tickTracker = 0;
            }
            else if (state == GREEN && tickTracker >= 8){
                state = YELLOW;
                tickTracker = 0;
            }
            else if (state == YELLOW && tickTracker >= 2){
                state = RED;
                tickTracker = 0;
            }
        }
};

class MovingObjects : public Object{
    protected:
        int speed;
        int direction;
    public:
        MovingObjects(int x, int y, string id, char glyph, int speed) : Object(x, y, id, glyph), speed(speed){
            direction = rand() % 4; //Randomly assign direction
            cout << "MovingObjects " << id << " created.\n";
        } //Constructor
        ~MovingObjects() {cout << "MovingObjects " << id << " destroyed.\n";}

        
};


class GridWorld{
    private:
        int dimX;
        int dimY;
        int *grid;

        int bikes;
        int cars_mov;
        int cars_static;
        int signs;
        int lights;

    public:
        GridWorld(int x = 40, int y = 40, int bikes = 4, int cars_mov = 3, int cars_static = 5, int signs = 2, int lights = 2) : dimX(x), dimY(y), bikes(bikes), cars_mov(cars_mov), cars_static(cars_static), signs(signs), lights(lights)
            {grid = new int[x * y](); cout << "GridWorld created with size of " << dimX << "x" << dimY << "\n";} //Initialize grid with size of x * y and all values set to 0
        ~GridWorld() {delete[] grid; cout << "GridWorld destroyed.\n";}

        int index(int x, int y) const{return x + (dimX * y);} //Returns index to access grid at (x,y), (equivalent to grid[x][y] if array was 2dim)
        int getv(int x, int y) const {return grid[index(x,y)];} //Returns value of grid at (x,y)
        void setv(int x, int y, int value) {grid[index(x,y)] = value;} //Sets value of grid at (x,y) to value
};

int main(int argc, char *argv[]){
    GridWorld world(10, 10);
    cout << "Accessing(3,4): " << world.getv(3,4) << "\n";
    return 0;
}