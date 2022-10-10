#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

enum Access {chairSem, barberMut};

struct SharedMemory {
    int barberMutex = 0;
    int chairSemaphore = 0;
    int numOfChairs = 0;
    vector<string *> customersInShop;
};

void wait(SharedMemory *sharedMemory, Access toAccess) {
    SharedMemory *memory = (struct SharedMemory*) sharedMemory;
    switch(toAccess) {
        case chairSem:
            if (memory->chairSemaphore < memory->numOfChairs) {
                memory->chairSemaphore++;
            }
            break;
        case barberMut:
            if (memory->barberMutex == 0) {
                memory->barberMutex = 1;
            }
            break;
    }
}

void signal(SharedMemory *sharedMemory, Access toAccess) {
    SharedMemory *memory = (struct SharedMemory*) sharedMemory;
    switch(toAccess) {
        case chairSem:
            if(memory->chairSemaphore > 0) {
                memory->chairSemaphore--;
            }
        case barberMut:
            if(memory->barberMutex == 1) {
                memory->barberMutex = 0;
            }
    }
}

void setNumChairs(SharedMemory *sharedMemory, int numChairs) {
    sharedMemory->numOfChairs = numChairs;
    sharedMemory->customersInShop = vector<string *>(sharedMemory->numOfChairs);
}

void cutHair(SharedMemory *sharedMemory) {
    cout << (*sharedMemory->customersInShop.back()) << endl;

}

void enterBarberShop(SharedMemory *sharedMemory, string *customer) {
    //wait(sharedMemory, chairSem);
    //sharedMemory->customersInShop.push_back(customer);
}

void* barber(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory*) sharedMemory;
    while (true) {
        if (memory->chairSemaphore > 0 && memory->barberMutex == 0) {
            wait(memory, barberMut);
            cout << *(memory->customersInShop.back()) << endl;
            memory->customersInShop.pop_back();
            signal(memory, barberMut);
            signal(memory, chairSem); // Need to implement leave shop function
        }
    }
    //cutHair(((struct SharedMemory*)sharedMemory));
}

void* producer(void *sharedMemory) {
    string first = "Andrew";
    //enterBarberShop((struct SharedMemory*)sharedMemory, &first);
    return NULL;
}

int main() {
    SharedMemory sharedMemory;
    setNumChairs(&sharedMemory, 3);


    string first = "Andrew";
    sharedMemory.customersInShop.push_back(&first);
    wait(&sharedMemory, chairSem);
    //enterBarberShop(&sharedMemory, &first);

    pthread_t tidBarber;
    pthread_t tidProducer;
    pthread_attr_t attrBarber;
    pthread_attr_t attrProducer;

    pthread_attr_init(&attrBarber);
    pthread_attr_init(&attrProducer);
    pthread_create(&tidBarber, &attrBarber, barber, &sharedMemory);
    //pthread_create(&tidBarber, &attrProducer, producer, &sharedMemory);


    pthread_join(tidBarber, NULL);
    //pthread_join(tidProducer, NULL);

    //string customer= "test";
    //sharedMemory.customersInShop.push_back(&customer);
    //string *thing = sharedMemory.customersInShop.back();
    //cout << *thing << endl;

}