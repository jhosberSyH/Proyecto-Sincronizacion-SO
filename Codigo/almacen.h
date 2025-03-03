#ifndef Almacen_h
#define Almacen_h
#include "Cola.h"
#define MAX_ALMACEN 250
#define MAX_CAPACIDAD_ALMACEN 1000

typedef struct {
	int id;
    int capacidad;
    char pais[50];
    Cola equipajes;
} Almacen;

void constructorAlmacen(Almacen almacen[]);
void almacenar(Almacen *almacen,Equipaje equipaje);
int compararPais(char pais[],Almacen *almacen);

void constructorAlmacen(Almacen almacen[]){
    int i;
    for (i = 0; i < MAX_ALMACEN; i++){
        almacen[i].id = i;
        almacen[i].capacidad = 0;
    }
    
}

void almacenar(Almacen *almacen,Equipaje equipaje){
    encolarPrioridad(&almacen->equipajes,equipaje);
    almacen->capacidad += 1;
}

int compararPais(char pais[],Almacen *almacen){
    int res = 0;
    if(strlen(almacen->pais) == 0){   
        res = 1;
        strcpy(almacen->pais,pais);
    }else if ((strcmp(pais,almacen->pais) == 0) && (almacen->capacidad < MAX_CAPACIDAD_ALMACEN)){
        res = 1;
    }
    return(res);
}

#endif