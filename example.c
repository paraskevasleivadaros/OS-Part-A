//
// Created by paras on 07-Apr-19.
//

pthread_mutex_t lock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int telephonist = 2; // available telephonists

int main() {

    int N = 10;
    int rc;
    pthread_t threads[N];
    int id[N];
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < N; i++) {
        id[i] = i + 1;
        printf("Main: creating thread %d\n", i + 1);
        rc = pthread_create(&threads[i], NULL, customer, id[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
}

void *customer(void *x) {

    int id = (int *) x;
    int rc;
    printf("Hello from customer: %d\n", id);
    rc = pthread_mutex_lock(&lock);

    while (tilefonites == 0) {
        printf("The customer %d, couldn't find telephonist available. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock);
    }

    printf("The customer %d, is being served.\n", id);
    tilefonites--;
    rc = pthread_mutex_unlock(&lock);

    sleep(5); // do some job with the telephonist

    rc = pthread_mutex_lock(&lock);
    printf("The customer %d was served successfully! \n", id);
    tilefonites++;
    rc = pthread_cond_signal(&cond);
    rc = pthread_mutex_unlock(&lock);
    pthread_exit(NULL); //return
}