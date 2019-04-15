//
// Created by paraskevas on 15/4/2019.
//

#include "p3150173-p3150090-p3120120-res1.h"

pthread_mutex_t lock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int telephonist = 2; // available telephonists
unsigned int choice[100];

int main(int argc, char *argv[]) {


    int N = 10;
    int rc;
    pthread_t threads[N];
    int id[N];

    void *customer(void *x);

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < N; i++) {
        id[i] = i + 1;
        // printf("Main: creating thread %d\n", i + 1);
        rc = pthread_create(&threads[i], NULL, customer, (void *) id[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
}

void *customer(void *x) {

    int id = (int) (int *) x;
    choice[id] = id;
    int rc;
    printf("%d %d\n", id, choice[id]);
    rc = pthread_mutex_lock(&lock);

    while (telephonist == 0) {
        // printf("The customer %d, couldn't find telephonist available. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock);
    }

    // printf("The customer %d, is being served.\n", id);
    telephonist--;
    rc = pthread_mutex_unlock(&lock);

    // sleep(5); // do some job with the telephonist

    rc = pthread_mutex_lock(&lock);
    // printf("The customer %d was served successfully! \n", id);
    telephonist++;
    rc = pthread_cond_signal(&cond);
    rc = pthread_mutex_unlock(&lock);
    pthread_exit(NULL); //return
}