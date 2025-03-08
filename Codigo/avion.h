#ifndef Avion_h
#define Avion_h
#include "Cola.h"
#include "Equipaje.h"
#define MAX_AVIONES 200

typedef struct {
	int id;
    float capacidadMaxima; //Capacidad maxima del avión
    float capacidad; //Capacidad actual del avión

    //Estado del vuelo (Despegue, Conexion, Aterrizaje)
    char estado[30]; 
    /*"Despegue": Se llena de 0
    "Conexion": Ya lleva equipajes y solo se descargan unos pocos equipajes que se indiquen
    "Aterrizaje": Se descargan todos los equipajes del avión (Este no se ha aplicado)*/

    char nombre[50];
    char codigoVuelo[10];

    //Destino del avión
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
        //Crear aviones según archivo de entrada
        if (sscanf(buffer, "%s %s %s %s %f %s", a.codigoVuelo, a.nombre, a.ciudad, a.pais, &a.capacidadMaxima, a.estado)) {
            a.id=i; //Id del avión
            a.capacidad = a.capacidadMaxima;
            //Inicializar colas de equipajes
            crear(&a.equipajeEsp);
            crear(&a.equipajeSD);
            crear(&a.equipajes);
            /*if (strcmp(a.estado, "Conexion") == 0){
                printf("El vuelo %s (%s, %s) es de conexion\n", a.codigoVuelo, a.ciudad, a.pais);
            }*/
            aviones[i] = a;
            //printf("AVION: %i %s %s %s %s %f %s\n", aviones[i].id,aviones[i].codigoVuelo,aviones[i].nombre,aviones[i].ciudad,aviones[i].pais, aviones[i].capacidad, aviones[i].estado);
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
//CARGAR EQUIPAJE QUE YA ESTA EN EL AVIÓN SEGUN LA ENTRADA
int verificarEquipaje(Avion *avion, Equipaje *e, sem_t *semAvion){
    sem_wait(semAvion);
    int estado = 0;
    if(e->peso <= avion->capacidad){
        if(strcmp(e->tipo, "Facturado") == 0){
            encolar(&avion->equipajes, *e);
        }else{
            if(strcmp(e->tipo, "Especial") == 0){
                encolar(&avion->equipajeEsp, *e);

            }else{
                if(strcmp(e->tipo, "Sobredimensionado") == 0){
                    encolar(&avion->equipajeSD, *e);

                }
            }
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
        fprintf(avionesLog,"\nAVION NUMERO %i (%s)\n", i, aviones[i].estado);
        fprintf(avionesLog,"DESTINO: (%s, %s)\n", aviones[i].ciudad, aviones[i].pais);
        fprintf(avionesLog,"Capacidad inicial: %f\n", aviones[i].capacidadMaxima);
        fprintf(avionesLog,"Capacidad Usada: %f\n", aviones[i].capacidadMaxima - aviones[i].capacidad);
        fprintf(avionesLog,"Capacidad restante: %f\n", aviones[i].capacidad);
        fprintf(avionesLog,"Equipaje Especial: %i\n", longitud(aviones[i].equipajeEsp));
        fprintf(avionesLog,"Equipaje Facturado: %i\n", longitud(aviones[i].equipajes));
        fprintf(avionesLog,"Equipaje sobredimensionado: %i\n", longitud(aviones[i].equipajeSD));
        fprintf(avionesLog,"\nEQUIPAJES\n");
        while (esVacio(aviones[i].equipajeEsp) == 0){
            fprintf(avionesLog,"%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajeEsp).id, primero(aviones[i].equipajeEsp).ciudad, primero(aviones[i].equipajeEsp).pais, primero(aviones[i].equipajeEsp).prioridad, primero(aviones[i].equipajeEsp).peso, primero(aviones[i].equipajeEsp).estado);
            desencolar(&aviones[i].equipajeEsp);
        }
        fprintf(avionesLog,"------------------\n");
        while (esVacio(aviones[i].equipajes) == 0){
            fprintf(avionesLog,"%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajes).id, primero(aviones[i].equipajes).ciudad, primero(aviones[i].equipajes).pais, primero(aviones[i].equipajes).prioridad, primero(aviones[i].equipajes).peso, primero(aviones[i].equipajes).estado);
            desencolar(&aviones[i].equipajes);
        }
        fprintf(avionesLog,"----------------\n");
        while (esVacio(aviones[i].equipajeSD) == 0){
            fprintf(avionesLog,"%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajeSD).id, primero(aviones[i].equipajeSD).ciudad, primero(aviones[i].equipajeSD).pais, primero(aviones[i].equipajeSD).prioridad, primero(aviones[i].equipajeSD).peso, primero(aviones[i].equipajeSD).estado);
            desencolar(&aviones[i].equipajeSD);
        }
    }
}
#endif