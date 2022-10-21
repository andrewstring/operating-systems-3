#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>
#include <tuple>

using namespace std;

enum Access {
    readerMut,
    writerMut,
    criticalSection
};

enum Type {
    reader,
    writer
};

string typeOutput[2] = {"reader", "writer"};

struct SharedMemory {
    int readerMutex = 0;
    int writerMutex = 0;
    int criticalSection = 0;
    queue<tuple<Type, string *>> readers;
    queue<tuple<Type, string *>> writers;
    queue<tuple<Type, string *>> peopleInDatabase;
    int numOfReaders = 0;
    int numOfWriters = 0;
    int numOfPeopleInDatabase = 0;
    int numOfReadersInDatabase = 0;
    int numOfWritersInDatabase = 0;
} sMem;

void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case readerMut:
            if (sharedMemory->readerMutex == 0) {
                sharedMemory->readerMutex = 1;
            }
            break;
        case writerMut:
            if (sharedMemory->writerMutex == 0) {
                sharedMemory->writerMutex = 1;
            }
            break;
        case criticalSection:
            if(sharedMemory->criticalSection == 0) {
                sharedMemory->criticalSection = 1;
            }
            break;
    }
}

void signal(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case readerMut:
            if (sharedMemory->readerMutex == 1) {
                sharedMemory->readerMutex = 0;
            }
            break;
        case writerMut:
            if (sharedMemory->writerMutex == 1) {
                sharedMemory->writerMutex = 0;
            }
            break;
        case criticalSection:
            if (sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
            break;
    }
}
void addReader(SharedMemory *sharedMemory, string *name) {
        tuple<Type, string*> newReader = make_tuple(reader, name);
        sharedMemory->numOfReaders++;
        sharedMemory->readers.push(newReader);
        cout << *name + " (Type: " + typeOutput[get<0>(newReader)] + ") has been added\n";
        cout.flush();
}

tuple<Type, string*> removeReader(SharedMemory *sharedMemory) {
    tuple<Type, string*> retrievedReader = sharedMemory->readers.front();
    sharedMemory->numOfReaders--;
    sharedMemory->readers.pop();
    return retrievedReader;
}

void addWriter(SharedMemory *sharedMemory, string *name) {
        tuple<Type, string*> newWriter = make_tuple(writer, name);
        sharedMemory->numOfWriters++;
        sharedMemory->writers.push(newWriter);
        cout << *name + " (Type: " + typeOutput[get<0>(newWriter)] + ") has been added\n";
        cout.flush();
}

tuple<Type, string*> removeWriter(SharedMemory *sharedMemory) {
    tuple<Type, string*> retrievedWriter = sharedMemory->writers.front();
    sharedMemory->numOfWriters--;
    sharedMemory->writers.pop();
    return retrievedWriter;

}

void enterDatabase(SharedMemory *sharedMemory, tuple<Type, string*> person) {
    if(sharedMemory->criticalSection == 0) {
       wait(sharedMemory, criticalSection);
        if (get<0>(person) == reader) {
            if(sharedMemory->writerMutex == 0) {
                wait(sharedMemory, readerMut);
                sharedMemory->numOfReadersInDatabase++;
                cout << *(get<1>(person)) + " (Type: " + typeOutput[get<0>(person)] + ") HAS ENTERED THE DATABASE\n";
                cout.flush();
                sharedMemory->numOfPeopleInDatabase++;
                sharedMemory->peopleInDatabase.push(person);
            }
        }

        if (get<0>(person) == writer) {
            if(sharedMemory->readerMutex == 0 && sharedMemory->writerMutex == 0) {
                wait(sharedMemory, writerMut);
                sharedMemory->numOfWritersInDatabase++;
                cout << *(get<1>(person)) + "(Type: " + typeOutput[get<0>(person)] + ") HAS ENTERED THE DATABASE\n";
                cout.flush();
                sharedMemory->numOfPeopleInDatabase++;
                sharedMemory->peopleInDatabase.push(person);
            }
        }
       signal(sharedMemory, criticalSection);
    }
}

void leaveDatabase(SharedMemory *sharedMemory) {
        tuple<Type, string*> personLeaving = sharedMemory->peopleInDatabase.front();
        if(get<0>(personLeaving) == reader) {
            sharedMemory->peopleInDatabase.pop();
            sharedMemory->numOfReadersInDatabase--;
            cout << *(get<1>(personLeaving)) + " (Type: " + typeOutput[get<0>(personLeaving)] + ") HAS LEFT THE DATABASE\n";
            cout.flush();
            sharedMemory->numOfPeopleInDatabase--;
            if (sharedMemory->numOfReadersInDatabase == 0) {
                signal(sharedMemory, readerMut);
            }
        }

        if(get<0>(personLeaving) == writer) {
            sharedMemory->peopleInDatabase.pop();
            sharedMemory->numOfWritersInDatabase--;
            cout << *(get<1>(personLeaving)) + " (Type: " + typeOutput[get<0>(personLeaving)] + ") HAS LEFT THE DATABASE\n";
            cout.flush();
            sharedMemory->numOfPeopleInDatabase--;
            signal(sharedMemory, writerMut);
        }
}

void* database(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    while(true) {
        this_thread::sleep_for(chrono::seconds(2));
        if(memory->numOfPeopleInDatabase > 0) {
            leaveDatabase(memory);
        }
    }

    return NULL;
}


void* readerProducer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    while(true) {
            if (memory->writerMutex == 0 && memory->numOfReaders > 0) {
                tuple<Type, string*> readerEnteringDatabase = removeReader(memory);
                enterDatabase(memory, readerEnteringDatabase);
            }
    }

    return NULL;
}

void* writerProducer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    while(true) {
            if(memory->readerMutex == 0 && memory->writerMutex == 0 && memory->numOfWriters > 0) {
                tuple<Type, string*> writerEnteringDatabase = removeWriter(memory);
                enterDatabase(memory, writerEnteringDatabase);

            }
    }

    return NULL;
}



int main() {
    SharedMemory *sharedMemory = &sMem;

    string students[26] = {"Alpha", "Bravo", "Charlie", "Delta", "Echo",
                           "Foxtrot", "Golf", "Hotel", "India", "Juliet",
                           "Kilo", "Lima", "Mike", "November", "Oscar",
                           "Papa", "Quebec", "Romeo", "Sierra", "Tango",
                           "Uniform", "Victor", "Whisky", "X-Ray", "Yankee",
                           "Zulu"};

    // add readers and writers
    addWriter(sharedMemory, &students[18]);
    addWriter(sharedMemory, &students[19]);

    addReader(sharedMemory, &students[0]);
    addReader(sharedMemory, &students[1]);
    addReader(sharedMemory, &students[2]);

    addWriter(sharedMemory, &students[3]);
    addWriter(sharedMemory, &students[4]);
    addWriter(sharedMemory, &students[5]);

    pthread_t tidDatabase;
    pthread_t tidReader;
    pthread_t tidWriter;
    pthread_attr_t attrDatabase;
    pthread_attr_t attrReader;
    pthread_attr_t attrWriter;

    pthread_attr_init(&attrDatabase);
    pthread_attr_init(&attrReader);
    pthread_attr_init(&attrWriter);
    pthread_create(&tidDatabase, &attrDatabase, database, sharedMemory);
    pthread_create(&tidReader, &attrReader, readerProducer, sharedMemory);
    pthread_create(&tidWriter, &attrWriter, writerProducer, sharedMemory);


    // wait, then add more readers and writers
    this_thread::sleep_for(chrono::seconds(5));

    addReader(sharedMemory, &students[10]);
    addWriter(sharedMemory, &students[11]);
    addReader(sharedMemory, &students[12]);

    addWriter(sharedMemory, &students[13]);
    addReader(sharedMemory, &students[14]);
    addWriter(sharedMemory, &students[15]);

    pthread_join(tidDatabase, NULL);
    pthread_join(tidReader, NULL);
    pthread_join(tidWriter, NULL);

    return 0;
}