#include <iostream>
#include <thread>
#include <chrono>
#include <list>
#include <string>

using namespace std;

enum Access {chairSem, barberMut};

int barberMutex = 0;
int chairSemaphore = 0;
int numOfChairs = 0;
list<string> customersInShop;

void setChairNum(int numChair) {
    numOfChairs = numChair;
}

void wait(Access toAccess) {
    switch(toAccess) {
        case chairSem:
            if (chairSemaphore < numOfChairs) {
                chairSemaphore++;
            }
        case barberMut:
            if (barberMutex == 0) {
                barberMutex = 1;
            }
    }
}

void signal(Access toAccess) {
    switch(toAccess) {
        case chairSem:
            if (chairSemaphore > 0) {
                chairSemaphore--;
            }
        case barberMut:
            if (barberMutex == 1) {
                barberMutex = 0;
            }
    }
}

void enterShop(string *customerNum) {
    if (chairSemaphore < numOfChairs) {
        wait(chairSem);
        customersInShop.push_back(*customerNum);
        cout << *customerNum + " has entered the barbershop" << endl;
    }
}

void leaveShop(string *customerNum) {
    customersInShop.remove(*customerNum);
    signal(chairSem);
}

void cutHair(string *customerNum) {
    wait(chairSem);
    cout << "Beginning to cut " + *customerNum + "'s hair" << endl;
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Finished cutting " + *customerNum + "'s hair" << endl;
    leaveShop(customerNum);
    signal(chairSem);
}

void* barber(void *) {
    while(true) {
        if (chairSemaphore) {
            if (barberMutex) {
                cutHair(&customersInShop.front());
            }
        }
    }
}


int main() {
    setChairNum(2);


    pthread_t pbarber;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&pbarber, &attr, barber, NULL);

    string customerOne = "Robert";
    enterShop(&customerOne);

    this_thread::sleep_for(chrono::seconds(5));
    string customerTen = "John";
    string customerEleven = "Sam";
    string customerTwelve = "Tim";
    enterShop(&customerTen);
    enterShop(&customerEleven);
    enterShop(&customerTwelve);

    pthread_join(pbarber, NULL);

    cout << "Done" << endl;

}