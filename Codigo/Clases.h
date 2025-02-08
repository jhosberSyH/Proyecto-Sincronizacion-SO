#ifndef Clases_h
#define Clases_h
#include <stdio.h>
#include <string.h>

typedef struct {
	int id;
	char tipo[20];
	char ciudad[50];
    char pais[50];
    char vuelo[10];
    int prioridad;
    char fragilidad[5];
    float peso;
} Equipaje;

int traducirPrioridad(char prioridad[]){
    int res = 0;
    char Mano[] = "Mano",MANO[] = "MANO", mano[] = "mano";
    char Facturado[] = "Facturado",FACTURADO[] = "FACTURADO", facturado[] = "facturado";
    if(((strcmp(Mano,prioridad) == 0) || (strcmp(MANO,prioridad) == 0) || (strcmp(mano,prioridad) == 0))){
        res = 3;
    }else if(((strcmp(Facturado,prioridad) == 0) || (strcmp(FACTURADO,prioridad) == 0) || (strcmp(facturado,prioridad) == 0))){
        res = 2;
    }else{
        res = 1;
    }
    return (res);
}

void construtorEquipaje(Equipaje *equipaje){
    equipaje->id = 0;
    equipaje->peso = 0;
    equipaje->prioridad = 0;
}

typedef struct {
    int valor;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}sem_t;

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
