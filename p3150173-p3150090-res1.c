//
// Created by paras on 23-Mar-19.
//
#include "p3150173-p3150090-res1.h"

void* bookSeats(int, int);

double profit = 0.0;
int transactions = 0;

pthread_mutex_t lock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]){

    pthread_t threads[N_TEL];
    int rc;
    pthread_mutex_init(&lock, NULL);
    int id[N_TEL];

    // Converting string type to integer type
    // using function atoi
    int customers = atoi(argv[1]);
    int seed = atoi(argv[2]);

    // Checking if all provided numbers are positive
    if (customers <= 0 || seed <= 0){
        printf("Please enter only positive values in arguments!");
        exit(1);
    }

    // Calculate time taken by a request
    struct timespec requestStart, requestEnd;
    // clock_gettime(CLOCK_REALTIME, &requestStart);

    printf("Threads starting...");

    for(int i=0; i<N_TEL; i++) {
        id[i] = i + 1;
        rc = pthread_create(&threads[i], NULL, bookSeats(customers, seed), (void *) id[i]);
        if (rc) {
            printf((const char *) stderr, "Thread Creation Error :(");
            exit(1);
        } else {
            printf("Main: creating thread %d\n", i + 1);
        }
    }

    for (int i=0; i<8; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    // clock_gettime(CLOCK_REALTIME, &requestEnd);

    printf("\nAll served!\nGoodbye..");
}

void* bookSeats(int N_CUST, int seed) {

    srand(seed);
    int rc;

    rc = pthread_mutex_lock(&lock);
    rc = pthread_cond_wait(&cond, &lock);
    rc = pthread_mutex_unlock(&lock);
    rc = pthread_mutex_lock(&lock);
    rc = pthread_cond_signal(&cond);
    rc = pthread_mutex_unlock(&lock);

    int N_SEATS_LEFT = N_SEATS;
    int N_CHOICE;

    while (N_CUST != 0) {

        printf("\nHow many seats would you like to book: ");
        scanf("%d", &N_CHOICE);

        if (N_CHOICE < N_SEAT_HIGH + 1 && N_CHOICE > N_SEAT_LOW - 1) {

            if (N_CHOICE <= N_SEATS_LEFT) {

                N_SEATS_LEFT -= N_CHOICE;
                if (P_CARD_SUCCESS * rand()) {
                    profit += N_CHOICE * C_SEAT;
                    ++transactions;
                    printf("Success: Your seats are booked :)");
                } else {
                    N_SEATS_LEFT += N_CHOICE;
                    printf((const char *) stderr, "Error: Your card failed :(");
                }

            } else {
                printf((const char *) stderr, "Error: Number of seats to book exceeds number of seats left");
            }

        } else {
            printf((const char *) stderr, "Error: You cannot book %d seats", N_CHOICE);
        }

        --N_CUST;
    }
    pthread_exit(NULL);
}