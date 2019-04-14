//
// Created by paras on 23-Mar-19.
//

#include <stdbool.h>
#include "p3150173-p3150090-p3120120-res1.h"

unsigned int profit = 0;
unsigned int *profit_ptr = &profit;
unsigned int transactions = 0;
unsigned int *transactions_ptr = &transactions;
unsigned int served = 0;
unsigned int *served_ptr = &served;

__thread unsigned int seed;
__thread unsigned int id;

int random_sleep;
int random_card;
int random_choice;

unsigned int customers;
unsigned int telephonist = N_TEL;
unsigned int remainingSeats = N_SEATS;
unsigned int *remainingSeats_ptr = &remainingSeats;

// Calculate time taken by a request
struct timespec requestStart, requestEnd;
struct tm start;
struct tm end;
struct Pairs {
    unsigned int i;
    unsigned int *arr1;
    unsigned int *arr2;
};
static __thread struct Pairs *my_pair;

unsigned int sleepRandom(int, int);

unsigned int choiceRandom(int, int);
double cardRandom(double, double);

void startTimer();
void stopTimer();
void Clock();

void printArray(unsigned int *);

void printInfo(unsigned int *);

pthread_mutex_t lock;
pthread_mutex_t bank;
pthread_mutex_t transaction;
pthread_mutex_t avgWaitTime;
pthread_mutex_t avgServingTime;
pthread_mutex_t array;
pthread_mutex_t print;
pthread_mutex_t decision;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

bool condition(unsigned int *var1, unsigned int i);

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

    printf("Customers to be served: %03d\n\n", customers);
    sleep(2);
    int rc;

    pthread_t threads[customers];
    unsigned int seatsArray[N_SEATS];
    unsigned int choice[customers];

    void *customer(void *x);

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&bank, NULL);
    pthread_mutex_init(&transaction, NULL);
    pthread_mutex_init(&avgWaitTime, NULL);
    pthread_mutex_init(&avgServingTime, NULL);
    pthread_mutex_init(&array, NULL);
    pthread_mutex_init(&print, NULL);
    pthread_mutex_init(&decision, NULL);

    startTimer();

    struct Pairs *pair;

    for (int i = 0; i < customers; i++) {
        random_sleep = rand_r(&seed);
        seed++;
        random_card = rand_r(&seed);
        seed++;
        random_choice = rand_r(&seed);
        pair = malloc(sizeof(struct Pairs));
        (*pair).i = i;
        (*pair).arr1 = choice;
        (*pair).arr2 = seatsArray;
        if ((rc = pthread_create(&threads[i], NULL, customer, (void *) pair))) {
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
    printInfo(seatsArray);
}

void *customer(void *x) {

    int rc;

    my_pair = (struct Pairs *) x;
    id = (*my_pair).i;
    unsigned int *choice = (*my_pair).arr1;
    rc = pthread_mutex_lock(&array);
    unsigned int *seatsArray = (*my_pair).arr2;
    rc = pthread_mutex_unlock(&array);

    // Clock();
    // printf("Customer %03d calling..\n", (id));

    rc = pthread_mutex_lock(&lock);

    while (telephonist == 0) {

        // rc = pthread_mutex_lock(&print);
        // Clock();
        // printf("Customer %03d couldn't find telephonist available. Blocked..\n", id);
        // rc = pthread_mutex_unlock(&print);
        rc = pthread_cond_wait(&cond, &lock);
    }

    // rc = pthread_mutex_lock(&print);
    // Clock();
    // printf("Customer %03d being served..\n", id);
    // rc = pthread_mutex_unlock(&print);

    --telephonist;
    rc = pthread_mutex_unlock(&lock);

    sleep(sleepRandom(T_SEAT_LOW, T_SEAT_HIGH));

    if (*remainingSeats_ptr == 0) {
        rc = pthread_mutex_lock(&print);
        Clock();
        printf("Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο\n");
        rc = pthread_mutex_unlock(&print);

    } else {

        if (condition(choice, id)) {

            if (cardRandom(0.0, 1.0) < P_CARD_SUCCESS) {

                rc = pthread_mutex_lock(&bank);
                *profit_ptr += (choice[id] * C_SEAT);
                rc = pthread_mutex_unlock(&bank);

                rc = pthread_mutex_lock(&transaction);
                ++(*transactions_ptr);
                rc = pthread_mutex_unlock(&transaction);

                rc = pthread_mutex_lock(&array);
                *remainingSeats_ptr -= choice[id];
                //for (int i = 0; i < (*temp_ptr + choice[id]) && i < N_SEATS; i++) {
                int flag = 0;
                for (int i = 0; i < N_SEATS; i++) {
                    if (seatsArray[i] == 0) {
                        for (int j = 0; j < choice[id]; j++) {
                            seatsArray[j] = id;
                            flag = 1;
                        }
                    }
                    if (flag) break;
                }
                rc = pthread_mutex_unlock(&array);

                rc = pthread_mutex_lock(&print);
                Clock();
                printf("Customer %03d seats booked\n", id);
                rc = pthread_mutex_unlock(&print);

            } else {
                rc = pthread_mutex_lock(&print);
                Clock();
                printf("Η κράτηση ματαιώθηκε γιατί η συναλλαγή με την πιστωτική κάρτα δεν έγινε αποδεκτή\n");
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
    printf("Customer %03d served successfully!\n", id);
    ++(*served_ptr);
    rc = pthread_mutex_unlock(&print);

    ++telephonist;
    rc = pthread_cond_broadcast(&cond);
    rc = pthread_mutex_unlock(&lock);

    pthread_exit(NULL); //return
}

unsigned int sleepRandom(int min, int max) {
    return (random_sleep % (max - min + 1)) + min;
}

unsigned int choiceRandom(int min, int max) {
    return (random_choice % (max - min + 1)) + min;
}

double cardRandom(double min, double max) {
    double f = (double) random_card / RAND_MAX;
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
    printf("[%02d:%02d:%02d] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void printArray(unsigned int *arr) {
    int printCounter = 1;
    for (int i = 0; i < N_SEATS; i++) {
        printf("Θέση %03d: Πελάτης %03d | ", i + 1, arr[i]);
        if (printCounter == 5) {
            printf("\n");
            printCounter = 0;
        }
        printCounter += 1;
    }
}

bool condition(unsigned int *var1, unsigned int i) {
    var1[i] = choiceRandom(N_SEAT_LOW, N_SEAT_HIGH);
    pthread_mutex_lock(&decision);
    bool result = (var1[i] <= *remainingSeats_ptr);
    pthread_mutex_unlock(&decision);
    if (!result) {
        printf("Η κράτηση ματαιώθηκε γιατί δενυπάρχουν αρκετές διαθέσιμες θέσεις\n");
    }
    return result;
}

void printInfo(unsigned int *arr) {
    long int totalSeconds = requestEnd.tv_sec - requestStart.tv_sec;
    long int minutes = totalSeconds / 60;
    long int seconds = totalSeconds % 60;
    double avTimePerCust = ((double) (totalSeconds) / customers);
    printArray(arr);
    printf("\nStart [%d:%d:%d]\n", start.tm_hour, start.tm_min, start.tm_sec);
    printf("End   [%d:%d:%d]\n\n", end.tm_hour, end.tm_min, end.tm_sec);
    printf("Διάρκεια: %ld λεπτό/ά και %ld δευτερόλεπτο/α (%lds)\n", minutes, seconds, totalSeconds);
    printf("Μέσος χρόνος αναμονής: %0.2f seconds\n", avTimePerCust);
    printf("Μέσος χρόνος εξυπηρέτησης: %0.2f seconds\n", avTimePerCust);
    printf("Number of customers served: %03d\n", served);
    printf("Number of seats booked: %d\n", N_SEATS - remainingSeats);
    printf("Number of seats left: %d\n", remainingSeats);
    printf("Συναλλαγές: %d\n", transactions);
    printf("Έσοδα: %d\u20AC\n", profit);
    printf("\nExiting..\n");
}