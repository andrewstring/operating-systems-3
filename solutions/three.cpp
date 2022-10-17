#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>

using namespace std;

enum Access {
    chairHallwaySem,
    chairTaSem,
    taMut,
    criticalSection
};

struct SharedMemory {
    int taMutex = 0;
    int chairTaSemaphore = 0;
    int chairHallwaySemaphore = 0;
    int numOfChairsHallway = 3;
    int numOfChairsTa = 1;
    int criticalSection = 0;
    queue<string *> studentsInHallway;
    queue<string *> studentWithTa;
} sMem;

void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch (toAccess) {
        case chairHallwaySem:
            if (sharedMemory->chairHallwaySemaphore < sharedMemory->numOfChairsHallway) {
                sharedMemory->chairHallwaySemaphore++;
            }
            break;
        case chairTaSem:
            if (sharedMemory->chairTaSemaphore < sharedMemory->numOfChairsTa) {
                sharedMemory->chairTaSemaphore++;
            }
            break;
        case taMut:
            if (sharedMemory->taMutex == 0) {
                sharedMemory->taMutex = 1;
            }
            break;
        case criticalSection:
            if (sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
    }
}

void signal(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case chairHallwaySem:
            if (sharedMemory->chairHallwaySemaphore > 0) {
                sharedMemory->chairHallwaySemaphore--;
            }
            break;
        case chairTaSem:
            if (sharedMemory->chairTaSemaphore > 0) {
                sharedMemory->chairTaSemaphore--;
            }
            break;
        case taMut:
            if (sharedMemory->taMutex == 1) {
                sharedMemory->taMutex = 0;
            }
            break;
        case criticalSection:
            if (sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
    }
}

void enterHallway(SharedMemory *sharedMemory, string *student) {
    wait(sharedMemory, criticalSection);
    if (sharedMemory->chairHallwaySemaphore >= sharedMemory->numOfChairsHallway) {
        cout << "Hallway is full..." + *student + " did not enter" << endl;
    }
    else {
        wait(sharedMemory, chairHallwaySem);
        sharedMemory->studentsInHallway.push(student);
        cout << *student + " has sat down in the hallway" << endl;
    }
    signal(sharedMemory, criticalSection);
}

string* enterTaOffice(SharedMemory *sharedMemory) {
    wait(sharedMemory, criticalSection);
    if (sharedMemory->chairTaSemaphore < sharedMemory->numOfChairsTa) {
        wait(sharedMemory, chairTaSem);
        string *studentFromHallway = sharedMemory->studentsInHallway.front();
        sharedMemory->studentsInHallway.pop();
        signal(sharedMemory, chairHallwaySem);
        sharedMemory->studentWithTa.push(studentFromHallway);
        cout << *studentFromHallway + " has entered TA's office" << endl;
        return studentFromHallway;
    }
    signal(sharedMemory, criticalSection);
}

void taHelpStudent(SharedMemory *sharedMemory) {
    string *student = enterTaOffice(sharedMemory);
    cout << "TA started helping " + *student << endl;
    this_thread::sleep_for(chrono::seconds(2));
    cout << "TA finished helping " + *student << endl;
    sharedMemory->studentWithTa.pop();
    signal(sharedMemory, taMut);
    cout << *student + " has left the TA's office" << endl;
}

void* ta(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;
    while(true) {
        if(memory->criticalSection == 0) {
            if (memory->chairHallwaySemaphore > 0 && memory->chairTaSemaphore < memory->numOfChairsTa) {
                taHelpStudent(memory);
            }
        }
    }

    return NULL;
}

void* producer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;
    string students[26] = {"Alpha", "Bravo", "Charlie", "Delta", "Echo",
                         "Foxtrot", "Golf", "Hotel", "India", "Juliet",
                         "Kilo", "Lima", "Mike", "November", "Oscar",
                         "Papa", "Quebec", "Romeo", "Sierra", "Tango",
                         "Uniform", "Victor", "Whisky", "X-Ray", "Yankee",
                         "Zulu"};

    enterHallway(memory, &students[0]);
    enterHallway(memory, &students[1]);
    enterHallway(memory, &students[2]);
    enterHallway(memory, &students[3]);
    enterHallway(memory, &students[4]);
    this_thread::sleep_for(chrono::seconds(7));
    enterHallway(memory, &students[5]);

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