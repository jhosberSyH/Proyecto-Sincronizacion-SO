#ifndef Avion_h
#define Avion_h
#include "Cola.h"
#include "Equipaje.h"
#define MAX_AVIONES 200

typedef struct
{
    int id;
    float capacidadMaxima; // Capacidad maxima del avión
    float capacidad;       // Capacidad actual del avión

    // Estado del vuelo (Despegue, Conexion, Aterrizaje)
    char estado[30];
    /*"Despegue": Se llena de 0
    "Conexion": Ya lleva equipajes y solo se descargan unos pocos equipajes que se indiquen
    "Aterrizaje": Se descargan todos los equipajes del avión (Este no se ha aplicado)*/

    char nombre[50];
    char codigoVuelo[10];

    // Destino del avión
    char ciudad[50];
    char pais[50];

    //Banderas de estados
    int faltaDescargar;
    int estaLleno;

    Cola enEspera; //Equipajes al lado del avión esperando ser cargados a este

    Cola equipajeEsp;  // Equipaje especial (MAXIMA PRIORIDAD)
    Cola equipajes;    // Equipaje facturado (PRIORIDAD MEDIA)
    Cola equipajeSD;   // Equipaje sobredimensionado (PRIORIDAD BAJA)
    Cola equipajeMano; // Equipaje de mano (VAN EN LA CABINA DEL AVION)
} Avion;

int buscarEquipajeFacturado(Avion *avion, Equipaje *e);
int buscarEquipajeEspecial(Avion *avion, Equipaje *e);
int buscarEquipajeMano(Avion *avion, Equipaje *e);
int buscarEquipajeSobredimensionado(Avion *avion, Equipaje *e);

FILE *avionesLog;

void crearAviones(Avion aviones[MAX_AVIONES], int *cantidadAviones)
{
    Avion a;
    int i = 0;
    FILE *file = fopen("../pruebas/vuelos.txt", "r");
    char buffer[256];
    if (file == NULL)
    {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        // Crear aviones según archivo de entrada
        if (sscanf(buffer, "%s %s %s %s %f %s", a.codigoVuelo, a.nombre, a.ciudad, a.pais, &a.capacidadMaxima, a.estado))
        {
            a.id = i; // Id del avión
            a.capacidad = a.capacidadMaxima;
            a.estaLleno = 0;
            a.faltaDescargar = 0;

            if(strcmp(a.estado, "Conexion") == 0){
                a.faltaDescargar = 1;
            }

            // Inicializar colas de equipajes
            crear(&a.enEspera);
            crear(&a.equipajeEsp);
            crear(&a.equipajeSD);
            crear(&a.equipajes);
            crear(&a.equipajeMano);
            /*if (strcmp(a.estado, "Conexion") == 0){
                printf("El vuelo %s (%s, %s) es de conexion\n", a.codigoVuelo, a.ciudad, a.pais);
            }*/
            aviones[i] = a;
            // printf("AVION: %i %s %s %s %s %f %s\n", aviones[i].id,aviones[i].codigoVuelo,aviones[i].nombre,aviones[i].ciudad,aviones[i].pais, aviones[i].capacidad, aviones[i].estado);
            i++;
            *cantidadAviones = *cantidadAviones + 1;
        }
    }
    fclose(file);
}
int cargarEquipaje(Avion *avion, Equipaje *e)
{

    int estado = 0;
    if (e->peso <= avion->capacidad)
    {
        switch (e->prioridad)
        {
        case 1:
            encolar(&avion->equipajeEsp, *e);
            break;
        case 2:
            encolar(&avion->equipajes, *e);
            break;
        case 3:
            encolar(&avion->equipajeSD, *e);
            break;
        case 4:
            encolar(&avion->equipajeMano, *e);
            break;
        default:
            break;
        }
        if (e->prioridad != 4)
        {
            avion->capacidad -= e->peso;
        }
        estado = 1;
    }else{
        //SI ESTA FULL Y NO FALTA DESCARGAR SE INDICA COMO LLENO
        if((avion->faltaDescargar == 0) && (avion->capacidad < 1.0f)){
            avion->estaLleno = 1;
        }else{
            if(avion->faltaDescargar == 1){
                estado = 2;
            }
        }
    }

    return estado;
}

// CARGAR EQUIPAJE QUE YA ESTA EN EL AVIÓN SEGUN LA ENTRADA
int verificarEquipaje(Avion *avion, Equipaje *e, sem_t *semAvion)
{
    sem_wait(semAvion);
    int estado = 0;
    if (e->peso <= avion->capacidad)
    {
        if (strcmp(e->tipo, "Facturado") == 0)
        {
            encolar(&avion->equipajes, *e);
        }
        else
        {
            if (strcmp(e->tipo, "Especial") == 0)
            {
                encolar(&avion->equipajeEsp, *e);
            }
            else
            {
                if (strcmp(e->tipo, "Sobredimensionado") == 0)
                {
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

int descargarEquipaje(Avion *avion, Equipaje *e)
{
    int encontrado = 0;
    int i = 0;
    // printf("1: \n");
    if (buscarEquipajeEspecial(avion, e))
    {
        encontrado = 1;
    }
    else
    {
        // printf("2: \n");
        if (buscarEquipajeFacturado(avion, e))
        {
            encontrado = 1;
        }
        else
        {
            // printf("3: \n");
            encontrado = buscarEquipajeSobredimensionado(avion, e);
        }
    }
    if (encontrado)
    {
        avion->capacidad += e->peso;
    }else{
        avion->faltaDescargar = 0;
    }
    return encontrado;
}

int buscarEquipajeEspecial(Avion *avion, Equipaje *e)
{
    int encontrado = 0;
    int i = 0;
    while (((encontrado == 0) && (longitud(avion->equipajeEsp) > 0)) && (i < longitud(avion->equipajeEsp)))
    {
        if (strcmp(primero(avion->equipajeEsp).estado, "Descargar") == 0)
        {
            encontrado = 1;
            *e = primero(avion->equipajeEsp);
        }
        else
        {
            encolar(&avion->equipajeEsp, primero(avion->equipajeEsp));
        }
        desencolar(&avion->equipajeEsp);
        i++;
    }
    return encontrado;
}
int buscarEquipajeFacturado(Avion *avion, Equipaje *e)
{
    int encontrado = 0;
    int i = 0;
    while (((encontrado == 0) && (longitud(avion->equipajes) > 0)) && (i < longitud(avion->equipajes)))
    {
        if (strcmp(primero(avion->equipajes).estado, "Descargar") == 0)
        {
            encontrado = 1;
            *e = primero(avion->equipajes);
        }
        else
        {
            encolar(&avion->equipajes, primero(avion->equipajes));
        }
        desencolar(&avion->equipajes);
        i++;
    }
    return encontrado;
}

int buscarEquipajeSobredimensionado(Avion *avion, Equipaje *e)
{
    int encontrado = 0;
    int i = 0;
    while (((encontrado == 0) && (longitud(avion->equipajeSD) > 0)) && (i < longitud(avion->equipajeSD)))
    {
        if (strcmp(primero(avion->equipajeSD).estado, "Descargar") == 0)
        {
            encontrado = 1;
            *e = primero(avion->equipajeSD);
        }
        else
        {
            encolar(&avion->equipajeSD, primero(avion->equipajeSD));
        }
        desencolar(&avion->equipajeSD);
        i++;
    }
    return encontrado;
}

int buscarEquipajeDeMano(Avion *avion, Equipaje *e)
{
    int encontrado = 0;
    int i = 0;
    while (((encontrado == 0) && (longitud(avion->equipajeMano) > 0)) && (i < longitud(avion->equipajeMano)))
    {
        if (strcmp(primero(avion->equipajeMano).estado, "Descargar") == 0)
        {
            encontrado = 1;
            *e = primero(avion->equipajeMano);
        }
        else
        {
            encolar(&avion->equipajeMano, primero(avion->equipajeMano));
        }
        desencolar(&avion->equipajeMano);
        i++;
    }
    return encontrado;
}

void verAviones(Avion aviones[MAX_AVIONES], int cantidad)
{
    int i;

    for (i = 0; i < cantidad; i++)
    {
        fprintf(avionesLog, "\nAVION NUMERO %i (%s)\n", i, aviones[i].estado);
        fprintf(avionesLog, "CODIGO VUELO: %s\n", aviones[i].codigoVuelo);
        fprintf(avionesLog, "Aerolinea: %s\n", aviones[i].nombre);
        fprintf(avionesLog, "DESTINO: (%s, %s)\n", aviones[i].ciudad, aviones[i].pais);
        fprintf(avionesLog, "Capacidad inicial: %f\n", aviones[i].capacidadMaxima);
        fprintf(avionesLog, "Capacidad Usada: %f\n", aviones[i].capacidadMaxima - aviones[i].capacidad);
        fprintf(avionesLog, "Capacidad restante: %f\n", aviones[i].capacidad);
        fprintf(avionesLog, "Equipaje Especial: %i\n", longitud(aviones[i].equipajeEsp));
        fprintf(avionesLog, "Equipaje Facturado: %i\n", longitud(aviones[i].equipajes));
        fprintf(avionesLog, "Equipaje sobredimensionado: %i\n", longitud(aviones[i].equipajeSD));
        fprintf(avionesLog, "Equipaje de Mano: %i\n", longitud(aviones[i].equipajeMano));
        fprintf(avionesLog, "\nEQUIPAJES\n");
        while (esVacio(aviones[i].equipajeEsp) == 0)
        {
            fprintf(avionesLog, "%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajeEsp).id, primero(aviones[i].equipajeEsp).ciudad, primero(aviones[i].equipajeEsp).pais, primero(aviones[i].equipajeEsp).prioridad, primero(aviones[i].equipajeEsp).peso, primero(aviones[i].equipajeEsp).estado);
            desencolar(&aviones[i].equipajeEsp);
        }
        fprintf(avionesLog, "------------------\n");
        while (esVacio(aviones[i].equipajes) == 0)
        {
            fprintf(avionesLog, "%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajes).id, primero(aviones[i].equipajes).ciudad, primero(aviones[i].equipajes).pais, primero(aviones[i].equipajes).prioridad, primero(aviones[i].equipajes).peso, primero(aviones[i].equipajes).estado);
            desencolar(&aviones[i].equipajes);
        }
        fprintf(avionesLog, "----------------\n");
        while (esVacio(aviones[i].equipajeSD) == 0)
        {
            fprintf(avionesLog, "%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajeSD).id, primero(aviones[i].equipajeSD).ciudad, primero(aviones[i].equipajeSD).pais, primero(aviones[i].equipajeSD).prioridad, primero(aviones[i].equipajeSD).peso, primero(aviones[i].equipajeSD).estado);
            desencolar(&aviones[i].equipajeSD);
        }
        fprintf(avionesLog, "----------------\n");
        while (esVacio(aviones[i].equipajeMano) == 0)
        {
            fprintf(avionesLog, "%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajeMano).id, primero(aviones[i].equipajeMano).ciudad, primero(aviones[i].equipajeMano).pais, primero(aviones[i].equipajeMano).prioridad, primero(aviones[i].equipajeMano).peso, primero(aviones[i].equipajeMano).estado);
            desencolar(&aviones[i].equipajeMano);
        }
    }
}
#endif