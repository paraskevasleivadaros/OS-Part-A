//
// Created by paras on 23-Mar-19.
//

#include "p3150173-p3150090-p3120120-res1.h"

unsigned int seed;
unsigned int seats;
unsigned int customers;
unsigned int profit = 0;
unsigned int servedCounter = 0;
unsigned int transactions = 0;
unsigned int telephonist = N_TEL;
unsigned int remainingSeats = N_SEATS;

unsigned int seatsPlan[N_SEATS];
unsigned int *profitPtr = &profit;
unsigned int *servedCounterPtr = &servedCounter;
unsigned int *transactionsPtr = &transactions;
unsigned int *remainingSeatsPtr = &remainingSeats;

// Calculate time taken by a request
struct timespec requestStart, requestEnd;
struct tm start;
struct tm end;

unsigned long int avgWaitTime;
unsigned long int avgServTime;

unsigned long int *avgWaitTimePtr = &avgWaitTime;
unsigned long int *avgServTimePtr = &avgServTime;

unsigned int sleepRandom(int, int);
unsigned int choiceRandom(int, int);
double cardRandom(double, double);

void startTimer();
void stopTimer();
void Clock();

void check_rc(int);

unsigned int receivePayment(int);

unsigned int logTransaction();
void bookSeats(unsigned int, int);
void unbookSeats(unsigned int, int);

void printSeatsPlan();
void printInfo();

bool checkAvailableSeats(unsigned int);

bool checkRemainingSeats();

pthread_mutex_t operatorsLock;
pthread_mutex_t paymentLock;
pthread_mutex_t transactionLock;
pthread_mutex_t avgWaitTimeLock;
pthread_mutex_t avgServingTimeLock;
pthread_mutex_t seatsPlanLock;
pthread_mutex_t screenLock;
pthread_cond_t availableOperators;

int main(int argc, char* argv[]){

    if (argc != 3) {
        printf("Παρακαλώ δώστε μόνο το πλήθος των πελατών και τον σπόρο\n");
        exit(-1);
    }

    // Converting string type to integer type
    // using function atoi
    customers = atoi(argv[1]);
    seed = atoi(argv[2]);

    // Checking if number of customers is positive
    if (customers <= 0) {
        printf("Παρακαλώ δώστε μόνο θετική τιμή για το πλήθος των πελατών\n");
        exit(-2);
    }

    printf("Πελάτες προς εξυπηρέτηση: %03d\n\n", customers);

    int rc;

    pthread_t threads[customers];
    int id[customers];

    void *customer(void *x);

    rc = pthread_mutex_init(&operatorsLock, NULL);
    check_rc(rc);
    rc = pthread_mutex_init(&paymentLock, NULL);
    check_rc(rc);
    rc = pthread_mutex_init(&transactionLock, NULL);
    check_rc(rc);
    rc = pthread_mutex_init(&avgWaitTimeLock, NULL);
    check_rc(rc);
    rc = pthread_mutex_init(&avgServingTimeLock, NULL);
    check_rc(rc);
    rc = pthread_mutex_init(&seatsPlanLock, NULL);
    check_rc(rc);
    rc = pthread_mutex_init(&screenLock, NULL);
    check_rc(rc);
    rc = pthread_cond_init(&availableOperators, NULL);
    check_rc(rc);

    startTimer();

    for (int i = 0; i < customers; i++) {
        id[i] = i + 1;
        rc = pthread_create(&threads[i], NULL, customer, (void *) id[i]);
        check_rc(rc);
    }

    for (int i = 0; i < customers; i++) {
        pthread_join(threads[i], NULL);
    }

    stopTimer();

    pthread_mutex_destroy(&operatorsLock);
    pthread_mutex_destroy(&paymentLock);
    pthread_mutex_destroy(&transactionLock);
    pthread_mutex_destroy(&avgWaitTimeLock);
    pthread_mutex_destroy(&avgServingTimeLock);
    pthread_mutex_destroy(&seatsPlanLock);
    pthread_mutex_destroy(&screenLock);
    pthread_cond_destroy(&availableOperators);

    printInfo();
}

void *customer(void *x) {

    int rc;
    int id = (int *) x;

    struct timespec waitStart, waitEnd, servStart, servEnd;

    // Customer calling..

    rc = pthread_mutex_lock(&operatorsLock);
    check_rc(rc);

    clock_gettime(CLOCK_REALTIME, &waitStart);

    while (telephonist == 0) {
        // Customer couldn't find telephonist available. Blocked..
        rc = pthread_cond_wait(&availableOperators, &operatorsLock);
        check_rc(rc);
    }

    clock_gettime(CLOCK_REALTIME, &waitEnd);

    // Customer being served..
    --telephonist;

    rc = pthread_mutex_unlock(&operatorsLock);
    check_rc(rc);

    rc = pthread_mutex_lock(&avgWaitTimeLock);
    check_rc(rc);
    *avgWaitTimePtr = *avgWaitTimePtr + (waitEnd.tv_sec - waitStart.tv_sec);
    rc = pthread_mutex_unlock(&avgWaitTimeLock);
    check_rc(rc);

    clock_gettime(CLOCK_REALTIME, &servStart);

    sleep(sleepRandom(T_SEAT_LOW, T_SEAT_HIGH));

    if (checkRemainingSeats()) {

        rc = pthread_mutex_lock(&screenLock);
        check_rc(rc);
        Clock();
        printf("Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο\n\n");
        rc = pthread_mutex_unlock(&screenLock);
        check_rc(rc);

    } else {

        if (checkAvailableSeats(seats = choiceRandom(N_SEAT_LOW, N_SEAT_HIGH))) {

            bookSeats(seats, id);

            if (cardRandom(0.0, 1.0) < P_CARD_SUCCESS) {

                unsigned int amount = receivePayment(seats);
                unsigned int transactionID = logTransaction();

                rc = pthread_mutex_lock(&screenLock);
                check_rc(rc);
                Clock();

                printf("Η κράτηση ολοκληρώθηκε επιτυχώς. Ο αριθμός συναλλαγής είναι %03d", transactionID);

                rc = pthread_mutex_lock(&seatsPlanLock);
                check_rc(rc);
                printf(", οι θέσεις σας είναι οι: ");
                for (int i = 0; i < N_SEATS; i++) {
                    if (seatsPlan[i] == id) {
                        printf("%03d ", i + 1);
                    }
                }
                rc = pthread_mutex_unlock(&seatsPlanLock);
                check_rc(rc);
                printf("και το κόστος της συναλλαγής είναι %03d\u20AC\n\n", amount);
                rc = pthread_mutex_unlock(&screenLock);
                check_rc(rc);

            } else {
                unbookSeats(seats, id);
                rc = pthread_mutex_lock(&screenLock);
                check_rc(rc);
                Clock();
                printf("Η κράτηση ματαιώθηκε γιατί η συναλλαγή με την πιστωτική κάρτα δεν έγινε αποδεκτή\n\n");
                rc = pthread_mutex_unlock(&screenLock);
                check_rc(rc);
            }
        }
    }

    rc = pthread_mutex_lock(&operatorsLock);
    check_rc(rc);

    // Customer served successfully
    ++(*servedCounterPtr);
    rc = clock_gettime(CLOCK_REALTIME, &servEnd);
    check_rc(rc);
    ++telephonist;
    rc = pthread_cond_broadcast(&availableOperators);
    check_rc(rc);
    rc = pthread_mutex_unlock(&operatorsLock);
    check_rc(rc);

    rc = pthread_mutex_lock(&avgServingTimeLock);
    check_rc(rc);
    *avgServTimePtr = *avgServTimePtr + (servEnd.tv_sec - servStart.tv_sec);
    rc = pthread_mutex_unlock(&avgServingTimeLock);
    check_rc(rc);

    pthread_exit(NULL); //return
}

unsigned int sleepRandom(int min, int max) {
    return (rand_r(&seed) % (max - min + 1)) + min;
}

unsigned int choiceRandom(int min, int max) {
    return (rand_r(&seed) % (max - min + 1)) + min;
}

double cardRandom(double min, double max) {
    double f = (double) rand_r(&seed) / RAND_MAX;
    return min + f * (max - min);
}

void startTimer(){
    Clock();
    printf("Έναρξη Χρονομέτρησης\n\n");
    time_t t = time(NULL);
    start = *localtime(&t);
    clock_gettime(CLOCK_REALTIME, &requestStart);
}

void stopTimer(){
    Clock();
    printf("Λήξη Χρονομέτρησης\n\n");
    time_t t = time(NULL);
    end = *localtime(&t);
    clock_gettime(CLOCK_REALTIME, &requestEnd);
}

void Clock() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("[%02d:%02d:%02d] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

unsigned int receivePayment(int numOfSeats) {
    int rc = pthread_mutex_lock(&paymentLock);
    check_rc(rc);
    unsigned int cost = numOfSeats * C_SEAT;
    *profitPtr += (cost);
    rc = pthread_mutex_unlock(&paymentLock);
    check_rc(rc);
    return cost;
}

unsigned int logTransaction() {
    int rc = pthread_mutex_lock(&transactionLock);
    check_rc(rc);
    unsigned int transactionID = ++(*transactionsPtr);
    rc = pthread_mutex_unlock(&transactionLock);
    check_rc(rc);
    return transactionID;
}

void bookSeats(unsigned int numOfSeats, int custID) {
    int rc = pthread_mutex_lock(&seatsPlanLock);
    check_rc(rc);
    int temp = numOfSeats;
    for (int i = 0; temp > 0 && i < N_SEATS; i++) {
        if (seatsPlan[i] == 0) {
            seatsPlan[i] = custID;
            temp--;
        }
    }
    (*remainingSeatsPtr) -= numOfSeats;
    rc = pthread_mutex_unlock(&seatsPlanLock);
    check_rc(rc);
}

void unbookSeats(unsigned int numOfSeats, int custID) {
    int rc = pthread_mutex_lock(&seatsPlanLock);
    check_rc(rc);
    int temp = numOfSeats;
    for (int i = 0; temp > 0 && i < N_SEATS; i++) {
        if (seatsPlan[i] == custID) {
            seatsPlan[i] = 0;
            temp--;
        }
    }
    (*remainingSeatsPtr) += numOfSeats;
    rc = pthread_mutex_unlock(&seatsPlanLock);
    check_rc(rc);
}

void check_rc(int rc) {
    if (rc) {
        pthread_mutex_lock(&screenLock);
        Clock();
        printf("Σφάλμα: rc\n\n");
        printf("Έξοδος..");
        pthread_mutex_unlock(&screenLock);
        exit(-1);
    }
}

bool checkAvailableSeats(unsigned int choice) {
    int rc = pthread_mutex_lock(&seatsPlanLock);
    check_rc(rc);
    bool result = (choice <= (*remainingSeatsPtr));
    if (!result) {
        pthread_mutex_lock(&screenLock);
        Clock();
        printf("Η κράτηση ματαιώθηκε γιατί δεν υπάρχουν αρκετές διαθέσιμες θέσεις\n\n");
        pthread_mutex_unlock(&screenLock);
    }
    rc = pthread_mutex_unlock(&seatsPlanLock);
    check_rc(rc);
    return result;
}

bool checkRemainingSeats() {
    int rc = pthread_mutex_lock(&seatsPlanLock);
    check_rc(rc);
    bool result = (*remainingSeatsPtr == 0);
    rc = pthread_mutex_unlock(&seatsPlanLock);
    check_rc(rc);
    return result;
}

void printDuration(unsigned long int minutes, unsigned long int seconds, unsigned long int totalSeconds) {
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
}

void printSeatsPlan() {
    printf("Πλάνο Θέσεων:\n");
    int printCounter = 1;
    printf("| ");
    for (int i = 0; i < N_SEATS; i++) {
        if (seatsPlan[i] == 0) {
            printf("Θέση %03d: Πελάτης     | ", i + 1);
        } else {
            printf("Θέση %03d: Πελάτης %03d | ", i + 1, seatsPlan[i]);
        }
        if (printCounter == 6) {
            printf("\n| ");
            printCounter = 0;
        }
        printCounter += 1;
    }
}

void printInfo() {
    unsigned long int totalSeconds = requestEnd.tv_sec - requestStart.tv_sec;
    unsigned long int minutes = totalSeconds / 60;
    unsigned long int seconds = totalSeconds % 60;
    printf("Έναρξη [%02d:%02d:%02d] - ", start.tm_hour, start.tm_min, start.tm_sec);
    printf("Λήξη [%02d:%02d:%02d]\n", end.tm_hour, end.tm_min, end.tm_sec);
    printDuration(minutes, seconds, totalSeconds);
    printf("\n");
    printSeatsPlan();
    printf("\n\n");
    printf("Μέσος χρόνος αναμονής: %0.2f seconds\n", (double) avgWaitTime / customers);
    printf("Μέσος χρόνος εξυπηρέτησης: %0.2f seconds\n", (double) avgServTime / customers);
    printf("Εξυπηρετήθηκαν: %03d πελάτες\n", servedCounter);
    printf("Δεσμευμένες Θέσεις: %d\n", N_SEATS - (*remainingSeatsPtr));
    printf("Ελεύθερες Θέσεις: %d\n", (*remainingSeatsPtr));
    printf("Συναλλαγές: %d\n", (*transactionsPtr));
    printf("Κέρδη: %d\u20AC\n", (*profitPtr));
    printf("\nΈξοδος..\n");
}