//
// Created by paras on 23-Mar-19.
//
#include "p3150173-p3150090-res1.h"

double profit = 0.0;
int transcations = 0;

// static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]){

    // Converting string type to integer type
    // using function atoi
    int ncust = atoi(argv[1]);
    int seed = atoi(argv[2]);

    // Checking if all provided numbers are positive or not
    if (ncust < 0 || seed < 0){
        printf("Please enter only positive values in arguments!");
        exit(1);
    }

    // TODO
    //

    printf("customers = %d, seed = %d", ncust, seed);

}