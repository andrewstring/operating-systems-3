#include <iostream>
#include <thread>
#include <chrono>
#include <string>

using namespace std;

enum Access {
        smokerMutex,
        agentMutex,
        criticalSection,
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
    int smokerMutex = 1;
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
            break;
        case agentMutex:
            if (sharedMemory->agentMutex == 0) {
                sharedMemory->agentMutex = 1;
            }
            break;
        case criticalSection:
            if (sharedMemory->criticalSection == 0) {
                sharedMemory->criticalSection = 1;
            }
            break;
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
            break;
        case agentMutex:
            if (sharedMemory->agentMutex == 1) {
                sharedMemory->agentMutex = 0;
            }
            break;
        case criticalSection:
            if (sharedMemory->criticalSection == 1) {
                sharedMemory->criticalSection = 0;
            }
            break;
    }
}

int getRandomIngredientIndex(int min, int max) {
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
    //signal(memory, criticalSection);

    //bool smokerFinished = false;
    //while (!smokerFinished) {
    while(true) {
        if (memory->smokerMutex == 0 && memory->criticalSection == 0) {
            if (memory->criticalSection == 0) {
                wait(memory, criticalSection);
                if (memory->smokerOne != memory->agentIngredients[0] and
                    memory->smokerOne != memory->agentIngredients[1]) {
                    cout << "Smoker one started smoking\n";
                    cout.flush();
                    this_thread::sleep_for(chrono::seconds(2));
                    cout << "Smoker one finished smoking\n";
                    cout.flush();
                    //smokerFinished = true;
                    wait(memory, smokerMutex);
                    signal(memory, agentMutex);
                }
                signal(memory, criticalSection);
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
    //signal(memory, criticalSection);

    //bool smokerFinished = false;
    //while (!smokerFinished) {
    while (true) {
        if (memory->smokerMutex == 0 && memory->criticalSection == 0) {
            if(memory->criticalSection == 0) {
                wait(memory, criticalSection);
                if (memory->smokerTwo != memory->agentIngredients[0] and
                    memory->smokerTwo != memory->agentIngredients[1]) {
                    cout << "Smoker two started smoking\n";
                    cout.flush();
                    this_thread::sleep_for(chrono::seconds(2));
                    cout << "Smoker two finished smoking\n";
                    cout.flush();
                    //smokerFinished = true;
                    wait(memory, smokerMutex);
                    signal(memory, agentMutex);
                }
                signal(memory, criticalSection);
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
    //signal(memory, criticalSection);

    bool smokerFinished = false;
    //while (!smokerFinished) {
    while(true) {
        if (memory->smokerMutex == 0 && memory->criticalSection == 0) {
            if(memory->criticalSection == 0) {
                wait(memory, criticalSection);
                if (memory->smokerThree != memory->agentIngredients[0] and
                    memory->smokerThree != memory->agentIngredients[1]) {
                    cout << "Smoker three started smoking\n";
                    cout.flush();
                    this_thread::sleep_for(chrono::seconds(2));
                    cout << "Smoker three finished smoking\n";
                    cout.flush();
                    smokerFinished = true;
                    wait(memory, smokerMutex);
                    signal(memory, agentMutex);
                }
                signal(memory, criticalSection);
            }
        }
    }

    return NULL;
}

void* agent(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;
    //wait(memory, criticalSection);
    //cout << "agent started" << endl;
    //signal(memory, criticalSection);
    wait(memory, smokerMutex);

    while (true) {
        if (memory->criticalSection == 0) {
            wait(memory, criticalSection);
            if (memory->agentMutex == 0) {
                cout << "Agent has placed: " + ingredientOutput[memory->agentIngredients[0]]
                        + " and " + ingredientOutput[memory->agentIngredients[1]] + "\n";
                wait(memory, agentMutex);
                setTwoIngredients(memory);
                this_thread::sleep_for(chrono::seconds(2));
                signal(memory, smokerMutex);
            }
            signal(memory, criticalSection);
        }
    }

    return NULL;
}

int main() {


    SharedMemory *sharedMemory = &sMem;

    //get random ingredient indices
    srand(time(NULL));
    setSmokerIngredients(sharedMemory);
    setTwoIngredients(sharedMemory);

    //display what each smoker has
    cout << "Smoker one has " + ingredientOutput[sharedMemory->smokerOne] + "\n";
    cout.flush();
    cout << "Smoker two has " + ingredientOutput[sharedMemory->smokerTwo] + "\n";
    cout.flush();
    cout << "Smoker three has " + ingredientOutput[sharedMemory->smokerThree] + "\n";
    cout.flush();

    //setTwoIngredients(sharedMemory);

    pthread_t tidSmokerOne;
    pthread_t tidSmokerTwo;
    pthread_t tidSmokerThree;
    pthread_t tidAgent;
    pthread_attr_t attrSmokerOne;
    pthread_attr_t attrSmokerTwo;
    pthread_attr_t attrSmokerThree;
    pthread_attr_t attrAgent;

    pthread_attr_init(&attrAgent);
    pthread_attr_init(&attrSmokerOne);
    pthread_attr_init(&attrSmokerTwo);
    pthread_attr_init(&attrSmokerThree);
    pthread_create(&tidAgent, &attrAgent, agent, sharedMemory);
    pthread_create(&tidSmokerOne, &attrSmokerOne, smokerOne, sharedMemory);
    pthread_create(&tidSmokerTwo, &attrSmokerTwo, smokerTwo, sharedMemory);
    pthread_create(&tidSmokerThree, &attrSmokerThree, smokerThree, sharedMemory);

    pthread_join(tidAgent, NULL);
    pthread_join(tidSmokerOne, NULL);
    pthread_join(tidSmokerTwo, NULL);
    pthread_join(tidSmokerThree, NULL);

    return 0;
}