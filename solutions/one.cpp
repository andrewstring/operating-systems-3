#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

enum Access {chairSem, barberMut};

struct SharedMemory {
    int barberMutex = 0;
    int chairSemaphore = 0;
    int numOfChairs = 0;
    queue<string *> customersInShop;
} sMem;



void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case chairSem:
            if (sharedMemory->chairSemaphore < sharedMemory->numOfChairs) {
                sharedMemory->chairSemaphore++;
            }
            break;
        case barberMut:
            if (sharedMemory->barberMutex == 0) {
                sharedMemory->barberMutex = 1;
            }
            break;
    }
}

void signal(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case chairSem:
            if(sharedMemory->chairSemaphore > 0) {
                sharedMemory->chairSemaphore--;
            }
            break;
        case barberMut:
            if(sharedMemory->barberMutex == 1) {
                sharedMemory->barberMutex = 0;
            }
            break;
    }
}

void setNumChairs(SharedMemory *sharedMemory, int numChairs) {
    sharedMemory->numOfChairs = numChairs;
}

void cutHair(SharedMemory *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory*) sharedMemory;
    wait(sharedMemory, barberMut);
    string customer = *(memory->customersInShop.front());
    cout << "Started cutting " + customer + "'s hair" << endl;
    this_thread::sleep_for(chrono::seconds(2));
    memory->customersInShop.pop();
    cout << "Finished cutting " + customer + "'s hair" << endl;
    signal(sharedMemory, barberMut);

}

void enterBarberShop(SharedMemory *sharedMemory, string *customer) {
    wait(sharedMemory,chairSem);
    sharedMemory->customersInShop.push(customer);
}

void* barber(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory*) sharedMemory;
    while (true) {
        if (memory->chairSemaphore > 0 && memory->barberMutex == 0) {
            cutHair(memory);
            signal(memory, chairSem); // Need to implement leave shop function
        }
    }
    //cutHair(((struct SharedMemory*)sharedMemory));
}

void* producer(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;
    string first = "Andrew";
    string second = "Bob";
    string third = "Tim";
    string fourth = "Jim";
    enterBarberShop(memory, &first);
    enterBarberShop(memory, &second);
    this_thread::sleep_for(chrono::seconds(5));
    enterBarberShop(memory, &third);
    enterBarberShop(memory, &fourth);

}

int main() {

    SharedMemory *sharedMemory = &sMem;



    setNumChairs(sharedMemory, 3);


    //string first = "Andrew";
    //sharedMemory->customersInShop.push(&first);
    //wait(chairSem);
    //enterBarberShop(sharedMemory, &first);

    pthread_t tidBarber;
    pthread_t tidProducer;
    pthread_attr_t attrBarber;
    pthread_attr_t attrProducer;

    pthread_attr_init(&attrBarber);
    pthread_attr_init(&attrProducer);
    pthread_create(&tidBarber, &attrBarber, barber, sharedMemory);
    pthread_create(&tidProducer, &attrProducer, producer, sharedMemory);


    pthread_join(tidBarber, NULL);
    //pthread_join(tidProducer, NULL);

    //string customer= "test";
    //sharedMemory.customersInShop.push_back(&customer);
    //string *thing = sharedMemory.customersInShop.back();
    //cout << *thing << endl;

}
