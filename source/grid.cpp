#include <iostream>
#include <string>
#include <cstdlib>
#include <typeinfo>
#include <ctime>
using namespace std;

class GridWorld;

class Object{
    protected:
        Position pos;
        string id;
        char glyph;
        Object* next; //Pointer to next object in linked list for multiple objects in same grid cell
        Object* prev; //Pointer to previous object in linked list
        int priority;
    public: 
        Object(int x = 0, int y = 0, string id = "def", char glyph = 'X', int priority = 0, Object* next = NULL, Object* prev = NULL) : pos({x, y}), id(id), glyph(glyph), next(next), prev(prev), priority(priority) {/*cout << "Object " << id << " created at (" << pos.x << "," << pos.y << ")\n";*/} //Constructor
        virtual ~Object() {/*cout << "Object " << id << " destroyed.\n";*/}
        int getX() const{return pos.x;};
        int getY() const{return pos.y;};
        string getID() const{return id;};
        char getGlyph() const{return glyph;};
        int getPriorityInv() const{return priority;};
        Object* getNext() const{return next;};  //Returns pointer to next object in list
        Object * getPrev() const{return prev;};  //Returns pointer to previous object in list

        void setX(int x) {pos.x = x;};
        void setY(int y) {pos.y = y;};
        void setID(string input) {id = input;};
        void setGlyph(char g) {glyph = g;};
        void setNext(Object* n) {next = n;}; //Sets pointer to next object in list
        void setPrev(Object* p) {prev = p;}; //Sets pointer to previous object in list
        void setPriorityInv(int h) {priority = h;};
};

class StaticObjects : public Object{
    public:
        StaticObjects(int x, int y, string id, char glyph, int priority = 0) : Object(x, y, id, glyph, priority) {/*cout << "StaticObjects " << id << " created.\n";*/} //Constructor
        ~StaticObjects() {/*cout << "StaticObjects " << id << " destroyed.\n";*/}
};

class StationaryVehicles : public StaticObjects{
    public:
        StationaryVehicles(int x, int y, string id) : StaticObjects(x, y, id, 'P', 8) {/*cout << "StationaryVehicles " << id << " created.\n";*/} //Constructor
        ~StationaryVehicles() {/*cout << "StationaryVehicles " << id << " destroyed.\n";*/}
};

class TrafficSigns : public StaticObjects{
    private:
        string type; //Placeholder; Only valid value is STOP
    public:
        TrafficSigns(int x, int y, string id, string type= "STOP") : StaticObjects(x, y, id, 'S', 4), type(type) {/*cout << "TrafficSigns " << id << " created.\n";*/} //Constructor
        ~TrafficSigns() {/*cout << "TrafficSigns " << id << " destroyed.\n";*/}
};

class TrafficLights : public StaticObjects{
    private:
        TrafficLightState state; //Duration: RED: 4TICKS, YELLOW: 2TICKS, GREEN: 8TICKS
        int tickTracker; //Tracks ticks to change state
    public:
        TrafficLights(int x, int y, string id, TrafficLightState state = RED, int tickTracker = 0) : StaticObjects(x, y, id, 'R', 2), state(state), tickTracker(tickTracker) {/*cout << "TrafficLights " << id << " created.\n";*/} //Constructor
        ~TrafficLights() {/*cout << "TrafficLights " << id << " destroyed.\n";*/}

        bool updateState(){ //Increments tickTracker to account for tick incrementation and changes state if duration has passed (>=).
            tickTracker++;
            if (state == RED && tickTracker >= 4){
                state = GREEN;
                glyph = 'G';
                setPriorityInv(7);
                tickTracker = 0;
                return true;
            }
            else if (state == GREEN && tickTracker >= 8){
                state = YELLOW;
                glyph = 'Y';
                setPriorityInv(3);
                tickTracker = 0;
                return true;
            }
            else if (state == YELLOW && tickTracker >= 2){
                state = RED;
                glyph = 'R';
                setPriorityInv(2);
                tickTracker = 0;
                return true;
            }
            else return false;
        }
};

class MovingObjects : public Object{
    protected:
        int speed;
        int direction;
    public:
        MovingObjects(int x, int y, string id, char glyph, int speed, int priority = 0) : Object(x, y, id, glyph, priority), speed(speed){
            direction = rand() % 4; //Randomly assign direction
            //cout << "MovingObjects " << id << " created.\n";
        } //Constructor
        ~MovingObjects() {/*cout << "MovingObjects " << id << " destroyed.\n";*/}

        void move(GridWorld& world); //Moves object in [direction] by [speed]
};

class Bikes : public MovingObjects{
    public:
        Bikes(int x, int y, string id) : MovingObjects(x, y, id, 'B', HALF_SPEED, 5) {/*cout << "Bikes " << id << " created.\n";*/} //Constructor
        ~Bikes() {/*cout << "Bikes " << id << " destroyed.\n";*/}
};

class Cars : public MovingObjects{
    public:
        Cars(int x, int y, string id) : MovingObjects(x, y, id, 'C', FULL_SPEED, 6) {/*cout << "Cars " << id << " created.\n";*/} //Constructor
        ~Cars() {/*cout << "Cars " << id << " destroyed.\n";*/}
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
        Bikes** bikesList; //Array of pointers to bikes for updating

        //Number of each object type
        int bikes;
        int cars_mov;
        int cars_static;
        int signs;
        int lights;
    public:
        GridWorld(int x = 40, int y = 40, int seed = time(NULL), int bikes = 4, int cars_mov = 3, int cars_static = 5, int signs = 2, int lights = 2) : seed(seed), dimX(x), dimY(y), bikes(bikes), cars_mov(cars_mov), cars_static(cars_static), signs(signs), lights(lights)
            {grid = new Object*[x * y](); displayGrid = new char[x * y]; trafficLightsList = new TrafficLights*[lights]; carsList = new Cars*[cars_mov]; bikesList = new Bikes*[bikes]; fill_n(displayGrid, x * y, '.'); /*cout << "GridWorld created with size of " << dimX << "x" << dimY << "\n";*/} //Initialize grid with size of x * y and all values set to 0
        ~GridWorld() {for (int i = 0; i < dimX * dimY; i++) delete grid[i]; delete[] grid; delete[] displayGrid; delete[] trafficLightsList; delete[] carsList; delete[] bikesList; cout << "GridWorld destroyed.\n";}

        int getDimX() const{return dimX;};
        int getDimY() const{return dimY;};
        int index(int x, int y) const{return x + (dimX * y);} //Returns index to access grid at (x,y), (equivalent to grid[x][y] if array was 2dim)
        Object* getv(int x, int y) const {return grid[index(x,y)];} //Returns value of grid at (x,y)
        void setv(int x, int y, Object* obj) {grid[index(x,y)] = obj; if(obj != NULL) {displayGrid[index(x,y)] = obj->getGlyph();} else {displayGrid[index(x,y)] = '.';}} //Sets value of grid at (x,y) to pointer to object
        void generateWorld(); //Generates world with objects
        void update(); //Updates world state by one tick
        void displayWorld() const { //Prints displayGrid
            for (int j = 0; j < dimY; j++){
                for (int i = 0; i < dimX; i++){
                    cout << displayGrid[index(i,j)];
                }
                cout << "\n";
            }
        }
        void collisionHandler(Position pos, Object* objNew); //Handles object collisions to properly set visibility
        void reorganize(Position pos, Object* obj); //Reorganize object's old position in grid
};

void GridWorld::collisionHandler(Position pos, Object* objNew){ //Handles object collisions to properly set visibility
    Object* objOld = getv(pos.x, pos.y); //Get existing object at position
    if (objOld->getPriorityInv() > objNew->getPriorityInv()){ //New object has higher priority, becomes visible
        objNew->setNext(objOld);
        objOld->setPrev(objNew);
        setv(pos.x, pos.y, objNew); //Set grid cell to new object
        displayGrid[index(pos.x, pos.y)] = objNew->getGlyph(); //Set displayGrid cell to new object's glyph
    }
    else if (objOld->getPriorityInv() < objNew->getPriorityInv()){ //Old object has higher priority, remains visible
        Object* temp = objOld->getNext();
        while (temp != NULL && temp->getPriorityInv() < objNew->getPriorityInv()) //Traverse list to find correct position
            temp = temp->getNext();
        temp->getPrev()->setNext(objNew); //Update pointers
        objNew->setPrev(temp->getPrev());
        objNew->setNext(temp);
        if (temp != NULL)
            temp->setPrev(objNew);
    }
    else{ //Equal priority
        objNew->setNext(objOld->getNext()); //Insert new object after old object
        if (objOld->getNext() != NULL)
            objOld->getNext()->setPrev(objNew);
        objOld->setNext(objNew);
        objNew->setPrev(objOld);
    }
    return;
}

void GridWorld::reorganize(Position pos, Object* obj){ //Reorganize object's old position in grid
    if (obj->getPrev() != NULL){ //Object is not at the head of the list
        obj->getPrev()->setNext(obj->getNext());
        obj->getNext()->setPrev(obj->getPrev());
    }
    else if (obj->getNext() != NULL){ //Object is at the head of the list and there is a next object
        setv(pos.x, pos.y, obj->getNext()); //Set grid cell to next object
        obj->getNext()->setPrev(NULL);
    }
    else //Object is only object in cell
        setv(pos.x, pos.y, NULL); //Set grid cell to NULL
    obj->setNext(NULL);
    obj->setPrev(NULL);
    return;
}

void GridWorld::generateWorld(){
    srand(seed); //Set seed
    Object* objPtr;
    for (int i = 0; i < cars_mov; i++){ //Moving Cars
        objPtr = new Cars(rand() % dimX, rand() % dimY, "CAR:" + to_string(i));
        carsList[i] = dynamic_cast<Cars*>(objPtr); //Store pointer in carsList for updating
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    for(int i = 0; i < bikes; i++){ //Moving Bikes
        objPtr = new Bikes(rand() % dimX, rand() % dimY, "BIKE:" + to_string(i));
        bikesList[i] = dynamic_cast<Bikes*>(objPtr); //Store pointer in bikesList for updating
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    for(int i = 0; i < cars_static; i++){ //Stationary Vehicles
        objPtr = new StationaryVehicles(rand() % dimX, rand() % dimY, "STATIONARYVEHICLE:" + to_string(i));
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    for(int i = 0; i < signs; i++){ //Traffic Signs
        objPtr = new TrafficSigns(rand() % dimX, rand() % dimY, "TRAFFICSIGN:" + to_string(i));
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    for(int i = 0; i < lights; i++){ //Traffic Lights
        objPtr = new TrafficLights(rand() % dimX, rand() % dimY, "TRAFFICLIGHT:" + to_string(i));
        trafficLightsList[i] = dynamic_cast<TrafficLights*>(objPtr); //Store pointer in trafficLightsList for updating
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    return;
}

void GridWorld::update(){
    for (int i = 0; i < lights; i++){ //Update traffic lights
        if (trafficLightsList[i]->updateState())
            displayGrid[index(trafficLightsList[i]->getX() ,trafficLightsList[i]->getY())] = trafficLightsList[i]->getGlyph();
    }
    for (int i = 0; i < cars_mov; i++){ //Update cars
        if (carsList[i]->getX() != -1)
            carsList[i]->move(*this);
    }
    for (int i = 0; i < bikes; i++){ //Update bikes
        if (bikesList[i]->getX() != -1)
            bikesList[i]->move(*this);
    }
    return;
}

void MovingObjects::move(GridWorld& world){ //Moves object in [direction] by [speed]
    Position posOld = pos;
    if (next != NULL){
        world.setv(pos.x, pos.y, next); //Set grid cell to next object in linked list if exists
    }
    if (direction == NORTH){
        setY(pos.y - speed);
        if (pos.y < 0){ //Check for out of bounds
            setX(-1);
        }
    }
    else if (direction == SOUTH){
        setY(pos.y + speed);
        if (pos.y >= world.getDimY()){ //Check for out of bounds
            setX(-1);
        }
    }
    else if (direction == EAST){
        setX(pos.x + speed);
        if (pos.x >= world.getDimX()){ //Check for out of bounds
            setX(-1);
        }
    }
    else if (direction == WEST){
        setX(pos.x - speed);
        if (pos.x < 0){ //Check for out of bounds
            setX(-1);
        }
    }
    world.reorganize(posOld, this); //Reorganize old position in grid
    if (pos.x == -1) return; //Out of bounds, don't place
    if (world.getv(pos.x, pos.y) != NULL) //Check for collision
            world.collisionHandler(pos, this);
    else {
        world.setv(pos.x, pos.y, this);
    }
}