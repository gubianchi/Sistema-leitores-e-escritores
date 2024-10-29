#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t rw_mutex, mutex;
int read_count = 0;

void *reader(void *arg) {
    sem_wait(&mutex);
    read_count++;
    if (read_count == 1) {
        sem_wait(&rw_mutex);  // Bloqueia o acesso para escritores
    }
    sem_post(&mutex);

    printf("Leitor %ld lendo...\n", (long)arg);

    sem_wait(&mutex);
    read_count--;
    if (read_count == 0) {
        sem_post(&rw_mutex);  // Libera o acesso para escritores
    }
    sem_post(&mutex);

    return NULL;
}

void *writer(void *arg) {
    sem_wait(&rw_mutex);

    printf("Escritor %ld escrevendo...\n", (long)arg);

    sem_post(&rw_mutex);

    return NULL;
}

int main() {
    pthread_t r[5], w[5];

    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);

    for (long i = 0; i < 5; i++) {
        pthread_create(&r[i], NULL, reader, (void *)i);
        pthread_create(&w[i], NULL, writer, (void *)i);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(r[i], NULL);
        pthread_join(w[i], NULL);
    }

    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);

    return 0;
}
