//
// Created by paras on 07-Apr-19.
//

pthread_mutex_t lock;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int tilefonites = 2; //diathesimoi tilefonites
void *pelatis(void *x) {
    int id = (int *) x;
    int rc;
    printf("Hello from pelati: %d\n", id);
    rc = pthread_mutex_lock(&lock);

    while (tilefonites == 0) {
        printf("O pelatis %d, den brike diathesimo tilefoniti. Blocked...\n", id);
        rc = pthread_cond_wait(&cond, &lock);
    }

    printf("O pelatis %d, eksipiretite.\n", id);
    tilefonites--;
    rc = pthread_mutex_unlock(&lock);

    sleep(5); //kane kapoia douleia me ton tilefoniti

    rc = pthread_mutex_lock(&lock);
    printf("O pelatis %d eksipiretithike epitixos! \n", id);
    tilefonites++;
    rc = pthread_cond_signal(&cond);
    rc = pthread_mutex_unlock(&lock);
    pthread_exit(NULL); //return
}

int main() {
    int N = 10;
    int rc;
    pthread_t threads[N];
    int id[N];
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < N; i++) {

        id[i] = i + 1;
        printf("Main: creating thread %d\n", i + 1);
        rc = pthread_create(&threads[i], NULL, pelatis, id[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    return 0;
}
