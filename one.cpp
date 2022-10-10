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

void setNumChairs(SharedMemory *sharedMemory, int numChairs) {
    sharedMemory->numOfChairs = numChairs;
    sharedMemory->customersInShop = vector<string *>(sharedMemory->numOfChairs);
}

void cutHair(SharedMemory *sharedMemory) {
    cout << (*sharedMemory->customersInShop.back()) << endl;

}

void enterBarberShop(SharedMemory *sharedMemory, string *customer) {
    sharedMemory->customersInShop.push_back(customer);
}

void* barber(void *sharedMemory) {
    cutHair(((struct SharedMemory*)sharedMemory));
}

void* producer(void *sharedMemory) {
    string first = "Andrew";
    enterBarberShop((struct SharedMemory*)sharedMemory, &first);
}

int main() {
    SharedMemory sharedMemory;
    setNumChairs(&sharedMemory, 3);

    //string first = "Andrew";
    //sharedMemory.customersInShop.push_back(&first);
    //enterBarberShop(&sharedMemory, &first);

    pthread_t tidBarber;
    pthread_t tidProducer;
    pthread_attr_t attrBarber;
    pthread_attr_t attrProducer;
    pthread_attr_init(&attrBarber);
    pthread_attr_init(&attrProducer);
    pthread_create(&tidBarber, &attrBarber, barber, &sharedMemory);
    pthread_create(&tidProducer, &attrProducer, producer, &sharedMemory);


    pthread_join(tidBarber, NULL);
    pthread_join(tidProducer, NULL);

    //string customer= "test";
    //sharedMemory.customersInShop.push_back(&customer);
    //string *thing = sharedMemory.customersInShop.back();
    //cout << *thing << endl;

}