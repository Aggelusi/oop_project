#include <iostream>
#include <string>
#include <cstdlib>
#include <typeinfo>
#include <ctime>
#include <algorithm>
#include "../headers/grid.h"
using namespace std;

Object::Object(int x, int y, string id, char glyph, int priority, Object* next, Object* prev)
    : pos({x, y}), id(id), glyph(glyph), next(next), prev(prev), priority(priority) {}

Object::~Object() {}

int Object::getX() const { return pos.x; }
int Object::getY() const { return pos.y; }
string Object::getID() const { return id; }
char Object::getGlyph() const { return glyph; }
int Object::getPriorityInv() const { return priority; }
Object* Object::getNext() const { return next; }
Object* Object::getPrev() const { return prev; }

void Object::setX(int x) { pos.x = x; }
void Object::setY(int y) { pos.y = y; }
void Object::setID(string input) { id = input; }
void Object::setGlyph(char g) { glyph = g; }
void Object::setNext(Object* n) { next = n; }
void Object::setPrev(Object* p) { prev = p; }
void Object::setPriorityInv(int h) { priority = h; }

StaticObjects::StaticObjects(int x, int y, string id, char glyph, int priority)
    : Object(x, y, id, glyph, priority) {}

StaticObjects::~StaticObjects() {}

StationaryVehicles::StationaryVehicles(int x, int y, string id)
    : StaticObjects(x, y, id, 'P', 8) {}

StationaryVehicles::~StationaryVehicles() {}

TrafficSigns::TrafficSigns(int x, int y, string id, string type)
    : StaticObjects(x, y, id, 'S', 4), type(type) {}

TrafficSigns::~TrafficSigns() {}

TrafficLights::TrafficLights(int x, int y, string id, TrafficLightState state, int tickTracker)
    : StaticObjects(x, y, id, 'R', 2), state(state), tickTracker(tickTracker) {}

TrafficLights::~TrafficLights() {}

bool TrafficLights::updateState() {
    tickTracker++;
    if (state == RED && tickTracker >= 4) {
        state = GREEN;
        glyph = 'G';
        setPriorityInv(7);
        tickTracker = 0;
        return true;
    }
    if (state == GREEN && tickTracker >= 8) {
        state = YELLOW;
        glyph = 'Y';
        setPriorityInv(3);
        tickTracker = 0;
        return true;
    }
    if (state == YELLOW && tickTracker >= 2) {
        state = RED;
        glyph = 'R';
        setPriorityInv(2);
        tickTracker = 0;
        return true;
    }
    return false;
}

MovingObjects::MovingObjects(int x, int y, string id, char glyph, int speed, int priority)
    : Object(x, y, id, glyph, priority), speed(speed) {
    direction = static_cast<directionState>(rand() % 4);
}

MovingObjects::~MovingObjects() {}

int MovingObjects::getSpeed() const { return speed; }
directionState MovingObjects::getDirection() const { return direction; }

Bikes::Bikes(int x, int y, string id)
    : MovingObjects(x, y, id, 'B', HALF_SPEED, 5) {}

Bikes::~Bikes() {}

Cars::Cars(int x, int y, string id)
    : MovingObjects(x, y, id, 'C', FULL_SPEED, 6) {}

Cars::~Cars() {}

GridWorld::GridWorld(int x, int y, int seed, int bikes, int cars_mov, int cars_static, int signs, int lights)
    : dimX(x), dimY(y), seed(seed), bikes(bikes), cars_mov(cars_mov), cars_static(cars_static), signs(signs), lights(lights) {
    grid = new Object*[x * y]();
    displayGrid = new char[x * y];
    trafficLightsList = new TrafficLights*[lights];
    carsList = new Cars*[cars_mov];
    bikesList = new Bikes*[bikes];
    fill_n(displayGrid, x * y, '.');
}

GridWorld::~GridWorld() {
    for (int i = 0; i < dimX * dimY; i++) {
        delete grid[i];
    }
    delete[] grid;
    delete[] displayGrid;
    delete[] trafficLightsList;
    delete[] carsList;
    delete[] bikesList;
    cout << "GridWorld destroyed.\n";
}

int GridWorld::getDimX() const { return dimX; }
int GridWorld::getDimY() const { return dimY; }
int GridWorld::index(int x, int y) const { return x + (dimX * y); }
Object* GridWorld::getv(int x, int y) const { return grid[index(x, y)]; }
void GridWorld::setv(int x, int y, Object* obj) {
    grid[index(x, y)] = obj;
    if (obj != NULL) {
        displayGrid[index(x, y)] = obj->getGlyph();
    } else {
        displayGrid[index(x, y)] = '.';
    }
}

void GridWorld::displayWorld() const {
    for (int j = 0; j < dimY; j++) {
        for (int i = 0; i < dimX; i++) {
            cout << displayGrid[index(i, j)];
        }
        cout << "\n";
    }
}

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