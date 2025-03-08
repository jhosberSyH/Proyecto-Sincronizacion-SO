#ifndef Avion_h
#define Avion_h
#include "Cola.h"
#include "Equipaje.h"
#define MAX_AVIONES 100

typedef struct {
	int id;
    float capacidad;
    float capacidadMaxima;
    char nombre[50];
    char codigoVuelo[10];
    char ciudad[50];
    char pais[50];
    Cola equipajeEsp; //Equipaje especial (MAXIMA PRIORIDAD)
    Cola equipajes;   //Equipaje facturado (PRIORIDAD MEDIA)
    Cola equipajeSD;  //Equipaje sobredimensionado (PRIORIDAD BAJA)
} Avion;


FILE *avionesLog;

void crearAviones(Avion aviones[MAX_AVIONES], int *cantidadAviones){
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
            a.capacidadMaxima = a.capacidad;
            crear(&a.equipajeEsp);
            crear(&a.equipajeSD);
            crear(&a.equipajes);
            aviones[i] = a;
            //printf("AVION: %i %s %s %s %s %f\n", aviones[i].id,aviones[i].codigoVuelo,aviones[i].nombre,aviones[i].ciudad,aviones[i].pais, aviones[i].capacidad);
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

void verAviones(Avion aviones[MAX_AVIONES], int cantidad){
    int i;

    for (i = 0; i < cantidad; i++){
        fprintf(avionesLog,"\nAVION NUMERO %i\n", i);
        fprintf(avionesLog,"DESTINO: (%s, %s)\n", aviones[i].ciudad, aviones[i].pais);
        fprintf(avionesLog,"Capacidad inicial: %f\n", aviones[i].capacidadMaxima);
        fprintf(avionesLog,"Capacidad Usada: %f\n", aviones[i].capacidadMaxima - aviones[i].capacidad);
        fprintf(avionesLog,"Capacidad restante: %f\n", aviones[i].capacidad);
        fprintf(avionesLog,"Equipaje Especial: %i\n", longitud(aviones[i].equipajeEsp));
        fprintf(avionesLog,"Equipaje Facturado: %i\n", longitud(aviones[i].equipajes));
        fprintf(avionesLog,"Equipaje sobredimensionado: %i\n", longitud(aviones[i].equipajeSD));
        fprintf(avionesLog,"\nEQUIPAJES\n");
        while (esVacio(aviones[i].equipajeEsp) == 0){
            fprintf(avionesLog,"%i (%s,%s) [prioridad: %i] peso: %f\n", primero(aviones[i].equipajeEsp).id, primero(aviones[i].equipajeEsp).ciudad, primero(aviones[i].equipajeEsp).pais, primero(aviones[i].equipajeEsp).prioridad, primero(aviones[i].equipajeEsp).peso);
            desencolar(&aviones[i].equipajeEsp);
        }
        fprintf(avionesLog,"------------------\n");
        while (esVacio(aviones[i].equipajes) == 0){
            fprintf(avionesLog,"%i (%s,%s) [prioridad: %i] peso: %f\n", primero(aviones[i].equipajes).id, primero(aviones[i].equipajes).ciudad, primero(aviones[i].equipajes).pais, primero(aviones[i].equipajes).prioridad, primero(aviones[i].equipajes).peso);
            desencolar(&aviones[i].equipajes);
        }
        fprintf(avionesLog,"----------------\n");
        while (esVacio(aviones[i].equipajeSD) == 0){
            fprintf(avionesLog,"%i (%s,%s) [prioridad: %i] peso: %f\n", primero(aviones[i].equipajeSD).id, primero(aviones[i].equipajeSD).ciudad, primero(aviones[i].equipajeSD).pais, primero(aviones[i].equipajeSD).prioridad, primero(aviones[i].equipajeSD).peso);
            desencolar(&aviones[i].equipajeSD);
        }
    }
}
#endif