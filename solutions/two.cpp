#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>

using namespace std;

enum Access {
        //smokerOneMutex,
        //smokerTwoMutex,
        //smokerThreeMutex,
        smokerMutex,
        agentMutex,
        criticalSection
};

enum Ingredient {
    Tobacco,
    Paper,
    Matches
};

Ingredient ingredientMapping[3] = {Tobacco, Paper, Matches};
string ingredientOutput[3] = {"Tobacco", "Paper", "Matches"};

struct SharedMemory {
    //int smokerOneMutex = 0;
    //int smokerTwoMutex = 0;
    //int smokerThreeMutex = 0;
    int smokerMutex = 0;
    int agentMutex = 0;
    int criticalSection = 0;
    Ingredient smokerOne;
    Ingredient smokerTwo;
    Ingredient smokerThree;
    Ingredient agentIngredients[2];
} sMem;

void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch (toAccess) {
        //case smokerOneMutex:
        //    if (sharedMemory->smokerOneMutex == 0) {
        //        sharedMemory->smokerOneMutex = 1;
        //    }
        //case smokerTwoMutex:
        //    if (sharedMemory->smokerTwoMutex == 0) {
        //        sharedMemory->smokerTwoMutex = 1;
        //    }
        //case smokerThreeMutex:
        //    if (sharedMemory->smokerThreeMutex == 0) {
        //        sharedMemory->smokerThreeMutex = 1;
        //    }
        case smokerMutex:
            if (sharedMemory->smokerMutex == 0) {
                sharedMemory->smokerMutex = 1;
            }
        case agentMutex:
            if (sharedMemory->agentMutex == 0) {
                sharedMemory->agentMutex = 1;
            }
        case criticalSection:
            if (sharedMemory->criticalSection == 0) {
                sharedMemory->criticalSection = 1;
            }
    }
}

void signal(SharedMemory *sharedMemory, Access toAccess) {
    switch (toAccess) {
        //case smokerOneMutex:
        //    if (sharedMemory->smokerOneMutex == 1) {
        //        sharedMemory->smokerOneMutex = 0;
        //    }
        //case smokerTwoMutex:
        //    if (sharedMemory->smokerTwoMutex == 1) {
        //        sharedMemory->smokerTwoMutex = 0;
        //    }
        //case smokerThreeMutex:
        //    if (sharedMemory->smokerThreeMutex == 1) {
        //        sharedMemory->smokerThreeMutex = 0;
        //    }
        case smokerMutex:
            if (sharedMemory->smokerMutex == 1) {
                sharedMemory->smokerMutex = 0;
            }
        case agentMutex:
            if (sharedMemory->agentMutex == 1) {
                sharedMemory->agentMutex = 0;
            }
        case criticalSection:
            if (sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
    }
}



int getRandomIngredientIndex(int min, int max) {
    srand(time(NULL));
    return rand() % (max+1) + min;
}

void setSmokerIngredients(SharedMemory *sharedMemory) {
    //ingredient array
    Ingredient ingredient[3];

    //set first ingredient
    int ingredientOneIndex = getRandomIngredientIndex(0,2);
    ingredient[0] = ingredientMapping[ingredientOneIndex];

    //set second ingredient
    Ingredient remainingIngredients[2];
    int index = 0;
    for (int i = 0; i < 3; ++i) {
        if (i != ingredientOneIndex) {
            remainingIngredients[index] = ingredientMapping[i];
            index++;
        }
    }
    int ingredientTwoIndex = getRandomIngredientIndex(0,1);
    ingredient[1] = remainingIngredients[ingredientTwoIndex];

    //set third ingredient
    Ingredient reminingIngredient;
    for(int i = 0; i < 3; ++i) {
        if (ingredientMapping[i] != ingredient[0] && ingredientMapping[i] != ingredient[1]) {
            ingredient[2] = ingredientMapping[i];
        }
    }

    //set smoker ingredients
    sharedMemory->smokerOne = ingredient[0];
    sharedMemory->smokerTwo = ingredient[1];
    sharedMemory->smokerThree = ingredient[2];
}

void setTwoIngredients(SharedMemory *sharedMemory) {
    //set first ingredient
    int ingredientOneIndex = getRandomIngredientIndex(0,2);
    sharedMemory->agentIngredients[0] = ingredientMapping[ingredientOneIndex];

    //set second ingredient
    Ingredient remainingIngredients[2];
    int index = 0;
    for (int i = 0; i < 3; ++i) {
        if (i != ingredientOneIndex) {
            remainingIngredients[index] = ingredientMapping[i];
            index++;
        }
    }
    int ingredientTwoIndex = getRandomIngredientIndex(0,1);
    sharedMemory->agentIngredients[1] = remainingIngredients[ingredientTwoIndex];
}

void* smokerOne(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;

    //while (memory->criticalSection == 1) {
    //    ;
    //}
    //wait(memory, criticalSection);
    bool smokerFinished = false;
    //signal(memory, criticalSection);

    while (!smokerFinished) {
        if (memory->smokerMutex == 0) {
            if (memory->smokerOne != memory->agentIngredients[0] and memory->smokerOne != memory->agentIngredients[1]) {
                cout << "Smoker one started smoking" << endl;
                this_thread::sleep_for(chrono::seconds(2));
                cout << "Smoker one finished smoking" << endl;
                smokerFinished = true;
                wait(memory, smokerMutex);
                signal(memory, agentMutex);
            }
        }
    }

    return NULL;
}

void* smokerTwo(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;

    //while (memory->criticalSection == 1) {
    //    ;
    //}
    //wait(memory, criticalSection);
    bool smokerFinished = false;
    //signal(memory, criticalSection);

    while (!smokerFinished) {
        if (memory->smokerMutex == 0) {
            if (memory->smokerTwo != memory->agentIngredients[0] and memory->smokerTwo != memory->agentIngredients[1]) {
                cout << "Smoker two started smoking" << endl;
                this_thread::sleep_for(chrono::seconds(2));
                cout << "Smoker two finished smoking" << endl;
                smokerFinished = true;
                wait(memory, smokerMutex);
                signal(memory, agentMutex);
            }
        }
    }

    return NULL;
}

void* smokerThree(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;

    //while (memory->criticalSection == 1) {
    //    ;
    //}
    //wait(memory, criticalSection);
    bool smokerFinished = false;
    //signal(memory, criticalSection);

    while (!smokerFinished) {
        if (memory->smokerMutex == 0) {
            if (memory->smokerThree != memory->agentIngredients[0] and memory->smokerThree != memory->agentIngredients[1]) {
                cout << "Smoker three started smoking" << endl;
                this_thread::sleep_for(chrono::seconds(2));
                cout << "Smoker three finished smoking" << endl;
                smokerFinished = true;
                wait(memory, smokerMutex);
                signal(memory, agentMutex);
            }
        }
    }

    return NULL;
}

void* agent(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;
    while (memory->criticalSection == 1) {
        ;
    }
    //wait(memory, criticalSection);
    cout << "agent started" << endl;
    //signal(memory, criticalSection);

    while (true) {
        if (memory->agentMutex == 0) {
            wait(memory, agentMutex);
            setTwoIngredients(memory);
            cout << "Agent has placed: " + ingredientOutput[memory->agentIngredients[0]]
                    + " and " + ingredientOutput[memory->agentIngredients[1]] << endl;

            signal(memory, smokerMutex);
        }
    }

    return NULL;
}

int main() {
    SharedMemory *sharedMemory = &sMem;

    //get random ingredient indices
    srand(time(NULL));
    setSmokerIngredients(sharedMemory);

    //display what each smoker has
    cout << "Smoker one has " + ingredientOutput[sharedMemory->smokerOne] << endl;
    cout << "Smoker two has " + ingredientOutput[sharedMemory->smokerTwo] << endl;
    cout << "Smoker three has " + ingredientOutput[sharedMemory->smokerThree] << endl;

    //setTwoIngredients(sharedMemory);

    pthread_t tidSmokerOne;
    pthread_t tidSmokerTwo;
    pthread_t tidSmokerThree;
    pthread_t tidAgent;
    pthread_attr_t attrSmokerOne;
    pthread_attr_t attrSmokerTwo;
    pthread_attr_t attrSmokerThree;
    pthread_attr_t attrAgent;

    pthread_attr_init(&attrSmokerOne);
    pthread_attr_init(&attrSmokerTwo);
    pthread_attr_init(&attrSmokerThree);
    pthread_attr_init(&attrAgent);
    pthread_create(&tidSmokerOne, &attrSmokerOne, smokerOne, sharedMemory);
    pthread_create(&tidSmokerTwo, &attrSmokerTwo, smokerTwo, sharedMemory);
    pthread_create(&tidSmokerThree, &attrSmokerThree, smokerThree, sharedMemory);
    pthread_create(&tidAgent, &attrAgent, agent, sharedMemory);

    pthread_join(tidSmokerOne, NULL);
    pthread_join(tidSmokerTwo, NULL);
    pthread_join(tidSmokerThree, NULL);
    pthread_join(tidAgent, NULL);

    return 0;
}