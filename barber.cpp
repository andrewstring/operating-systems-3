#include <iostream>
#include <thread>
#include <chrono>
#include <list>
#include <string>

using namespace std;

enum Access {chairSem, barberMut};


struct SharedMemory {
    int barberMutex = 0;
    int chairSemaphore = 0;
    int numOfChairs = 0;
    list<string> customersInShop;
};




void setChairNum(SharedMemory sharedMemory, int numChair) {
    sharedMemory.numOfChairs = numChair;
}

void wait(SharedMemory sharedMemory, Access toAccess) {
    switch(toAccess) {
        case chairSem:
            if (sharedMemory.chairSemaphore < sharedMemory.numOfChairs) {
                sharedMemory.chairSemaphore++;
            }
        case barberMut:
            if (sharedMemory.barberMutex == 0) {
                sharedMemory.barberMutex = 1;
            }
    }
}

void signal(SharedMemory sharedMemory, Access toAccess) {
    switch(toAccess) {
        case chairSem:
            if (sharedMemory.chairSemaphore > 0) {
                sharedMemory.chairSemaphore--;
            }
        case barberMut:
            if (sharedMemory.barberMutex == 1) {
                sharedMemory.barberMutex = 0;
            }
    }
}

void enterShop(SharedMemory *sharedMemory, string customerNum) {
    if ((*sharedMemory).chairSemaphore < (*sharedMemory).numOfChairs) {
        wait((*sharedMemory), chairSem);
        (*sharedMemory).customersInShop.push_back(customerNum);
        cout << customerNum + " has entered the barbershop" << endl;
    }
}

void leaveShop(SharedMemory *sharedMemory, string customerNum) {
    (*sharedMemory).customersInShop.remove(customerNum);
    signal(*sharedMemory, chairSem);
}

void cutHair(SharedMemory *sharedMemory, string customerNum) {
    wait(*sharedMemory, chairSem);
    cout << "Beginning to cut " + customerNum + "'s hair" << endl;
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Finished cutting " + customerNum + "'s hair" << endl;
    leaveShop(sharedMemory, customerNum);
    signal(*sharedMemory, chairSem);
}

void* barber(void *sharedMemory) {
    while(true) {
        if (((struct SharedMemory*) sharedMemory)->chairSemaphore) {
            if (((struct SharedMemory *) sharedMemory)->barberMutex) {
                cutHair(((struct SharedMemory*) sharedMemory), ((struct SharedMemory*) sharedMemory)->customersInShop.front());
            }
        }
    }
}

void* producer(void *sharedMemory) {
    string customerOne = "Robert";
    enterShop(((struct SharedMemory*) sharedMemory), customerOne);
}


int main() {

    SharedMemory *sharedMemory;
    cout << (*sharedMemory).numOfChairs;



    pthread_t pbarber, pproducer;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&pbarber, &attr, barber, sharedMemory);
    //pthread_create(&pproducer, &attr, producer, sharedMemory);

    //string customerOne = "Robert";
    //enterShop(&customerOne);

    //this_thread::sleep_for(chrono::seconds(5));
    //string customerTen = "John";
    //string customerEleven = "Sam";
    //string customerTwelve = "Tim";
    //enterShop(&customerTen);
    //enterShop(&customerEleven);
    //enterShop(&customerTwelve);


    string customerOne = "John";
    enterShop(sharedMemory, customerOne);

    pthread_join(pbarber, NULL);
    //pthread_join(pproducer, NULL);



    cout << "Done" << endl;

}