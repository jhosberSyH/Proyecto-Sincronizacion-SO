#ifndef Avion_h
#define Avion_h
#include "Cola.h"
#include "Equipaje.h"

typedef struct {
	int id;
    float capacidad;
    char nombre[50];
    char codigoVuelo[10];
    char ciudad[50];
    char pais[50];
    Cola equipajeEsp; //Equipaje especial (MAXIMA PRIORIDAD)
    Cola equipajes;   //Equipaje facturado (PRIORIDAD MEDIA)
    Cola equipajeSD;  //Equipaje sobredimensionado (PRIORIDAD BAJA)
} Avion;



void crearAviones(Avion aviones[], int *cantidadAviones){
    Avion a;
    int i=0;
    FILE *file = fopen("../pruebas/vuelos.txt", "r");
    char buffer[256];
    if (file == NULL) {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (sscanf(buffer, "%s %s %s %s %f", a.codigoVuelo, a.nombre, a.ciudad, a.pais, &a.capacidad)) {
            a.id=i;
            crear(&a.equipajeEsp);
            crear(&a.equipajeSD);
            crear(&a.equipajes);
            aviones[i] = a;
            //printf("AVION: %i %s %s %s %s %i\n", aviones[i].id,aviones[i].codigoVuelo,aviones[i].nombre,aviones[i].ciudad,aviones[i].pais, aviones[i].capacidad);
            i++;
            *cantidadAviones = *cantidadAviones + 1;
        }
    }
    fclose(file); 
}
int cargarEquipaje(Avion *avion, Equipaje *e, sem_t *semAvion){
    sem_wait(semAvion);
    int estado = 0;
    if(e->peso <= avion->capacidad){
        switch(e->prioridad){
            case 1:
                encolar(&avion->equipajeEsp, *e);
                break;
            case 2:
                encolar(&avion->equipajes, *e);
                break;
            case 3:
                encolar(&avion->equipajeSD, *e);
                break;
            default:
                break;
        }
        avion->capacidad -= e->peso;
        estado = 1;
    }
    sem_post(semAvion);
    return estado;
}
#endif