To execute, run either:
    g++ -std=c++11 -lpthread -o {file}.out -lpthread {file}.cpp
    or
    clang++ -std=c++11 -lpthread -o {file}.out {file}.cpp if g++ does not work

In order to change producer additions:
    For 1:
        For setting number of chairs, in main function, change second argument value for
        setNumChairs
        For changing admissions into barber shop, add/remove enterBarberShop calls in
        producer function, with different indices of people array for different people names

    For 2:
        For setting number of rounds, in main function, change seconds argument value for
        setNumRounds

    For 3:
        For setting number of chairs, in main function, change second argument value for
        setNumHallwayChairs
        For changing admissions into hallway, add/remove enterHallway calls in producer function,
        with different indices of students array for different student names

    For 4 (a,b,c):
        For changing admissions into database, add/remove addReader/addWriter calls in addPeople
        function, with different indices of students array for different student names

    For 5:
        For changing red robin quantum value, change the second argument value of the call to
        setRRQuantum in the main function
        For changing north and south travelers, add/remove addNorthTraveler/addSouthTraveler in
        northProducer/southProducer functions, with different indices of traveler array for different
        traveler names

    For 6:
        a:
        For changing rope capacity, change the second argument value of the call to setRopeCapacity
        in the main function
        For changing eastward and westward monkeys, add/remove addEastwardMonkey/addWestwardMonkey calls in
        eastwardProducer/westwardProducer functions, with different values for the id of the monkey

        b:
        For changing rope capacity, change the second argument value of the call to setRopeCapacity
        in the main function
        For changing the round robin quantum, change the second argument value of the call to setRRQuantum
        in the main function
        For changing eastward and westward monkeys, add/remove addEastwardMonkey/addWestwardMonkey calls in
        eastwardProducer/westwardProducer, with different values for the id of the monkey

        c:
        For changing overall, eastward, and westward rope capacities, change the second argument value of the
        calls to setRopeCapacity/setEastwardRopeCapacity/setWestwardRopeCapacity in the main function
        For changing eastward and westward students, add/remove addEastwardStudent/addWestwardStudent calls in
        eastwardProducer/westwardProducer, with different values for the id of the student