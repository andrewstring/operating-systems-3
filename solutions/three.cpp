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
    int chairHallwaySemaphore = 0;
    int numOfChairsHallway = 3;
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
        case taMut:
            if (sharedMemory->taMutex == 0) {
                sharedMemory->taMutex = 1;
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
        case chairHallwaySem:
            if (sharedMemory->chairHallwaySemaphore > 0) {
                sharedMemory->chairHallwaySemaphore--;
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
        cout.flush();
    }
    else {
        wait(sharedMemory, chairHallwaySem);
        sharedMemory->studentsInHallway.push(student);
        cout << *student + " has sat down in the hallway" << endl;
        cout.flush();
    }
    signal(sharedMemory, criticalSection);
}

string* enterTaOffice(SharedMemory *sharedMemory) {
    //wait(sharedMemory, criticalSection);
    if (sharedMemory->taMutex == 0) {
        wait(sharedMemory, chairTaSem);
        string *studentFromHallway = sharedMemory->studentsInHallway.front();
        sharedMemory->studentsInHallway.pop();
        sharedMemory->studentWithTa.push(studentFromHallway);
        cout << *studentFromHallway + " has entered TA's office" << endl;
        signal(sharedMemory, chairHallwaySem);
        cout.flush();
        //signal(sharedMemory, criticalSection);
        return studentFromHallway;
    }
    else {
        //signal(sharedMemory, criticalSection);
        return NULL;
    }

}

void taHelpStudent(SharedMemory *sharedMemory) {
    string *student = enterTaOffice(sharedMemory);
    cout << "TA started helping " + *student << endl;
    cout.flush();
    this_thread::sleep_for(chrono::seconds(2));
    cout << "TA finished helping " + *student << endl;
    cout.flush();
    sharedMemory->studentWithTa.pop();
    signal(sharedMemory, taMut);
    cout << *student + " has left the TA's office" << endl;
    cout.flush();
}

void* ta(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;
    while(true) {
        if(memory->criticalSection == 0) {
            if (memory->chairHallwaySemaphore > 0 && memory->taMutex == 0) {
                taHelpStudent(memory);
            }
        }
    }

    return NULL;
}

void* producer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;
    bool run = true;

    while (true) {
        if (run) {
            //SAMPLES: NEED TO FIGURE OUT TESTING FOR THIS
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
            this_thread::sleep_for(chrono::seconds(6));
            enterHallway(memory, &students[5]);
            enterHallway(memory, &students[6]);
            enterHallway(memory, &students[7]);
            enterHallway(memory, &students[8]);
            enterHallway(memory, &students[9]);

            //prevent from running again
            run = false;
        }
    }
    
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