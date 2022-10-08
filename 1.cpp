#include <iostream>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <string>
#include <queue>



void hold(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

class BarberShop {
    public:
        int chairs;
        int chairsTaken;
        std::queue<Customer> *customersInShop;
        BarberShop() { // default constructor
            chairs = 5;
            chairsTaken = 0;
            customersInShop = new std::queue<Customer>;
        }
        BarberShop(int numChairs) {
            chairs = numChairs;
            chairsTaken = 0;
        }
        bool enter(Customer customer) {
            if (chairsTaken < chairs) {
                (*customersInShop).push(customer);
                chairsTaken += 1;
                return true; // successfully took customer in
            }
            else {
                return false; // shop was full, customer cannot enter
            }
        }
        Customer leave() {
            Customer customerLeaving = (*customersInShop).front();
            (*customersInShop).pop();
            chairsTaken -= 1;
            return customerLeaving;
        }
};

class Barber {
    public:
        bool working;
        BarberShop *barberShop;
        Barber() { // default constructor
            barberShop = new BarberShop(5);
            working = false;
        }
        Barber(BarberShop shop) {
            barberShop = &shop;
            working = false;
        }

        void cutHair() {
            while (true) {
                if (barberShop->chairs == 0) {
                    working = true;
                    std::cout << "Started haircut\n";
                    hold(2);
                    std::cout << "Finished haircut\n";
                    working = false;
                }
            }
        }
};

class Customer {
    public:
        std::string name;
        Customer() { // default constructor
            name = "Unnamed";
        }
        Customer(std::string inputName) {
            name = inputName;
        }
};


int main() {

    // TODO: implement using command line arguments for names
    std::string customerName[5] = {"John", "Amanda", "Lucy", "Bert", "Cynthia"};
    Customer *customers[5];

    BarberShop* barberShop = new BarberShop(3);
    Barber* barber = new Barber();

    for (int i = 0; i < sizeof(customerName)/sizeof(customerName[0]); ++i) {
        customers[i] = new Customer(customerName[i]);
        barberShop->enter(*customers[i]);
    }


    
    barber->cutHair();

}