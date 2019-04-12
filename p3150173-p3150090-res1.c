//
// Created by paras on 23-Mar-19.
//

#include <assert.h>
#include "p3150173-p3150090-res1.h"

unsigned int profit = 0;
unsigned int *profit_ptr = &profit;
unsigned int transactions = 0;
unsigned int *transactions_ptr = &transactions;
unsigned int served = 0;
unsigned int *served_ptr = &served;

_Thread_local unsigned int seed;
int random_number;

unsigned int customers;
unsigned int telephonist = N_TEL;
unsigned int N_SEATS_LEFT = N_SEATS;
unsigned int *N_SEATS_LEFT_ptr = &N_SEATS_LEFT;

int *seatsArray;

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

void bookSeats(int, int);
void printInfo();

pthread_mutex_t lock;
pthread_mutex_t lock1;
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

    printf("Customers to be served: %d\n\n", customers);

    int rc;

    pthread_t threads[customers];
    int id[customers];

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock1, NULL);

    seatsArray = malloc(customers * sizeof(int));

    startTimer();

    for (int i = 0; i < customers; i++) {
        id[i] = i + 1;
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
    pthread_mutex_destroy(&lock1);
    pthread_cond_destroy(&cond);

    stopTimer();

    printInfo();

    free(seatsArray);
    seatsArray = NULL;
}

void *customer(void *x) {

    random_number = rand_r(&seed);
    int id = (int) (int *) x;
    int rc;

    Clock();
    printf("Customer %d calling..\n", (id));

    rc = pthread_mutex_lock(&lock);

    while (telephonist == 0) {

        Clock();
        printf("Customer %d couldn't find telephonist available. Blocked..\n", id);

        rc = pthread_cond_wait(&cond, &lock);
    }

    Clock();
    printf("Customer %d being served..\n", id);

    --telephonist;
    rc = pthread_mutex_unlock(&lock);

    // rc = pthread_mutex_lock(&lock1);
    int N_CHOICE = i_random(N_SEAT_LOW, N_SEAT_HIGH);

    if (N_CHOICE <= N_SEATS_LEFT) {

        sleep(i_random(T_SEAT_LOW, T_SEAT_HIGH));


        if (f_random(0.0, 1.0) < P_CARD_SUCCESS) {

            bookSeats(N_CHOICE, id);

            Clock();
            printf("Customer %d seats booked\n", id);

        } else {

            Clock();
            printf("Card of Customer %d failed\n", id);

        }
    } else {

        Clock();
        printf("Not enough seats left to book.\n");

    }
    // rc = pthread_mutex_unlock(&lock1);

    rc = pthread_mutex_lock(&lock);

    Clock();
    printf("Customer %d served successfully!\n", id);
    ++(*served_ptr);

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

void bookSeats(int choice, int id) {
    *N_SEATS_LEFT_ptr -= choice;
    *profit_ptr += (choice * C_SEAT);
    ++(*transactions_ptr);
    seatsArray[id] = choice;
}

void printArray() {
    for (int i = 0; i < customers; i++) {
        printf("%d ", seatsArray[i]);
    }
}

void printInfo() {
    printf("Start [%d:%d:%d]\n", start.tm_hour, start.tm_min, start.tm_sec);
    printf("End   [%d:%d:%d]\n\n", end.tm_hour, end.tm_min, end.tm_sec);
    long int totalSeconds = requestEnd.tv_sec - requestStart.tv_sec;
    long int minutes = totalSeconds / 60;
    long int seconds = totalSeconds % 60;
    double avTimePerCust = ((double) (totalSeconds) / customers);
    printf("Duration: %ld minutes and %ld seconds (%lds)\n\n", minutes, seconds, totalSeconds);
    printf("Average time per customer: %0.2f seconds\n\n", avTimePerCust);
    printf("Number of customers served: %d\n", served);
    printf("Number of seats booked: %d\n", N_SEATS - N_SEATS_LEFT);
    printf("Number of seats left: %d\n", N_SEATS_LEFT);
    printf("Transactions: %d\n", transactions);
    printf("Profit: %d\u20AC\n", profit);
    printArray();
    printf("\nExiting..\n");
}