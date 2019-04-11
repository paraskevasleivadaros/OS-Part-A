//
// Created by paras on 23-Mar-19.
//

#include "p3150173-p3150090-res1.h"

int profit = 0;
int transactions = 0;

int cust_id=0;

int N_TEL_LEFT = N_TEL;
int N_SEATS_LEFT = N_SEATS;

int random_number;

// Calculate time taken by a request
struct timespec requestStart, requestEnd;
struct tm start;
struct tm end;

void *bookSeats(void *);
int i_random(int, int);
double f_random(double, double);
void startTimer();
void stopTimer();
void showClock();

pthread_mutex_t lock= PTHREAD_MUTEX_INITIALIZER;;
pthread_mutex_t lock1= PTHREAD_MUTEX_INITIALIZER;;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]){

    int rc;
    pthread_t threads[N_TEL];
    int id[N_TEL];
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock1, NULL);

    // Converting string type to integer type
    // using function atoi
    unsigned int customers = atoi(argv[1]);
    unsigned int seed = atoi(argv[2]);

    // Checking if all provided numbers are positive
    if (customers <= 0 || seed <= 0){
        printf("Please enter only positive values in arguments!\n");
        exit(1);
    }

    printf("Customers to be served: %d\n\n", customers);

    random_number = rand_r(&seed);

    int N_CUST = customers;
    int flag =1;

    startTimer();

    while(customers>0 && flag){
        for (int i = 0; i < N_TEL; i++) {

            ++cust_id;
            id[i] = cust_id;

            if (cust_id>N_CUST) {
                --cust_id;
                flag = 0;
                break;
            }
            if ((rc = pthread_create(&threads[i], NULL, bookSeats, (void *) id[i]))) {
                showClock();
                printf("Thread Creation Error\n");
                exit(1);
            } else {
                --customers;
            }
        }
    }

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&lock1);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&cond1);

    sleep(5);

    stopTimer();
    printf("Start [%d:%d:%d]\n", start.tm_hour, start.tm_min, start.tm_sec);
    printf("End   [%d:%d:%d]\n\n", end.tm_hour, end.tm_min, end.tm_sec);
    long int totalSeconds = requestEnd.tv_sec-requestStart.tv_sec;
    long int minutes = totalSeconds/60;
    long int seconds = totalSeconds%60;
    double avTimePerCust = ((double)(totalSeconds)/N_CUST);
    printf("Duration: %ld minutes and %ld seconds (%lds)\n\n", minutes, seconds, totalSeconds);
    printf("Average time per customer: %0.2f seconds\n\n", avTimePerCust);
    printf("Number of customers served: %d\n", cust_id);
    printf("Number of seats booked: %d\n", N_SEATS-N_SEATS_LEFT);
    printf("Number of seats left: %d\n", N_SEATS_LEFT);
    printf("Transactions: %d\n", transactions);
    printf("Profits: %d\u20AC\n", profit);
    printf("\nExiting..\n");
}

void *bookSeats(void *x) {

    int id = (int) (int *) x;
    int rc;
    int rc1;

    rc = pthread_mutex_lock(&lock);

    showClock();
    printf("Telephonist %d in line.\n", (id%8+1));

    while (N_TEL_LEFT == 0) {
        showClock();
        printf("Customer %d couldn't find telephonist available. Blocked..\n", id);
        rc = pthread_cond_wait(&cond, &lock);
        rc = pthread_cond_wait(&cond1, &lock1);
    }

    showClock();
    printf("Customer %d being served..\n", id);
    --N_TEL_LEFT;
    rc = pthread_mutex_unlock(&lock);

    rc = pthread_mutex_lock(&lock1);
    int N_CHOICE = i_random(N_SEAT_LOW, N_SEAT_HIGH);

    if (N_CHOICE <= N_SEATS_LEFT || N_SEATS_LEFT>0) {

        sleep(i_random(T_SEAT_LOW, T_SEAT_HIGH));

        if (f_random(0.0, 1.0) < P_CARD_SUCCESS) {
            N_SEATS_LEFT -= N_CHOICE;
            profit += N_CHOICE * C_SEAT;
            ++transactions;
            showClock();
            printf("Customer %d seats booked\n", id);
        } else {
            showClock();
            printf("Card of Customer %d failed\n", id);
        }
    } else {
        showClock();
        printf("Not enough seats left to book.\n");
    }
    rc = pthread_cond_signal(&cond1);
    rc = pthread_mutex_unlock(&lock1);

    rc = pthread_mutex_lock(&lock);
    showClock();
    printf("Customer %d served successfully!\n", id);
    ++N_TEL_LEFT;
    rc = pthread_cond_signal(&cond);
    rc = pthread_mutex_unlock(&lock);
    pthread_exit(NULL); //return
    return NULL;
}

int i_random(int min, int max){
    return min + random_number / (RAND_MAX / (max - min + 1) + 1);
}

double f_random(double min, double max){
    double f = (double) random_number / RAND_MAX;
    return min + f * (max - min);
}

void startTimer(){
    showClock();
    printf("Starting Timer\n\n");
    time_t t = time(NULL);
    start = *localtime(&t);
    clock_gettime(CLOCK_REALTIME, &requestStart);
}

void stopTimer(){
    printf("\n");
    showClock();
    printf("Stopping Timer\n\n");
    time_t t = time(NULL);
    end = *localtime(&t);
    clock_gettime(CLOCK_REALTIME, &requestEnd);
}

void showClock(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("[%d:%d:%d] ", tm.tm_hour, tm.tm_min, tm.tm_sec);
}