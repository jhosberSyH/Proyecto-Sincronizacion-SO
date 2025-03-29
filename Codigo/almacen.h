#ifndef Almacen_h
#define Almacen_h
#include "Cola.h"
#include "avion.h"
#define MAX_ALMACEN 250
#define MAX_CAPACIDAD_ALMACEN 1000

typedef struct {
	int id;
    int capacidad;
    char pais[50];
    Cola equipajeEsp; //Equipaje especial (MAXIMA PRIORIDAD)
    Cola equipajes;   //Equipaje facturado (PRIORIDAD MEDIA)
    Cola equipajeSD;  //Equipaje sobredimensionado (PRIORIDAD BAJA)
    int lleno;  //Espacios del almacén llenos
    int esPerdido; //Bandera para saber si el equipaje es perdido
} Almacen;

FILE *almacenLog;
FILE *finalAlmacen;

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
        almacen[i].lleno = 0;
        almacen[i].esPerdido = 0;
        //INICIALIZAR COLAS DE EQUIPAJES
        crear(&almacen[i].equipajeEsp);
        crear(&almacen[i].equipajes);
        crear(&almacen[i].equipajeSD);
    }    
}

void constructorAlmacenPerdidos(Almacen *almacen){
    almacen->id = 1;
    almacen->capacidad = MAX_CAPACIDAD_ALMACEN;
    almacen->lleno = 0;
    almacen->esPerdido = 1;
    //INICIALIZAR COLAS DE EQUIPAJES
    crear(&almacen->equipajeEsp);
    crear(&almacen->equipajes);
    crear(&almacen->equipajeSD);
}

void escribirAlmacenado(Almacen almacen, Equipaje e ){
    //printf("aca");
    fprintf(almacenLog, "El Equipaje %s número %i con peso %.2f y destino (%s,%s) en el almacén  %i (Prioridad %i)\n",e.tipo, e.id, e.peso, e.ciudad, e.pais, almacen.id, e.prioridad);
}
void escribirNoAlmacenado(Almacen almacen){
    //printf("aca tmb");
    //fprintf(almacenLog, "NO CABE YA\n");
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
    if(!almacen->esPerdido){
        almacen->capacidad -= 1;
    }
    almacen->lleno += 1;
    escribirAlmacenado(*almacen, equipaje);
    return 1;
}
void descargarAlmacen(Almacen *almacen, Avion aviones[MAX_AVIONES], sem_t semAviones[MAX_AVIONES], Equipaje *e){

    if(almacen->lleno > 0){
        if(esVacio(almacen->equipajeEsp) == 0){
            *e = primero(almacen->equipajeEsp);
            desencolar(&almacen->equipajeEsp);
        }else{
            if(esVacio(almacen->equipajes) == 0){
                *e = primero(almacen->equipajes);
                desencolar(&almacen->equipajes);
            }else{
                if(esVacio(almacen->equipajeSD) == 0){
                    *e = primero(almacen->equipajeSD);
                    desencolar(&almacen->equipajeSD);
                }
            }
        }
        //FALTA HACER IMPRESIONES DE LOGS
        //CARGAR EQUIPAJE AL AVIÓN
        //printf("SE VA A CARGAR %i, %s\n", e.idVuelo, aviones[e.idVuelo].ciudad);
        almacen->lleno -=1;
        almacen->capacidad +=1;
    }
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
    for (i = 0; i < MAX_ALMACEN; i++){
        fprintf(finalAlmacen,"\nALMACEN NUMERO %i\n", i);
        fprintf(finalAlmacen,"Equipaje especial: %i\n", longitud(almacen[i].equipajeEsp));
        fprintf(finalAlmacen,"Equipaje facturado: %i\n", longitud(almacen[i].equipajes));
        fprintf(finalAlmacen,"Equipaje sobredimensionado: %i\n", longitud(almacen[i].equipajeSD));
    }
}
#endif