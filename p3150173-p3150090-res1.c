//
// Created by paras on 23-Mar-19.
//

#include "p3150173-p3150090-res1.h"

unsigned int profit = 0;
unsigned int *profit_ptr = &profit;
unsigned int transactions = 0;
unsigned int *transactions_ptr = &transactions;
unsigned int served = 0;
unsigned int *served_ptr = &served;

_Thread_local unsigned int seed;
int random_number;
unsigned int choice;

// int *seatsArray;

unsigned int customers;
unsigned int telephonist = N_TEL;
unsigned int remainingSeats = N_SEATS;
unsigned int *remainingSeats_ptr = &remainingSeats;

// Calculate time taken by a request
struct timespec requestStart, requestEnd;
struct tm start;
struct tm end;

void *customer(void *);

int i_random(int, int);
double f_random(double, double);

void startTimer();
void stopTimer();
void Clock();

// void printInfo(int arr[]);
void printInfo2();

pthread_mutex_t lock;
pthread_mutex_t bank;
pthread_mutex_t transaction;
pthread_mutex_t avgWaitTime;
pthread_mutex_t avgServingTime;
pthread_mutex_t array;
pthread_mutex_t print;
pthread_mutex_t decision;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]){

    if (argc != 3) {
        printf("Please enter only the number of customers and the seed!");
        exit(-1);
    }

    // Converting string type to integer type
    // using function atoi
    customers = atoi(argv[1]);
    seed = atoi(argv[2]);

    // Checking if all provided numbers are positive
    if (customers <= 0 || seed <= 0) {
        printf("Please enter only positive values in arguments!\n");
        exit(-2);
    }

    printf("Customers to be served: %02d\n\n", customers);

    int rc;

    pthread_t threads[customers];
    int id[customers];


    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&bank, NULL);
    pthread_mutex_init(&transaction, NULL);
    pthread_mutex_init(&avgWaitTime, NULL);
    pthread_mutex_init(&avgServingTime, NULL);
    pthread_mutex_init(&array, NULL);
    pthread_mutex_init(&print, NULL);
    pthread_mutex_init(&decision, NULL);
/*
    seatsArray = malloc(customers * sizeof(int));
    if (seatsArray == NULL) {
        fprintf(stderr, "Malloc Failed\n");
        exit(-1);
    }
    memset(seatsArray, 0, sizeof(int) * customers);
*/
    startTimer();

    for (int i = 0; i < customers; i++) {
        id[i] = i + 1;
        random_number = rand_r(&seed);
        if ((rc = pthread_create(&threads[i], NULL, customer, (void *) id[i]))) {
            Clock();
            printf("Thread Creation Error\n");
            exit(1);
        }
    }

    for (int i = 0; i < customers; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&bank);
    pthread_mutex_destroy(&transaction);
    pthread_mutex_destroy(&avgWaitTime);
    pthread_mutex_destroy(&avgServingTime);
    pthread_mutex_destroy(&array);
    pthread_mutex_destroy(&print);
    pthread_mutex_destroy(&decision);
    pthread_cond_destroy(&cond);

    stopTimer();
    //printInfo(seatsArray);
    printInfo2();
}

void *customer(void *x) {

    int id = (int) (int *) x;
    int rc;

    rc = pthread_mutex_lock(&print);
    Clock();
    printf("Customer %02d calling..\n", (id));
    rc = pthread_mutex_unlock(&print);

    rc = pthread_mutex_lock(&lock);

    while (telephonist == 0) {

        rc = pthread_mutex_lock(&print);
        Clock();
        printf("Customer %02d couldn't find telephonist available. Blocked..\n", id);
        rc = pthread_mutex_unlock(&print);
        rc = pthread_cond_wait(&cond, &lock);
    }

    rc = pthread_mutex_lock(&print);
    Clock();
    printf("Customer %02d being served..\n", id);
    rc = pthread_mutex_unlock(&print);

    --telephonist;
    rc = pthread_mutex_unlock(&lock);

    sleep(i_random(T_SEAT_LOW, T_SEAT_HIGH));

    if (remainingSeats == 0) {
        rc = pthread_mutex_lock(&print);
        Clock();
        printf("Sold out!\n");
        rc = pthread_mutex_unlock(&print);
    } else {

        choice = i_random(N_SEAT_LOW, N_SEAT_HIGH);

        if (choice <= remainingSeats) {

            if (f_random(0.0, 1.0) < P_CARD_SUCCESS) {

                rc = pthread_mutex_lock(&bank);
                *profit_ptr += (choice * C_SEAT);
                rc = pthread_mutex_unlock(&bank);

                rc = pthread_mutex_lock(&transaction);
                ++(*transactions_ptr);
                rc = pthread_mutex_unlock(&transaction);

                rc = pthread_mutex_lock(&array);
                *remainingSeats_ptr -= choice;
                // seatsArray[id] = choice;
                rc = pthread_mutex_unlock(&array);

                rc = pthread_mutex_lock(&print);
                Clock();
                printf("Customer %02d seats booked\n", id);
                rc = pthread_mutex_unlock(&print);

            } else {
                rc = pthread_mutex_lock(&print);
                Clock();
                printf("Card of Customer %02d failed\n", id);
                rc = pthread_mutex_unlock(&print);
            }
        } else {
            rc = pthread_mutex_lock(&print);
            Clock();
            printf("Not enough seats left to book.\n");
            rc = pthread_mutex_unlock(&print);
        }
    }

    rc = pthread_mutex_lock(&lock);

    rc = pthread_mutex_lock(&print);
    Clock();
    printf("Customer %02d served successfully!\n", id);
    ++(*served_ptr);
    rc = pthread_mutex_unlock(&print);

    ++telephonist;
    rc = pthread_cond_broadcast(&cond);
    rc = pthread_mutex_unlock(&lock);

    pthread_exit(NULL); //return
}

int i_random(int min, int max){
    return (random_number % (max - min + 1)) + min;
}

double f_random(double min, double max){
    double f = (double) random_number / RAND_MAX;
    return min + f * (max - min);
}

void startTimer(){
    Clock();
    printf("Starting Timer\n\n");
    time_t t = time(NULL);
    start = *localtime(&t);
    clock_gettime(CLOCK_REALTIME, &requestStart);
}

void stopTimer(){
    printf("\n");
    Clock();
    printf("Stopping Timer\n\n");
    time_t t = time(NULL);
    end = *localtime(&t);
    clock_gettime(CLOCK_REALTIME, &requestEnd);
}

void Clock() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("[%d:%d:%d] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

/*
void printInfo(int arr[]) {
    printf("Start [%d:%d:%d]\n", start.tm_hour, start.tm_min, start.tm_sec);
    printf("End   [%d:%d:%d]\n\n", end.tm_hour, end.tm_min, end.tm_sec);
    long int totalSeconds = requestEnd.tv_sec - requestStart.tv_sec;
    long int minutes = totalSeconds / 60;
    long int seconds = totalSeconds % 60;
    double avTimePerCust = ((double) (totalSeconds) / customers);
    printf("Duration: %ld minutes and %ld seconds (%lds)\n\n", minutes, seconds, totalSeconds);
    printf("Average time per customer: %0.2f seconds\n\n", avTimePerCust);
    printf("Number of customers served: %02d\n", served);
    printf("Number of seats booked: %d\n", N_SEATS - remainingSeats);
    printf("Number of seats left: %d\n", remainingSeats);
    printf("Transactions: %d\n", transactions);
    printf("Profit: %d\u20AC\n", profit);
    for (int i = 0; i < customers; i++) {
        printf("%d ", arr[i]);
    }
    printf("\nExiting..\n");
}
*/
void printInfo2() {
    printf("Start [%d:%d:%d]\n", start.tm_hour, start.tm_min, start.tm_sec);
    printf("End   [%d:%d:%d]\n\n", end.tm_hour, end.tm_min, end.tm_sec);
    long int totalSeconds = requestEnd.tv_sec - requestStart.tv_sec;
    long int minutes = totalSeconds / 60;
    long int seconds = totalSeconds % 60;
    double avTimePerCust = ((double) (totalSeconds) / customers);
    printf("Duration: %ld minutes and %ld seconds (%lds)\n\n", minutes, seconds, totalSeconds);
    printf("Average time per customer: %0.2f seconds\n\n", avTimePerCust);
    printf("Number of customers served: %02d\n", served);
    printf("Number of seats booked: %d\n", N_SEATS - remainingSeats);
    printf("Number of seats left: %d\n", remainingSeats);
    printf("Transactions: %d\n", transactions);
    printf("Profit: %d\u20AC\n", profit);
    printf("\nExiting..\n");
}