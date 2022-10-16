#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>

using namespace std;

enum Access {smokerOneMutex,
        smokerTwoMutex,
        smokerThreeMutex,
        agentMutex,
        criticalSection};

enum Ingredient {
    Tobacco,
    Paper,
    Matches
};

Ingredient ingredientMapping[3] = {Tobacco, Paper, Matches};

struct SharedMemory {
    int smokerOneMutex = 0;
    int smokerTwoMutex = 0;
    int smokerThreeMutex = 0;
    int agentMutex = 0;
    int criticalSection = 0;
    Ingredient smokerOne;
    Ingredient smokerTwo;
    Ingredient smokerThree;
} sMem;

void wait(SharedMemory *sharedMemory, Access toAccess) {
    switch (toAccess) {
        case smokerOneMutex:
            if (sharedMemory->smokerOneMutex == 0) {
                sharedMemory->smokerOneMutex = 1;
            }
        case smokerTwoMutex:
            if (sharedMemory->smokerTwoMutex == 0) {
                sharedMemory->smokerTwoMutex = 1;
            }
        case smokerThreeMutex:
            if (sharedMemory->smokerThreeMutex == 0) {
                sharedMemory->smokerThreeMutex = 1;
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
        case smokerOneMutex:
            if (sharedMemory->smokerOneMutex == 1) {
                sharedMemory->smokerOneMutex = 0;
            }
        case smokerTwoMutex:
            if (sharedMemory->smokerTwoMutex == 1) {
                sharedMemory->smokerTwoMutex = 0;
            }
        case smokerThreeMutex:
            if (sharedMemory->smokerThreeMutex == 1) {
                sharedMemory->smokerThreeMutex = 0;
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

void* smokerOne(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;
    while (true)
    cout << "smoker one started" << endl;

    return NULL;
}

void* smokerTwo(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;
    cout << "smoker two started" << endl;

    return NULL;
}

void* smokerThree(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;
    cout << "smoker three started" << endl;

    return NULL;
}

void* agent(void *sharedMemory) {
    SharedMemory *memory = (SharedMemory *) sharedMemory;
    cout << "agent started" << endl;

    return NULL;
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

void setTwoIngredients(Ingredient *ingredients) {
    //set first ingredient
    int ingredientOneIndex = getRandomIngredientIndex(0,2);
    ingredients[0] = ingredientMapping[ingredientOneIndex];

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
    ingredients[1] = remainingIngredients[ingredientTwoIndex];
}

int main() {
    SharedMemory *sharedMemory = &sMem;

    //get random ingredient indices
    srand(time(NULL));
    setSmokerIngredients(sharedMemory);

    Ingredient twoIngredients[2];
    setTwoIngredients(twoIngredients);

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
    pthread_create(&tidSmokerOne, &attrSmokerOne, smokerOne, NULL);
    pthread_create(&tidSmokerTwo, &attrSmokerTwo, smokerTwo, NULL);
    pthread_create(&tidSmokerThree, &attrSmokerThree, smokerThree, NULL);
    pthread_create(&tidAgent, &attrAgent, agent, NULL);

    pthread_join(tidSmokerOne, NULL);
    pthread_join(tidSmokerTwo, NULL);
    pthread_join(tidSmokerThree, NULL);
    pthread_join(tidAgent, NULL);

    return 0;
}