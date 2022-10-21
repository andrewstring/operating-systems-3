#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>
#include <tuple>
#include <map>

// TODO Make it so each farmer has their own thread

using namespace std;

enum Access {
    northMut,
    southMut,
    criticalSection
};

enum Type {
    north,
    south
};


string typeOutput[2] = {"NorthBound, SouthBound"};

struct SharedMemory {
    int northMutex = 0;
    int southMutex = 0;
    int criticalSection = 0;
    queue<tuple<Type, string *>> northTravelers;
    queue<tuple<Type, string *>> southTravelers;
    int numOfNorthTravelers = 0;
    int numOfSouthTravelers = 0;
    int numOfPeopleOnBridge = 0;
} sMem;

void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case northMut:
            if (sharedMemory->northMutex == 0) {
                sharedMemory->northMutex = 1;
            }
            break;
        case southMut:
            if (sharedMemory->southMutex == 0) {
                sharedMemory->southMutex = 1;
            }
            break;
        case criticalSection:
            if (sharedMemory->criticalSection == 0) {
                sharedMemory->criticalSection = 1;
            }
            break;
    }
}

void signal(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case northMut:
            if (sharedMemory->northMutex == 1) {
                sharedMemory->northMutex = 0;
            }
            break;
        case southMut:
            if (sharedMemory->southMutex == 1) {
                sharedMemory->southMutex = 0;
            }
            break;
        case criticalSection:
            if (sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
            break;
    }
}

void* bridge(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    while(true) {
    ;
    }
}

void* northProducer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    string farmers[26] = {"Alpha", "Bravo", "Charlie", "Delta", "Echo",
                         "Foxtrot", "Golf", "Hotel", "India", "Juliet",
                         "Kilo", "Lima", "Mike", "November", "Oscar",
                         "Papa", "Quebec", "Romeo", "Sierra", "Tango",
                         "Uniform", "Victor", "Whisky", "X-Ray", "Yankee",
                         "Zulu"};
}

void* southProducer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    string farmers[26] = {"Alpha", "Bravo", "Charlie", "Delta", "Echo",
                          "Foxtrot", "Golf", "Hotel", "India", "Juliet",
                          "Kilo", "Lima", "Mike", "November", "Oscar",
                          "Papa", "Quebec", "Romeo", "Sierra", "Tango",
                          "Zulu"};
}

int main() {
    SharedMemory *sharedMemory = &sMem;

    pthread_t tidBridge;
    pthread_t tidNorth;
    pthread_t tidSouth;
    pthread_attr_t attrBridge;
    pthread_attr_t attrNorth;
    pthread_attr_t attrSouth;

    pthread_attr_init(&attrBridge);
    pthread_attr_init(&attrNorth);
    pthread_attr_init(&attrSouth);
    pthread_create(&tidBridge, &attrBridge, bridge, sharedMemory);
    pthread_create(&tidNorth, &attrNorth, northProducer, sharedMemory);
    pthread_create(&tidSouth, &attrSouth, southProducer, sharedMemory);

    pthread_join(tidBridge, NULL);
    pthread_join(tidNorth, NULL);
    pthread_join(tidSouth, NULL);
}

// Not done with this