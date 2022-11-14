#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>
#include <tuple>

using namespace std;

// access points for acquire and release
enum Mutex {
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
    int readerMutex = 1;
    int writerMutex = 1;
    int criticalSection = 1;
    queue<tuple<Type, string *>> readers;
    queue<tuple<Type, string *>> writers;
    queue<tuple<Type, string *>> peopleInDatabase;
    int numOfReaders = 0;
    int numOfWriters = 0;
    int numOfPeopleInDatabase = 0;
    int numOfReadersInDatabase = 0;
    int numOfWritersInDatabase = 0;
} sMem;

void acquire(SharedMemory *sharedMemory, Mutex toAccess) {
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
            if(sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
            break;
    }
}

void release(SharedMemory *sharedMemory, Mutex toAccess) {
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
            if (sharedMemory->criticalSection == 0) {
                sharedMemory->criticalSection = 1;
            }
            break;
    }
}

void addReader(SharedMemory *sharedMemory, string *name) {
        tuple<Type, string*> newReader = make_tuple(reader, name);
        sharedMemory->numOfReaders++;

        // add new reader to the reader queue
        sharedMemory->readers.push(newReader);
        cout << *name + " (Type: " + typeOutput[get<0>(newReader)] + ") has been added\n";
        cout.flush();
}

tuple<Type, string*> removeReader(SharedMemory *sharedMemory) {
    tuple<Type, string*> retrievedReader = sharedMemory->readers.front();
    sharedMemory->numOfReaders--;

    // remove reader from the reader queue
    sharedMemory->readers.pop();
    return retrievedReader;
}

void addWriter(SharedMemory *sharedMemory, string *name) {
        tuple<Type, string*> newWriter = make_tuple(writer, name);
        sharedMemory->numOfWriters++;

        // add writer to the writer queue
        sharedMemory->writers.push(newWriter);
        cout << *name + " (Type: " + typeOutput[get<0>(newWriter)] + ") has been added\n";
        cout.flush();
}

tuple<Type, string*> removeWriter(SharedMemory *sharedMemory) {
    tuple<Type, string*> retrievedWriter = sharedMemory->writers.front();
    sharedMemory->numOfWriters--;

    // remove writer from the writer queue
    sharedMemory->writers.pop();
    return retrievedWriter;

}

void enterDatabase(SharedMemory *sharedMemory, tuple<Type, string*> person) {
    if (get<0>(person) == reader) {
        if(sharedMemory->writerMutex == 1) {
            acquire(sharedMemory, readerMut);
            sharedMemory->numOfReadersInDatabase++;
            cout << *(get<1>(person)) + " (Type: " + typeOutput[get<0>(person)] + ") HAS ENTERED THE DATABASE\n";
            cout.flush();
            sharedMemory->numOfPeopleInDatabase++;

            // add reader to the database queue if there are no writers in the database
            sharedMemory->peopleInDatabase.push(person);
        }
    }

    if (get<0>(person) == writer) {
        if(sharedMemory->readerMutex == 1 && sharedMemory->writerMutex == 1) {
            acquire(sharedMemory, writerMut);
            sharedMemory->numOfWritersInDatabase++;
            cout << *(get<1>(person)) + "(Type: " + typeOutput[get<0>(person)] + ") HAS ENTERED THE DATABASE\n";
            cout.flush();
            sharedMemory->numOfPeopleInDatabase++;

            // add writer to the database queue if there are no readers or writers in the database
            sharedMemory->peopleInDatabase.push(person);
        }
    }
}

void leaveDatabase(SharedMemory *sharedMemory) {
        tuple<Type, string*> personLeaving = sharedMemory->peopleInDatabase.front();
        if(get<0>(personLeaving) == reader) {

            // remove reader from the database queue
            sharedMemory->peopleInDatabase.pop();
            sharedMemory->numOfReadersInDatabase--;
            cout << *(get<1>(personLeaving)) + " (Type: " + typeOutput[get<0>(personLeaving)] + ") HAS LEFT THE DATABASE\n";
            cout.flush();
            sharedMemory->numOfPeopleInDatabase--;
            if (sharedMemory->numOfReadersInDatabase == 0) {
                release(sharedMemory, readerMut);
            }
        }

        if(get<0>(personLeaving) == writer) {

            // remove writer from the database queue
            sharedMemory->peopleInDatabase.pop();
            sharedMemory->numOfWritersInDatabase--;
            cout << *(get<1>(personLeaving)) + " (Type: " + typeOutput[get<0>(personLeaving)] + ") HAS LEFT THE DATABASE\n";
            cout.flush();
            sharedMemory->numOfPeopleInDatabase--;
            release(sharedMemory, writerMut);
        }
}

void* database(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    while(true) {
        // check for readers/writers in the database every two seconds
        this_thread::sleep_for(chrono::seconds(2));

        // if there is someone in the database, pop them out
        if(memory->numOfPeopleInDatabase > 0) {
            leaveDatabase(memory);
        }
    }

    return NULL;
}

void* producer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    while(true) {
        if(memory->criticalSection == 1) {
            if(memory->writerMutex == 1 && memory->numOfReaders > 0) {
                acquire(memory, criticalSection);
                tuple<Type, string*> readerEnteringDatabase = removeReader(memory);

                //reader can enter database if there are no writers in the database
                enterDatabase(memory, readerEnteringDatabase);
                release(memory, criticalSection);
            }
            else if(memory->readerMutex == 1 && memory->writerMutex == 1 && memory->numOfWriters > 0) {
                acquire(memory, criticalSection);
                tuple<Type, string*> writerEnteringDatabase = removeWriter(memory);

                // writer can enter the database if there are no readers or writers in the database
                enterDatabase(memory, writerEnteringDatabase);
                release(memory, criticalSection);
            }
        }

    }
}

void addPeople(SharedMemory* sharedMemory, string *students) {
    bool run1 = true;
    while(run1) {
        if(sharedMemory->criticalSection == 1) {
            acquire(sharedMemory, criticalSection);
            addReader(sharedMemory, &students[24]);
            addWriter(sharedMemory, &students[18]);
            addWriter(sharedMemory, &students[19]);
            addReader(sharedMemory, &students[0]);
            addReader(sharedMemory, &students[1]);
            addReader(sharedMemory, &students[2]);
            addWriter(sharedMemory, &students[3]);
            addWriter(sharedMemory, &students[4]);
            addWriter(sharedMemory, &students[5]);
            run1 = false;
        }
    }
    release(sharedMemory, criticalSection);

    // wait before entering more people
    this_thread::sleep_for(chrono::seconds(5));

    bool run2 = true;
    while(run2) {
        if(sharedMemory->criticalSection == 1) {
            acquire(sharedMemory, criticalSection);
            addReader(sharedMemory, &students[10]);
            addWriter(sharedMemory, &students[11]);
            addReader(sharedMemory, &students[12]);
            addWriter(sharedMemory, &students[13]);
            addReader(sharedMemory, &students[14]);
            addWriter(sharedMemory, &students[15]);
            run2 = false;
        }
    }
    release(sharedMemory, criticalSection);
}



int main() {
    cout << "\nNOTE: Threads will continue to run after last reader/writer leaves\n";
    cout << "After last reader/writer leaves, program will hang\n\n";

    SharedMemory *sharedMemory = &sMem;

    string people[26] = {"Alpha", "Bravo", "Charlie", "Delta", "Echo",
                           "Foxtrot", "Golf", "Hotel", "India", "Juliet",
                           "Kilo", "Lima", "Mike", "November", "Oscar",
                           "Papa", "Quebec", "Romeo", "Sierra", "Tango",
                           "Uniform", "Victor", "Whisky", "X-Ray", "Yankee",
                           "Zulu"};



    // two threads, consumer=database, producer=producer
    pthread_t tidDatabase;
    pthread_t tidProducer;
    pthread_attr_t attrDatabase;
    pthread_attr_t attrProducer;

    pthread_attr_init(&attrDatabase);
    pthread_attr_init(&attrProducer);
    pthread_create(&tidDatabase, &attrDatabase, database, sharedMemory);
    pthread_create(&tidProducer, &attrProducer, producer, sharedMemory);

    addPeople(sharedMemory, people);

    pthread_join(tidDatabase, NULL);
    pthread_join(tidProducer, NULL);

    return 0;
}