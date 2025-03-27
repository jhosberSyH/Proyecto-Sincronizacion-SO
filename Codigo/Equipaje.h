#ifndef Equipaje_h
#define Equipaje_h
#include <stdio.h>
#include <string.h>
#include "Cola.h"

typedef struct {
	int id;
	char tipo[20];
	char ciudad[50];
    char pais[50];
    char vuelo[10];
    int idVuelo;
    char codVuelo[10];
    int prioridad;
    char estado[20];
    char fragilidad[5];
    float peso;
} Equipaje;

void construtorEquipaje(Equipaje *equipaje);
int traducirPrioridad(char prioridad[]);

void construtorEquipaje(Equipaje *equipaje){
    equipaje->id = 0;
    equipaje->peso = 0;
    equipaje->prioridad = 0;
}

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

#endif
