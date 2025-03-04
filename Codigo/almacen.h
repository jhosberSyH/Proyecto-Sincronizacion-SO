#ifndef Almacen_h
#define Almacen_h
#include "Cola.h"
#define MAX_ALMACEN 250
#define MAX_CAPACIDAD_ALMACEN 1000

typedef struct {
	int id;
    int capacidad;
    char pais[50];
    Cola equipajeEsp; //Equipaje especial (MAXIMA PRIORIDAD)
    Cola equipajes;   //Equipaje facturado (PRIORIDAD MEDIA)
    Cola equipajeSD;  //Equipaje sobredimensionado (PRIORIDAD BAJA)
} Almacen;

void constructorAlmacen(Almacen almacen[]);
int almacenar(Almacen *almacen,Equipaje equipaje);
int compararPais(char pais[],Almacen *almacen);
void escribirAlmacenado(Almacen almacen, Equipaje e );
void escribirNoAlmacenado(Almacen almacen);

void constructorAlmacen(Almacen almacen[]){
    int i;
    for (i = 0; i < MAX_ALMACEN; i++){
        almacen[i].id = i;
        almacen[i].capacidad = MAX_CAPACIDAD_ALMACEN;
    }    
}

void escribirAlmacenado(Almacen almacen, Equipaje e ){
    FILE *f = fopen("../pruebas/logAlmacen.txt", "a");
    //printf("aca");
    fprintf(f, "Equipaje %i (%s,%s) en el almacén  %i (Prioridad %i)\n", e.id, e.ciudad, e.pais, almacen.id, e.prioridad);
    fclose(f);
}
void escribirNoAlmacenado(Almacen almacen){
    FILE *f = fopen("../pruebas/logAlmacen.txt", "a");
    //printf("aca tmb");
    fprintf(f, "NO CABE YA\n");
    fclose(f);
}

int almacenar(Almacen *almacen,Equipaje equipaje){
    int prioridad = equipaje.prioridad;
    //Encolar en la cola correspondiente a su prioridad
    if(almacen->capacidad <= 0 ){
        escribirNoAlmacenado(*almacen);
        return 0;
    }
    if (prioridad == 1){
        encolar(&almacen->equipajeEsp, equipaje);
    }else{
        if(prioridad == 2){
            encolar(&almacen->equipajes, equipaje);
        }else{
            encolar(&almacen->equipajeSD, equipaje);
        }
    }
    //encolarPrioridad(&almacen->equipajes,equipaje);
    almacen->capacidad -= 1;
    escribirAlmacenado(*almacen, equipaje);
    return 1;
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


void verColasAlmacenes(Almacen almacen[]){
    int i;
    FILE *f = fopen("../pruebas/finalAlmacen.txt", "w");

    for (i = 0; i < MAX_ALMACEN; i++){
        fprintf(f,"\nALMACEN NUMERO %i\n", i);
        fprintf(f,"Equipaje especial: %i\n", longitud(almacen[i].equipajeEsp));
        fprintf(f,"Equipaje facturado: %i\n", longitud(almacen[i].equipajes));
        fprintf(f,"Equipaje sobredimensionado: %i\n", longitud(almacen[i].equipajeSD));
    }
    fclose(f);
}
#endif