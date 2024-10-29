#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


int saldo = 100;
int read_count = 0;  // Contador de leitores
sem_t rw_mutex, mutex;  // Semáforos para controlar o acesso

// Função de leitura (somente leitura do saldo)
void *leitor(void *arg) {
    for (int i = 0; i < 5; i++) {
        sem_wait(&mutex);  // Protege o contador de leitores
        read_count++;
        if (read_count == 1) {
            sem_wait(&rw_mutex);  // O primeiro leitor bloqueia os escritores
        }
        sem_post(&mutex);

        // Simula a leitura do saldo
        printf("Leitor %ld: Consultou o saldo, valor atual: %d reais\n", (long)arg, saldo);
        sleep(1);  // Pausa para simular a leitura

        sem_wait(&mutex);  // Protege o contador de leitores
        read_count--;
        if (read_count == 0) {
            sem_post(&rw_mutex);  // O último leitor libera os escritores
        }
        sem_post(&mutex);
        sleep(1);  // Pausa entre as consultas
    }
    return NULL;
}

// Função de escrita (depósito ou saque)
void *escritor(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < 5; i++) {
        sem_wait(&rw_mutex);  // Escritor bloqueia leitores e outros escritores

        // Simula a modificação do saldo (depósito ou saque)
        if (id % 2 == 0) {  // Escritores com id par depositam
            saldo += 10;
            printf("Escritor %ld: Depositou 10 reais, saldo atual: %d reais\n", id, saldo);
        } else {  // Escritores com id ímpar sacam
            if (saldo >= 10) {
                saldo -= 10;
                printf("Escritor %ld: Sacou 10 reais, saldo atual: %d reais\n", id, saldo);
            } else {
                printf("Escritor %ld: Tentou sacar, mas saldo insuficiente.\n", id);
            }
        }
        sem_post(&rw_mutex);  // Libera o acesso para leitores e outros escritores

        sleep(1);  // Pausa para simular o tempo de escrita
    }
    return NULL;
}

int main() {
    // Inicializa os semáforos
    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);

    // Cria 3 threads de leitores e 2 de escritores
    pthread_t leitores[3], escritores[2];

    for (long i = 0; i < 3; i++) {
        pthread_create(&leitores[i], NULL, leitor, (void *)i);
    }
    for (long i = 0; i < 2; i++) {
        pthread_create(&escritores[i], NULL, escritor, (void *)i);
    }

    // Aguarda todas as threads terminarem
    for (int i = 0; i < 3; i++) {
        pthread_join(leitores[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(escritores[i], NULL);
    }

    // Destrói os semáforos
    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);

    printf("Operações concluídas, saldo final: %d reais\n", saldo);

    return 0;
}


