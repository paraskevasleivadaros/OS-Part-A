//
// Created by paras on 23-Mar-19.
//

#include "p3150173-p3150090-p3120120-res1.h"

unsigned int seed;
unsigned int seats;
unsigned int customers;
unsigned int profit = 0;
unsigned int served = 0;
unsigned int transactions = 0;
unsigned int telephonist = N_TEL;
unsigned int remainingSeats = N_SEATS;

unsigned int seatsArray[N_SEATS];
unsigned int *profit_ptr = &profit;
unsigned int *served_ptr = &served;
unsigned int *transactions_ptr = &transactions;
unsigned int *remainingSeats_ptr = &remainingSeats;

// Calculate time taken by a request
struct timespec requestStart, requestEnd;
struct tm start;
struct tm end;

unsigned long int avgWaitTime;
unsigned long int avgServingTime;

unsigned long int *avgWaitTime_ptr = &avgWaitTime;
unsigned long int *avgServingTime_ptr = &avgServingTime;

unsigned int sleepRandom(int, int);
unsigned int choiceRandom(int, int);
double cardRandom(double, double);

void startTimer();
void stopTimer();
void Clock();

void checkRC(int);

unsigned int bank(int);
void bookSeats(unsigned int, int);
void unbookSeats(unsigned int, int);

void printArray(unsigned int *);
void printInfo();

pthread_mutex_t operatorsLock;
pthread_mutex_t bankLock;
pthread_mutex_t transactionLock;
pthread_mutex_t avgWaitTimeLock;
pthread_mutex_t avgServingTimeLock;
pthread_mutex_t arrayLock;
pthread_mutex_t screenLock;
pthread_cond_t cond;

bool checkAvailableSeats(unsigned int);
bool checkRemainingSeats();

unsigned int transaction();

int main(int argc, char* argv[]){

    if (argc != 3) {
        printf("Παρακαλώ δώστε μόνο το πλήθος των πελατών και τον σπόρο");
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
    checkRC(rc);
    rc = pthread_mutex_init(&bankLock, NULL);
    checkRC(rc);
    rc = pthread_mutex_init(&transactionLock, NULL);
    checkRC(rc);
    rc = pthread_mutex_init(&avgWaitTimeLock, NULL);
    checkRC(rc);
    rc = pthread_mutex_init(&avgServingTimeLock, NULL);
    checkRC(rc);
    rc = pthread_mutex_init(&arrayLock, NULL);
    checkRC(rc);
    rc = pthread_mutex_init(&screenLock, NULL);
    checkRC(rc);
    rc = pthread_cond_init(&cond, NULL);
    checkRC(rc);

    startTimer();
    for (int i = 0; i < customers; i++) {
        id[i] = i + 1;
        rc = pthread_create(&threads[i], NULL, customer, (void *) id[i]);
        checkRC(rc);
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
    checkRC(rc);

    clock_gettime(CLOCK_REALTIME, &waitStart);

    while (telephonist == 0) {
        // Customer couldn't find telephonist available. Blocked..
        rc = pthread_cond_wait(&cond, &operatorsLock);
        checkRC(rc);
    }

    clock_gettime(CLOCK_REALTIME, &waitEnd);

    // Customer being served..
    --telephonist;

    rc = pthread_mutex_unlock(&operatorsLock);
    checkRC(rc);

    rc = pthread_mutex_lock(&avgWaitTimeLock);
    checkRC(rc);
    *avgWaitTime_ptr = *avgWaitTime_ptr + (waitEnd.tv_sec - waitStart.tv_sec);
    rc = pthread_mutex_unlock(&avgWaitTimeLock);
    checkRC(rc);

    sleep(sleepRandom(T_SEAT_LOW, T_SEAT_HIGH));

    if (checkRemainingSeats()) {

        rc = pthread_mutex_lock(&screenLock);
        checkRC(rc);
        Clock();
        printf("Η κράτηση ματαιώθηκε γιατί το θέατρο είναι γεμάτο\n");
        rc = pthread_mutex_unlock(&screenLock);
        checkRC(rc);

    } else {

        if (checkAvailableSeats(seats = choiceRandom(N_SEAT_LOW, N_SEAT_HIGH))) {

            bookSeats(seats, id);

            if (cardRandom(0.0, 1.0) < P_CARD_SUCCESS) {

                unsigned int cost = bank(seats);
                unsigned int transactionID = transaction();

                rc = pthread_mutex_lock(&screenLock);
                checkRC(rc);
                Clock();

                printf("Η κράτηση ολοκληρώθηκε επιτυχώς. Ο αριθμός συναλλαγής είναι %03d", transactionID);

                rc = pthread_mutex_lock(&arrayLock);
                checkRC(rc);
                printf(", οι θέσεις σας είναι οι: ");
                for (int i = 0; i < N_SEATS; i++) {
                    if (seatsArray[i] == id) {
                        printf("%03d ", i + 1);
                    }
                }
                rc = pthread_mutex_unlock(&arrayLock);
                checkRC(rc);
                printf("και το κόστος της συναλλαγής είναι %03d\u20AC\n\n", cost);
                rc = pthread_mutex_unlock(&screenLock);
                checkRC(rc);

            } else {
                unbookSeats(seats, id);
                rc = pthread_mutex_lock(&screenLock);
                checkRC(rc);
                Clock();
                printf("Η κράτηση ματαιώθηκε γιατί η συναλλαγή με την πιστωτική κάρτα δεν έγινε αποδεκτή\n\n");
                rc = pthread_mutex_unlock(&screenLock);
                checkRC(rc);
            }
        }
    }

    rc = pthread_mutex_lock(&operatorsLock);
    checkRC(rc);

    // Customer served successfully
    ++(*served_ptr);
    rc = clock_gettime(CLOCK_REALTIME, &servEnd);
    checkRC(rc);
    ++telephonist;
    rc = pthread_cond_broadcast(&cond);
    checkRC(rc);
    rc = pthread_mutex_unlock(&operatorsLock);
    checkRC(rc);

    rc = pthread_mutex_lock(&avgServingTimeLock);
    checkRC(rc);
    *avgServingTime_ptr = *avgServingTime_ptr + (servEnd.tv_sec - servStart.tv_sec);
    rc = pthread_mutex_unlock(&avgServingTimeLock);
    checkRC(rc);

    pthread_exit(NULL); //return
}

unsigned int transaction() {
    int rc = pthread_mutex_lock(&transactionLock);
    checkRC(rc);
    unsigned int transactionID = ++(*transactions_ptr);
    rc = pthread_mutex_unlock(&transactionLock);
    checkRC(rc);
    return transactionID;
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
    printf("\n");
    Clock();
    printf("Σταμάτημα Χρονομέτρησης\n\n");
    time_t t = time(NULL);
    end = *localtime(&t);
    clock_gettime(CLOCK_REALTIME, &requestEnd);
}

void Clock() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("[%02d:%02d:%02d] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

unsigned int bank(int numOfSeats) {
    int rc = pthread_mutex_lock(&bankLock);
    checkRC(rc);
    unsigned int cost = numOfSeats * C_SEAT;
    *profit_ptr += (cost);
    rc = pthread_mutex_unlock(&bankLock);
    checkRC(rc);
    return cost;
}

void bookSeats(unsigned int numOfSeats, int custID) {
    int rc = pthread_mutex_lock(&arrayLock);
    checkRC(rc);
    int temp = numOfSeats;
    for (int i = 0; temp > 0 && i < N_SEATS; i++) {
        if (seatsArray[i] == 0) {
            seatsArray[i] = custID;
            temp--;
        }
    }
    (*remainingSeats_ptr) -= numOfSeats;
    rc = pthread_mutex_unlock(&arrayLock);
    checkRC(rc);
}

void unbookSeats(unsigned int numOfSeats, int custID) {
    int rc = pthread_mutex_lock(&arrayLock);
    checkRC(rc);
    int temp = numOfSeats;
    for (int i = 0; temp > 0 && i < N_SEATS; i++) {
        if (seatsArray[i] == custID) {
            seatsArray[i] = 0;
            temp--;
        }
    }
    (*remainingSeats_ptr) += numOfSeats;
    rc = pthread_mutex_unlock(&arrayLock);
    checkRC(rc);
}

void checkRC(int rc) {
    if (rc) {
        pthread_mutex_lock(&screenLock);
        printf("Σφάλμα: rc");
        pthread_mutex_unlock(&screenLock);
        exit(-1);
    }
}

bool checkAvailableSeats(unsigned int choice) {
    int rc = pthread_mutex_lock(&arrayLock);
    checkRC(rc);
    bool result = (choice <= (*remainingSeats_ptr));
    if (!result) {
        pthread_mutex_lock(&screenLock);
        Clock();
        printf("Η κράτηση ματαιώθηκε γιατί δεν υπάρχουν αρκετές διαθέσιμες θέσεις\n");
        pthread_mutex_unlock(&screenLock);
    }
    rc = pthread_mutex_unlock(&arrayLock);
    checkRC(rc);
    return result;
}

bool checkRemainingSeats() {
    int rc = pthread_mutex_lock(&arrayLock);
    checkRC(rc);
    bool result = (*remainingSeats_ptr == 0);
    rc = pthread_mutex_unlock(&arrayLock);
    checkRC(rc);
    return result;
}

void printArray(unsigned int *arr) {
    int printCounter = 1;
    printf("| ");
    for (int i = 0; i < N_SEATS; i++) {
        if (arr[i] == 0) {
            printf("Θέση %03d: Πελάτης     | ", i + 1);
        } else {
            printf("Θέση %03d: Πελάτης %03d | ", i + 1, arr[i]);
        }
        if (printCounter == 6) {
            printf("\n| ");
            printCounter = 0;
        }
        printCounter += 1;
    }
}

void printDuration(long int minutes, long int seconds, long int totalSeconds) {
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

void printInfo() {
    unsigned long int totalSeconds = requestEnd.tv_sec - requestStart.tv_sec;
    unsigned long int minutes = totalSeconds / 60;
    unsigned long int seconds = totalSeconds % 60;
    printf("Αρχή  [%d:%d:%d]\n", start.tm_hour, start.tm_min, start.tm_sec);
    printf("Τέλος [%d:%d:%d]\n", end.tm_hour, end.tm_min, end.tm_sec);
    printDuration(minutes, seconds, totalSeconds);
    printf("\n");
    printArray(seatsArray);
    printf("\n\n");
    printf("Μέσος χρόνος αναμονής: %0.2f seconds\n", (double) avgWaitTime / customers);
    printf("Μέσος χρόνος εξυπηρέτησης: %0.2f seconds\n", (double) avgServingTime / customers);
    printf("Εξυπηρετήθηκαν: %03d πελάτες\n", served);
    printf("Δεσμευμένες Θέσεις: %d\n", N_SEATS - (*remainingSeats_ptr));
    printf("Ελεύθερες Θέσεις: %d\n", (*remainingSeats_ptr));
    printf("Συναλλαγές: %d\n", (*transactions_ptr));
    printf("Κέρδη: %d\u20AC\n", (*profit_ptr));
    printf("\nΈξοδος..\n");
}