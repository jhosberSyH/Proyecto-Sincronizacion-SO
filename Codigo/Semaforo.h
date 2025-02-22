#ifndef Semaforo_h
#define Semaforo_h
#include <pthread.h>

typedef struct {
    int valor;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}sem_t;

int sem_init(sem_t *sem,int valor);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);

int sem_init(sem_t *sem,int valor){
    sem->valor = valor;
    pthread_mutex_init(&sem->mutex,NULL);
    pthread_cond_init(&sem->cond,NULL);
}
int sem_destroy(sem_t *sem){
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
    return (0);
}
int sem_wait(sem_t *sem){
    pthread_mutex_lock(&sem->mutex);
    while (sem->valor <= 0){
        pthread_cond_wait(&sem->cond,&sem->mutex);
    }
    sem->valor--;
    pthread_mutex_unlock(&sem->mutex);
    return (0);
    
}
int sem_post(sem_t *sem){
    pthread_mutex_lock(&sem->mutex);
    sem->valor++;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
}

#endif
