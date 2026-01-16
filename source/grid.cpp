
#include "../headers/grid.h"

//class Object
Object::Object(int x = 0, int y = 0, string id = "def", char glyph = 'X', int priority = 0, Object* next = NULL, Object* prev = NULL) : pos({x, y}), id(id), glyph(glyph), next(next), prev(prev), priority(priority) {/*cout << "Object " << id << " created at (" << pos.x << "," << pos.y << ")\n";*/} //Constructor
Object::~Object() {/*cout << "Object " << id << " destroyed.\n";*/}
int Object::getX() const{return pos.x;};
int Object::getY() const{return pos.y;};
string Object::getID() const{return id;};
char Object::getGlyph() const{return glyph;};
int Object::getPriority() const{return priority;};
Object* Object::getNext() const{return next;};  //Returns pointer to next object in list
Object * Object::getPrev() const{return prev;};  //Returns pointer to previous object in list
void Object::setX(int x) {pos.x = x;};
void Object::setY(int y) {pos.y = y;};
void Object::setID(string input) {id = input;};
void Object::setGlyph(char g) {glyph = g;};
void Object::setNext(Object* n) {next = n;}; //Sets pointer to next object in list
void Object::setPrev(Object* p) {prev = p;}; //Sets pointer to previous object in list
void Object::setPriority(int h) {priority = h;};

//class StaticObjects
StaticObjects::StaticObjects(int x, int y, string id, char glyph, int priority = 0) : Object(x, y, id, glyph, priority) {/*cout << "StaticObjects " << id << " created.\n";*/} //Constructor
StaticObjects::~StaticObjects() {/*cout << "StaticObjects " << id << " destroyed.\n";*/}

//class StationaryVehicles
StationaryVehicles::StationaryVehicles(int x, int y, string id) : StaticObjects(x, y, id, 'P', 1) {/*cout << "StationaryVehicles " << id << " created.\n";*/} //Constructor
StationaryVehicles::~StationaryVehicles() {/*cout << "StationaryVehicles " << id << " destroyed.\n";*/}


//class TrafficSigns
TrafficSigns::TrafficSigns(int x, int y, string id, string type= "STOP") : StaticObjects(x, y, id, 'S', 5), type(type) {/*cout << "TrafficSigns " << id << " created.\n";*/} //Constructor
TrafficSigns::~TrafficSigns() {/*cout << "TrafficSigns " << id << " destroyed.\n";*/}


//class TrafficLights
TrafficLights::TrafficLights(int x, int y, string id, TrafficLightState state = RED, int tickTracker = 0) : StaticObjects(x, y, id, 'R', 7), state(state), tickTracker(tickTracker) {/*cout << "TrafficLights " << id << " created.\n";*/} //Constructor
TrafficLights::~TrafficLights() {/*cout << "TrafficLights " << id << " destroyed.\n";*/}
bool TrafficLights::updateState(){ //Increments tickTracker to account for tick incrementation and changes state if duration has passed (>=).
    tickTracker++;
    if (state == RED && tickTracker >= 4){
        state = GREEN;
        glyph = 'G';
        setPriority(2);
        tickTracker = 0;
        return true;
    }
    else if (state == GREEN && tickTracker >= 8){
        state = YELLOW;
        glyph = 'Y';
        setPriority(6);
        tickTracker = 0;
        return true;
    }
    else if (state == YELLOW && tickTracker >= 2){
        state = RED;
        glyph = 'R';
        setPriority(7);
        tickTracker = 0;
        return true;
    }
    else return false;
}


//class MovingObjects
MovingObjects::MovingObjects(int x, int y, string id, char glyph, int speed, int priority = 0) : Object(x, y, id, glyph, priority), speed(speed){
    direction = rand() % 4; //Randomly assign direction
    //cout << "MovingObjects " << id << " created.\n";
} //Constructor
MovingObjects::~MovingObjects() {/*cout << "MovingObjects " << id << " destroyed.\n";*/}
int MovingObjects::getSpeed() const { return speed; }
int MovingObjects::getDirection() const { return direction; }

//class Bikes
Bikes::Bikes(int x, int y, string id) : MovingObjects(x, y, id, 'B', HALF_SPEED, 4) {/*cout << "Bikes " << id << " created.\n";*/} //Constructor
Bikes::~Bikes() {/*cout << "Bikes " << id << " destroyed.\n";*/}


//class Cars
Cars::Cars(int x, int y, string id) : MovingObjects(x, y, id, 'C', FULL_SPEED, 3) {/*cout << "Cars " << id << " created.\n";*/} //Constructor
Cars::~Cars() {/*cout << "Cars " << id << " destroyed.\n";*/}


//class GridWorld
GridWorld::GridWorld(int x = 40, int y = 40, int seed = time(NULL), int bikes = 4, int cars_mov = 3, int cars_static = 5, int signs = 2, int lights = 2) : seed(seed), dimX(x), dimY(y), bikes(bikes), cars_mov(cars_mov), cars_static(cars_static), signs(signs), lights(lights)
    {paramChecker(); grid = new Object*[x * y](); displayGrid = new char[x * y]; trafficLightsList = new TrafficLights*[lights]; carsList = new Cars*[cars_mov]; bikesList = new Bikes*[bikes]; stationaryVehiclesList = new StationaryVehicles*[cars_static]; trafficSignsList = new TrafficSigns*[signs]; fill_n(displayGrid, x * y, '.'); /*cout << "GridWorld created with size of " << dimX << "x" << dimY << "\n";*/} //Initialize grid with size of x * y and all values set to 0
GridWorld::~GridWorld() {delete[] grid; delete[] displayGrid; for (int i = 0; i < lights; i++) delete trafficLightsList[i]; for (int i = 0; i < cars_mov; i++) delete carsList[i]; for (int i = 0; i < bikes; i++) delete bikesList[i]; for (int i = 0; i < cars_static; i++) delete stationaryVehiclesList[i]; for (int i = 0; i < signs; i++) delete trafficSignsList[i]; delete[] trafficLightsList; delete[] carsList; delete[] bikesList; delete[] stationaryVehiclesList; delete[] trafficSignsList; cout << "GridWorld destroyed.\n";}
int GridWorld::getDimX() const{return dimX;};
int GridWorld::getDimY() const{return dimY;};
int GridWorld::index(int x, int y) const{return x + (dimX * y);} //Returns index to access grid at (x,y), (equivalent to grid[x][y] if array was 2dim)
Object* GridWorld::getv(int x, int y) const {return grid[index(x,y)];} //Returns value of grid at (x,y)
void GridWorld::setv(int x, int y, Object* obj) {grid[index(x,y)] = obj; if(obj != NULL) {displayGrid[index(x,y)] = obj->getGlyph();} else {displayGrid[index(x,y)] = '.';}} //Sets value of grid at (x,y) to pointer to object
void GridWorld::displayWorld(Position vehiclePos) const { //Prints displayGrid
    for (int j = 0; j < dimY; j++){
        for (int i = 0; i < dimX; i++){
            if (i == vehiclePos.x && j == vehiclePos.y)
                cout << '@';
            else
                cout << displayGrid[index(i,j)];
        }
        cout << "\n";
    }
}
void GridWorld::displaySurroundings(Position vehiclePos, int viewRadius) const { //Prints displayGrid around vehicle position within viewRadius
    int startX = max(0, vehiclePos.x - viewRadius);
    int endX = min(dimX - 1, vehiclePos.x + viewRadius);
    int startY = max(0, vehiclePos.y - viewRadius);
    int endY = min(dimY - 1, vehiclePos.y + viewRadius);

    for (int j = startY; j <= endY; j++){
        for (int i = startX; i <= endX; i++){
            if (i == vehiclePos.x && j == vehiclePos.y)
                cout << '@';
            else
                cout << displayGrid[index(i,j)];
        }
        cout << "\n";
    }
}


void GridWorld::paramChecker(){ //Ensures parameters are within acceptable ranges
    int maxObjects = dimX * dimY / 3; //Max objects is 1/3 of grid size to prevent overcrowding
    if (bikes < 0 || bikes > maxObjects) bikes = 4;
    if (cars_mov < 0 || cars_mov > maxObjects) cars_mov = 3;
    if (cars_static < 0 || cars_static > maxObjects) cars_static = 5;
    if (signs < 0 || signs > maxObjects) signs = 2;
    if (lights < 0 || lights > maxObjects) lights = 2;
    int totalObjects = bikes + cars_mov + cars_static + signs + lights;
    if (totalObjects > maxObjects){ //Revert to defaults if total exceeds max
        bikes = 4;
        cars_mov = 3;
        cars_static = 5;
        signs = 2;
        lights = 2;
        
    }
}

void GridWorld::collisionHandler(Position pos, Object* objNew){ //Handles object collisions to properly set visibility
    Object* objOld = getv(pos.x, pos.y); //Get existing object at position
    if (objOld->getPriority() < objNew->getPriority()){ //New object has higher priority, becomes visible
        objNew->setNext(objOld);
        objOld->setPrev(objNew);
        setv(pos.x, pos.y, objNew); //Set grid cell to new object
        displayGrid[index(pos.x, pos.y)] = objNew->getGlyph(); //Set displayGrid cell to new object's glyph
    }
    else if (objOld->getPriority() > objNew->getPriority()){ //Old object has higher priority, remains visible
        Object* temp = objOld->getNext();
        while (temp != NULL && temp->getPriority() < objNew->getPriority()) //Traverse list to find correct position
            temp = temp->getNext();
        if (temp != NULL) {
            // Found position in middle of list
            temp->getPrev()->setNext(objNew); //Update pointers
            objNew->setPrev(temp->getPrev());
            objNew->setNext(temp);
            temp->setPrev(objNew);
        } else {
            // Reached end of list, insert at end
            Object* last = objOld;
            while (last->getNext() != NULL)
                last = last->getNext();
            last->setNext(objNew);
            objNew->setPrev(last);
            objNew->setNext(NULL);
        }
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
        if (obj->getNext() != NULL)
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
    int x, y;
    for (int i = 0; i < cars_mov; i++){ //Moving Cars
        do{
            x = rand() % dimX;
            y = rand() % dimY;
        } while (getv(x, y) != NULL); //Ensure no initial overlap
        objPtr = new Cars(x, y, "CAR:" + to_string(i));
        carsList[i] = dynamic_cast<Cars*>(objPtr); //Store pointer in carsList for updating
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    for(int i = 0; i < bikes; i++){ //Moving Bikes
        do{
            x = rand() % dimX;
            y = rand() % dimY;
        } while (getv(x, y) != NULL); //Ensure no initial overlap
        objPtr = new Bikes(x, y, "BIKE:" + to_string(i));
        bikesList[i] = dynamic_cast<Bikes*>(objPtr); //Store pointer in bikesList for updating
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    for(int i = 0; i < cars_static; i++){ //Stationary Vehicles
        do{
            x = rand() % dimX;
            y = rand() % dimY;
        } while (getv(x, y) != NULL); //Ensure no initial overlap
        objPtr = new StationaryVehicles(x, y, "STATIONARYVEHICLE:" + to_string(i));
        stationaryVehiclesList[i] = dynamic_cast<StationaryVehicles*>(objPtr); //Store pointer in stationaryVehiclesList for updating
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    for(int i = 0; i < signs; i++){ //Traffic Signs
        do{
            x = rand() % dimX;
            y = rand() % dimY;
        } while (getv(x, y) != NULL); //Ensure no initial overlap
        objPtr = new TrafficSigns(x, y, "TRAFFICSIGN:" + to_string(i));
        trafficSignsList[i] = dynamic_cast<TrafficSigns*>(objPtr); //Store pointer in trafficSignsList for updating
        setv(objPtr->getX(), objPtr->getY(), objPtr);
    }
    for(int i = 0; i < lights; i++){ //Traffic Lights
        do{
            x = rand() % dimX;
            y = rand() % dimY;
        } while (getv(x, y) != NULL); //Ensure no initial overlap
        objPtr = new TrafficLights(x, y, "TRAFFICLIGHT:" + to_string(i));
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
