#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>

using namespace std;

enum Access {smokerOneMutex,
        smokerTwoMutex,
        smokerThreeMutex,
        agentMutex};

struct SharedMemory {
    int smokerOneMutex = 0;
    int smokerTwoMutex = 0;
    int smokerThreeMutex = 0;
    int agentMutex = 0;
} sMem;

void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch (toAccess) {
        case smokerOneMutex:
            if (sharedMemory->smokerOneMutex == 0) {
                sharedMemory->smokerOneMutex = 1;
            }
        case smokerTwoMutex:
            if (sharedMemory->smokerTwoMutex == 0) {
                sharedMemory->smokerTwoMutex = 1;
            }
        case smokerThreeMutex:
            if (sharedMemory->smokerThreeMutex == 0) {
                sharedMemory->smokerThreeMutex = 1;
            }
        case agentMutex:
            if (sharedMemory->agentMutex == 0) {
                sharedMemory->agentMutex = 1;
            }
    }
}

void signal(SharedMemory *sharedMemory, Access toAccess) {
    switch (toAccess) {
        case smokerOneMutex:
            if (sharedMemory->smokerOneMutex == 1) {
                sharedMemory->smokerOneMutex = 0;
            }
        case smokerTwoMutex:
            if (sharedMemory->smokerTwoMutex == 1) {
                sharedMemory->smokerTwoMutex = 0;
            }
        case smokerThreeMutex:
            if (sharedMemory->smokerThreeMutex == 1) {
                sharedMemory->smokerThreeMutex = 0;
            }
        case agentMutex:
            if (sharedMemory->agentMutex == 1) {
                sharedMemory->agentMutex = 0;
            }
    }
}

void* smoker(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;
    cout << "smoker started" << endl;
}

void* agent(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;
    cout << "agent started" << endl;
}

int main() {
    SharedMemory *sharedMemory = &sMem;

    pthread_t tidSmoker;
    pthread_t tidAgent;
    pthread_attr_t attrSmoker;
    pthread_attr_t attrAgent;

    pthread_attr_init(&attrSmoker);
    pthread_attr_init(&attrAgent);
    pthread_create(&tidSmoker, &attrSmoker, smoker, NULL);
    pthread_create(&tidAgent, &attrAgent, agent, NULL);

    pthread_join(tidSmoker, NULL);
    pthread_join(tidAgent, NULL);
}