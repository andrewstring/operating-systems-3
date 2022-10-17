#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>

using namespace std;

enum Access {
    readerSem,
    writerSem,
    criticalSection
};

struct SharedMemory {
    int readerSemaphore = 0;
    int writerSemaphore = 0;
    int criticalSection = 0;
    queue<string *> readers;
    queue<string *> writers;

} sMem;

void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch(toAccess) {
        case readerSem:
            sharedMemory->readerSemaphore++;
            break;
        case writerSem:
            sharedMemory->writerSemaphore++;
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
        case readerSem:
            if(sharedMemory->readerSemaphore > 0) {
                sharedMemory->readerSemaphore--;
            }
            break;
        case writerSem:
            if(sharedMemory->writerSemaphore > 0) {
                sharedMemory->writerSemaphore--;
            }
            break;
        case criticalSection:
            if(sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
    }
}

void *consumer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    return NULL;
}

void *readerProducer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    return NULL;
}

void *writerProducer(void *sharedMemory) {
    SharedMemory *memory = (struct SharedMemory *) sharedMemory;

    return NULL;
}

int main() {
    SharedMemory *sharedMemory = &sMem;

    pthread_t tidConsumer;
    pthread_t tidReaderProducer;
    pthread_t tidWriterProducer;
    pthread_attr_t attrConsumer;
    pthread_attr_t attrReaderProducer;
    pthread_attr_t attrWriterProducer;

    pthread_attr_init(&attrConsumer);
    pthread_attr_init(&attrReaderProducer);
    pthread_attr_init(&attrWriterProducer);
    pthread_create(&tidConsumer, &attrConsumer, consumer, sharedMemory);
    pthread_create(&tidReaderProducer, &attrReaderProducer, readerProducer, sharedMemory);
    pthread_create(&tidWriterProducer, &attrWriterProducer, writerProducer, sharedMemory);

    pthread_join(tidConsumer, NULL);
    pthread_join(tidReaderProducer, NULL);
    pthread_join(tidWriterProducer, NULL);

    return 0;
}