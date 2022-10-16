#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>

using namespace std;

enum Access {
    chairSem,
    taMut
};

struct SharedMemory {
    int taMutex = 0;
    int chairSemaphore = 0;
    int numOfChair = 3;
    queue<string *> studentsWaiting;
} sMem;

void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch (toAccess) {
        case chairSem:
            break;
        case taMut:
            break;
    }
}

void* ta(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    return NULL;
}

void* producer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    return NULL;
}

int main() {

    SharedMemory *sharedMemory = &sMem;

    pthread_t tidTa;
    pthread_t tidProducer;
    pthread_attr_t attrTa;
    pthread_attr_t attrProducer;

    pthread_attr_init(&attrTa);
    pthread_attr_init(&attrProducer);
    pthread_create(&tidTa, &attrTa, ta, sharedMemory);
    pthread_create(&tidProducer, &attrProducer, producer, sharedMemory);

    pthread_join(tidTa, NULL);
    pthread_join(tidProducer, NULL);


    return 0;
}