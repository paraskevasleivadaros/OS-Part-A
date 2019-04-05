//
// Created by paras on 23-Mar-19.
//
#include "p3150173-p3150090-res1.h"

double profit = 0.0;
int transactions = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]){

    // Converting string type to integer type
    // using function atoi
    int N_CUST = atoi(argv[1]);
    int seed = atoi(argv[2]);

    // Checking if all provided numbers are positive
    if (N_CUST <= 0 || seed <= 0){
        printf("Please enter only positive values in arguments!");
        exit(1);
    }

    printf("customers = %d, seed = %d", N_CUST, seed);

    srand(seed);

    int N_SEATS_LEFT = N_SEATS;
    int N_CHOICE;

    while (N_CUST !=0){
        printf("\nHow many seats would you like to book:");
        scanf("%d", &N_CHOICE);
        if (N_CHOICE<N_SEAT_HIGH+1 && N_CHOICE > N_SEAT_LOW-1){
            if (N_CHOICE<=N_SEATS_LEFT){
                N_SEATS_LEFT -= N_CHOICE;
                if (P_CARD_SUCCESS*rand()){
                    profit += N_CHOICE*C_SEAT;
                    ++transactions;
                    printf("Success: Your seats are booked :)");
                }else{
                    N_SEATS_LEFT+=N_CHOICE;
                    printf("Error: Your card failed :(");
                }
            } else {
                printf("Error: Number of seats to book exceeds number of seats left");
            }
        } else {
            printf("Error: You cannot book %d seats", N_CHOICE);
        }
        --N_CUST;
    }
    printf("\nAll served!\nExiting..");
}





// std::mutex m;//you can use std::lock_guard if you want to be exception safe
// int i = 0;
// void makeACallFromPhoneBooth()
//    {
//        // The other men wait outside
//        m.lock(); // one man gets a hold of the phone booth door and locks it.
//        //man happily talks to his wife from now....
//        std::cout << i << " Hello Wife" << std::endl;
//        i++; // no other thread can access variable i until m.unlock() is called
//        //...until now, with no interruption from other men
//        m.unlock();//man unlocks the phone booth door
//    }
//
//    int main()
//    {
//        // This is the main crowd of people uninterested in making a phone call
//
//        // man1 leaves the crowd to go to the phone booth
//        std::thread man1(makeACallFromPhoneBooth);
//        // Although man2 appears to start second, there's a good chance he might
//        // reach the phone booth before man1
//        std::thread man2(makeACallFromPhoneBooth);
//        // And hey, man3 also joined the race to the booth
//        std::thread man3(makeACallFromPhoneBooth);
//
//        man1.join();//man1 finished his phone call and joins the crowd
//        man2.join();//man2 finished his phone call and joins the crowd
//        man3.join();//man3 finished his phone call and joins the crowd
//        return 0;
//    }