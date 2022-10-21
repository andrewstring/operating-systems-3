#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>

using namespace std;

enum Access {chairSem,
        barberMut,
        criticalSection};

struct SharedMemory {
    int barberMutex = 0;
    int chairSemaphore = 0;
    int numOfChairs = 0;
    int criticalSection = 0;
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
        case criticalSection:
            if (sharedMemory->criticalSection == 0) {
                sharedMemory->criticalSection = 1;
            }
    }
}

void signal(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case chairSem:
            if (sharedMemory->chairSemaphore > 0) {
                sharedMemory->chairSemaphore--;
            }
            break;
        case barberMut:
            if (sharedMemory->barberMutex == 1) {
                sharedMemory->barberMutex = 0;
            }
            break;
        case criticalSection:
            if(sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
    }
}

void setNumChairs(SharedMemory *sharedMemory, int numChairs) {
    sharedMemory->numOfChairs = numChairs;
}

void enterBarberShop(SharedMemory *sharedMemory, string *customer) {
    wait(sharedMemory, criticalSection);
    if (sharedMemory->chairSemaphore >= sharedMemory->numOfChairs) {
        cout << "Barber shop is full..." + *customer + " did not enter\n";
        cout.flush();
    }
    else {
        sharedMemory->customersInShop.push(customer);
        cout << *customer + " has entered the barber shop\n";
        cout.flush();
        wait(sharedMemory,chairSem);
    }
    signal(sharedMemory, criticalSection);
}

void leaveBarberShop(SharedMemory *sharedMemory, string *customer) {
    signal(sharedMemory, barberMut);
    cout << *customer + " left the barber shop\n";
    cout.flush();
}

void cutHair(SharedMemory *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory*) sharedMemory;
    wait(sharedMemory, barberMut);
    string *customer = memory->customersInShop.front();
    cout << "Started cutting " + *customer + "'s hair\n";
    cout.flush();
    this_thread::sleep_for(chrono::seconds(2));
    memory->customersInShop.pop();
    cout << "Finished cutting " + *customer + "'s hair\n";
    cout.flush();
    leaveBarberShop(memory, customer);
}



void* barber(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;
    while (true) {
        if (memory->criticalSection == 0) {
            if (memory->chairSemaphore > 0 && memory->barberMutex == 0) {
                cutHair(memory);
                signal(memory, chairSem);
            }
        }
    }

    return NULL;
}

void* producer(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;
    bool run = true;

    while (true) {
        // only run producer code once
        if (run) {
            // SAMPLES: NEED TO FIGURE OUT TESTING FOR THIS
            string people[26] = {"Alpha", "Bravo", "Charlie", "Delta", "Echo",
                                "Foxtrot", "Golf", "Hotel", "India", "Juliet",
                                "Kilo", "Lima", "Mike", "November", "Oscar",
                                "Papa", "Quebec", "Romeo", "Sierra", "Tango",
                                "Uniform", "Victor", "Whisky", "X-Ray", "Yankee",
                                "Zulu"};
            enterBarberShop(memory, &people[0]);
            enterBarberShop(memory, &people[1]);
            enterBarberShop(memory, &people[2]);
            enterBarberShop(memory, &people[3]);
            enterBarberShop(memory, &people[4]);
            this_thread::sleep_for(chrono::seconds(3));
            enterBarberShop(memory, &people[5]);
            enterBarberShop(memory, &people[6]);
            enterBarberShop(memory, &people[7]);
            enterBarberShop(memory, &people[8]);
            this_thread::sleep_for(chrono::seconds(10));
            enterBarberShop(memory, &people[9]);

            //prevent from running again
            run = false;
        }
    }

    

    return NULL;
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
    pthread_join(tidProducer, NULL);

    //string customer= "test";
    //sharedMemory.customersInShop.push_back(&customer);
    //string *thing = sharedMemory.customersInShop.back();
    //cout << *thing << endl;

    return 0;

}
