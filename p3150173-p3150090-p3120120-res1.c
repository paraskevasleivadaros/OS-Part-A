//
// Created by paras on 23-Mar-19.
//

#include "p3150173-p3150090-p3120120-res1.h"

unsigned int profit = 0;
unsigned int transactions = 0;
unsigned int served = 0;
unsigned int customers;
unsigned int telephonist = N_TEL;
unsigned int remainingSeats = N_SEATS;
unsigned int seed;

unsigned int *profit_ptr = &profit;
unsigned int *transactions_ptr = &transactions;
unsigned int *served_ptr = &served;
unsigned int *remainingSeats_ptr = &remainingSeats;
unsigned int seatsArray[N_SEATS];

unsigned int random_sleep;
unsigned int random_card;
unsigned int random_choice;

// Calculate time taken by a request
struct timespec requestStart, requestEnd;
struct tm start;
struct tm end;

unsigned long int avgServingTime;
unsigned long int avgWaitTime;
unsigned long int *avgServingTime_ptr = &avgServingTime;
unsigned long int *avgWaitTime_ptr = &avgWaitTime;

unsigned int sleepRandom(int, int);
unsigned int choiceRandom(int, int);
double cardRandom(double, double);

void startTimer();
void stopTimer();
void Clock();

void printArray(unsigned int *);

void printInfo();

pthread_mutex_t operatorsLock;
pthread_mutex_t bankLock;
pthread_mutex_t transactionLock;
pthread_mutex_t avgWaitTimeLock;
pthread_mutex_t avgServingTimeLock;
pthread_mutex_t arrayLock;
pthread_mutex_t screenLock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

bool checkAvailableSeats(unsigned int);

bool checkRemainingSeats();

int main(int argc, char* argv[]){

    if (argc != 3) {
        printf("Please enter only the number of customers and the seed!");
        exit(-1);
    }

    // Converting string type to integer type
    // using function atoi
    customers = atoi(argv[1]);
    seed = atoi(argv[2]);

    // Checking if number of customers is positive
    if (customers <= 0) {
        printf("Please enter only positive values in arguments!\n");
        exit(-2);
    }

    printf("Customers to be served: %03d\n\n", customers);

    int rc;

    pthread_t threads[customers];
    int id[customers];

    void *customer(void *x);

    rc = pthread_mutex_init(&operatorsLock, NULL);
    rc = pthread_mutex_init(&bankLock, NULL);
    rc = pthread_mutex_init(&transactionLock, NULL);
    rc = pthread_mutex_init(&avgWaitTimeLock, NULL);
    rc = pthread_mutex_init(&avgServingTimeLock, NULL);
    rc = pthread_mutex_init(&arrayLock, NULL);
    rc = pthread_mutex_init(&screenLock, NULL);

    startTimer();

    for (int i = 0; i < customers; i++) {
        id[i] = i + 1;
        random_sleep = rand_r(&seed);
        random_card = rand_r(&seed);
        random_choice = rand_r(&seed);
        if ((rc = pthread_create(&threads[i], NULL, customer, (void *) id[i]))) {
            Clock();
            printf("Thread Creation Error\n");
            exit(1);
        }
    }

    for (int i = 0; i < customers; i++) {
        pthread_join(threads[i], NULL);
    }

    stopTimer();

    printInfo(seatsArray);

    pthread_mutex_destroy(&operatorsLock);
    pthread_mutex_destroy(&bankLock);
    pthread_mutex_destroy(&transactionLock);
    pthread_mutex_destroy(&avgWaitTimeLock);
    pthread_mutex_destroy(&avgServingTimeLock);
    pthread_mutex_destroy(&arrayLock);
    pthread_mutex_destroy(&screenLock);
    pthread_cond_destroy(&cond);
}

void *customer(void *x) {

    int rc;
    int id = (int *) x;

    struct timespec waitStart, waitEnd, servStart, servEnd;

    // Customer calling..
    clock_gettime(CLOCK_REALTIME, &servStart);

    rc = pthread_mutex_lock(&operatorsLock);

    clock_gettime(CLOCK_REALTIME, &waitStart);

    while (telephonist == 0) {
        // Customer couldn't find telephonist available. Blocked..
        rc = pthread_cond_wait(&cond, &operatorsLock);
    }

    clock_gettime(CLOCK_REALTIME, &waitEnd);

    // Customer being served..

    --telephonist;

    rc = pthread_mutex_unlock(&operatorsLock);

    rc = pthread_mutex_lock(&avgWaitTimeLock);
    *avgWaitTime_ptr = *avgWaitTime_ptr + (waitEnd.tv_sec - waitStart.tv_sec);
    rc = pthread_mutex_unlock(&avgWaitTimeLock);

    sleep(sleepRandom(T_SEAT_LOW, T_SEAT_HIGH));

    if (checkRemainingSeats()) {

        rc = pthread_mutex_lock(&screenLock);
        Clock();
        printf("Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο\n");
        rc = pthread_mutex_unlock(&screenLock);

    } else {

        unsigned int seats = choiceRandom(N_SEAT_LOW, N_SEAT_HIGH);
        if (checkAvailableSeats(seats)) {

            if (cardRandom(0.0, 1.0) < P_CARD_SUCCESS) {

                rc = pthread_mutex_lock(&arrayLock);
                int temp = seats;
                for (int i = 0; temp > 0 && i < N_SEATS; i++) {
                    if (seatsArray[i] == 0) {
                        seatsArray[i] = id;
                        temp--;
                    }
                }
                (*remainingSeats_ptr) -= seats;
                rc = pthread_mutex_unlock(&arrayLock);

                rc = pthread_mutex_lock(&bankLock);
                *profit_ptr += (seats * C_SEAT);
                rc = pthread_mutex_unlock(&bankLock);

                rc = pthread_mutex_lock(&transactionLock);
                int transactionID = ++(*transactions_ptr);
                rc = pthread_mutex_unlock(&transactionLock);

                rc = pthread_mutex_lock(&screenLock);
                Clock();
                printf("Οι θέσεις για τον πελάτη %03d δεσμεύθηκαν επιτυχώς\n", id);
                Clock();
                printf("Κωδικός Συναλλαγής: %03d\n", transactionID);
                rc = pthread_mutex_lock(&arrayLock);
                Clock();
                printf("Θέσεις: ");
                for (int i = 0; i < N_SEATS; i++) {
                    if (seatsArray[i] == id) {
                        printf("%03d ", i + 1);
                    }
                }
                printf("\n");
                rc = pthread_mutex_unlock(&arrayLock);
                Clock();
                printf("Κόστος: %03d\u20AC\n", seats * C_SEAT);
                rc = pthread_mutex_unlock(&screenLock);

            } else {
                rc = pthread_mutex_lock(&screenLock);
                Clock();
                printf("Η κράτηση ματαιώθηκε γιατί η συναλλαγή με την πιστωτική κάρτα δεν έγινε αποδεκτή\n");
                rc = pthread_mutex_unlock(&screenLock);
            }
        } else {
            rc = pthread_mutex_lock(&screenLock);
            Clock();
            printf("Not enough seats left to book.\n");
            rc = pthread_mutex_unlock(&screenLock);
        }
    }

    rc = pthread_mutex_lock(&operatorsLock);

    // Customer served successfully
    ++(*served_ptr);

    ++telephonist;
    rc = pthread_cond_broadcast(&cond);
    rc = pthread_mutex_unlock(&operatorsLock);
    rc = clock_gettime(CLOCK_REALTIME, &servEnd);

    rc = pthread_mutex_lock(&avgServingTimeLock);
    *avgServingTime_ptr = *avgServingTime_ptr + (servEnd.tv_sec - servStart.tv_sec);
    rc = pthread_mutex_unlock(&avgServingTimeLock);

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

bool checkAvailableSeats(unsigned int choice) {
    int rc = pthread_mutex_lock(&arrayLock);
    bool result = (choice <= (*remainingSeats_ptr));
    if (!result) {
        pthread_mutex_lock(&screenLock);
        printf("Η κράτηση ματαιώθηκε γιατί δεν υπάρχουν αρκετές διαθέσιμες θέσεις\n");
        pthread_mutex_unlock(&screenLock);
    }
    rc = pthread_mutex_unlock(&arrayLock);
    return result;
}

bool checkRemainingSeats() {
    int rc = pthread_mutex_lock(&arrayLock);
    bool result = (*remainingSeats_ptr == 0);
    rc = pthread_mutex_unlock(&arrayLock);
    return result;
}

void printArray(unsigned int *arr) {
    int rc = pthread_mutex_lock(&arrayLock);
    int printCounter = 1;
    printf("|");
    for (int i = 0; i < N_SEATS; i++) {
        if (arr[i] == 0) {
            printf("Θέση %03d: Πελάτης     | ", i + 1);
        } else {
            printf("Θέση %03d: Πελάτης %03d | ", i + 1, arr[i]);
        }
        if (printCounter == 6) {
            printf("\n|");
            printCounter = 0;
        }
        printCounter += 1;
    }
    rc = pthread_mutex_unlock(&arrayLock);
}

void printInfo() {
    unsigned long int totalSeconds = requestEnd.tv_sec - requestStart.tv_sec;
    unsigned long int minutes = totalSeconds / 60;
    unsigned long int seconds = totalSeconds % 60;
    printf("Start [%d:%d:%d]\n", start.tm_hour, start.tm_min, start.tm_sec);
    printf("End   [%d:%d:%d]\n\n", end.tm_hour, end.tm_min, end.tm_sec);
    printArray(seatsArray);
    printf("\n");
    if (minutes > 1) {
        printf("Διάρκεια: %ld λεπτά ", minutes);
    } else {
        printf("Διάρκεια: %ld λεπτό ", minutes);
    }
    if (seconds > 1) {
        printf("και %ld δευτερόλεπτα (%lds)\n", seconds, totalSeconds);
    } else {
        printf("και %ld δευτερόλεπτο (%lds)\n", seconds, totalSeconds);
    }
    printf("Μέσος χρόνος αναμονής: %0.2f seconds\n", (double) avgWaitTime / customers);
    printf("Μέσος χρόνος εξυπηρέτησης: %0.2f seconds\n", (double) avgServingTime / customers);
    printf("Εξυπηρετήθηκαν: %03d πελάτες\n", served);
    int rc = pthread_mutex_lock(&arrayLock);
    printf("Δεσμευμένες Θέσεις: %d\n", N_SEATS - (*remainingSeats_ptr));
    printf("Ελεύθερεις Θέσεις: %d\n", (*remainingSeats_ptr));
    rc = pthread_mutex_unlock(&arrayLock);
    printf("Συναλλαγές: %d\n", (*transactions_ptr));
    printf("Έσοδα: %d\u20AC\n", (*profit_ptr));
    printf("\nExiting..\n");
}