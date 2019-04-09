//
// Created by paras on 23-Mar-19.
//

#include "p3150173-p3150090-res1.h"

void *bookSeats(void *);
int random1(int, int);
double f_random(double, double);
void startTimer();
void stopTimer();

int profit = 0;
int transactions = 0;

int N_TEL_LEFT = N_TEL;
int N_SEATS_LEFT = N_SEATS;

// Calculate time taken by a request
struct timespec requestStart, requestEnd;

pthread_mutex_t lock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]){

    int rc;
    pthread_t threads[N_TEL];
    int id[N_TEL];
    pthread_mutex_init(&lock, NULL);

    // Converting string type to integer type
    // using function atoi
    int customers = atoi(argv[1]);
    int seed = atoi(argv[2]);
    srand(seed);

    // Checking if all provided numbers are positive
    if (customers <= 0 || seed <= 0){
        printf("Please enter only positive values in arguments!");
        exit(1);
    }

    startTimer();

    printf("Customers to be served: %d\n", customers);

    int N_CUST = customers;
    int counter=0;
    int flag =1;

    while(customers>0 && flag){
        for (int i = 0; i < N_TEL; i++) {

            id[i] = i + 1;
            ++counter;

            if (counter>N_CUST) {
                --counter;
                flag = 0;
                break;
            }

            printf("Creating Thread %d\n", counter);
            if ((rc= pthread_create(&threads[i], NULL, bookSeats, (void *) id[i]))) {
                printf("Thread Creation Error");
                exit(1);
            } else {
                --customers;
            }
        }

        for (int i = 0; i < N_TEL; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    stopTimer();

    printf("Duration: %ld\n\n", (requestEnd.tv_sec-requestStart.tv_sec));

    printf("Number of customers served: %d\n", counter);
    printf("Number of seats booked: %d\n", N_SEATS_LEFT+N_SEATS);
    printf("Number of transactions: %d\n", transactions);
    printf("Profits: %d\n", profit);

    printf("\nExiting..");
}

void *bookSeats(void *x) {

    int id = (int) (int *) x;
    int rc;
    printf("Telephonist %d in line.\n", id);
    rc = pthread_mutex_lock(&lock);

    while (N_TEL_LEFT == 0) {
        printf("Customer %d couldn't find telephonist available. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock);
    }

    printf("Customer %d being served.\n", id);
    --N_TEL_LEFT;
    rc = pthread_mutex_unlock(&lock);

    int N_CHOICE = random1(N_SEAT_LOW, N_SEAT_HIGH);

    if (N_CHOICE <= N_SEATS_LEFT) {

        sleep(random1(T_SEAT_LOW, T_SEAT_HIGH));
        N_SEATS_LEFT -= N_CHOICE;

        if (f_random(0.0, 1.0) < P_CARD_SUCCESS) {
            profit += N_CHOICE * C_SEAT;
            ++transactions;
            printf("Customer %d seats booked\n", id);
        } else {
            N_SEATS_LEFT += N_CHOICE;
            printf("Card of Customer %d failed\n", id);
        }

    } else {
        printf("Not enough seats left to book.\n");
    }

    rc = pthread_mutex_lock(&lock);
    printf("Customer %d served successfully!\n", id);
    N_TEL_LEFT++;
    rc = pthread_cond_signal(&cond);
    rc = pthread_mutex_unlock(&lock);
    pthread_exit(NULL); //return
    return NULL;
}

int random1(int min, int max){
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

double f_random(double min, double max){
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}

void startTimer(){
    printf("Starting Clock\n\n");
    clock_gettime(CLOCK_REALTIME, &requestStart);
}

void stopTimer(){
    printf("\nStopping Clock\n\n");
    clock_gettime(CLOCK_REALTIME, &requestEnd);
}