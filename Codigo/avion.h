#ifndef Avion_h
#define Avion_h
#include "Cola.h"
#include "Equipaje.h"
#define MAX_AVIONES 200

typedef struct
{
    int id; //Identificar único del avión
    float capacidadMaxima; // Capacidad maxima del avión
    float capacidad;       // Capacidad actual del avión

    // Estado del vuelo (Despegue, Conexion, Aterrizaje)
    char estado[30];
    /*
    "Despegue": Se llena de 0
    "Conexion": Ya lleva equipajes y solo se descargan unos pocos equipajes que se indiquen
    */

    char nombre[50];    //Nombre de la compañia
    char codigoVuelo[10]; //Codigo del vuelo

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
    Cola equipajeMano; // Equipaje de mano (VAN EN LA CABINA DEL AVION -> NO INFLUYEN EN SU CAPACIDAD)
} Avion;

void crearAviones(Avion aviones[MAX_AVIONES], int *cantidadAviones);
int cargarEquipaje(Avion *avion, Equipaje *e);
int verificarEquipaje(Avion *avion, Equipaje *e, sem_t *semAvion);
int descargarEquipaje(Avion *avion, Equipaje *e);
int buscarEquipajeFacturado(Avion *avion, Equipaje *e);
int buscarEquipajeEspecial(Avion *avion, Equipaje *e);
int buscarEquipajeSobredimensionado(Avion *avion, Equipaje *e);


void verAviones(Avion aviones[MAX_AVIONES], int cantidad);

FILE *avionesLog;

/*
    Lee el archivo vuelos.txt y crear todos los vuelos indicados
    -Cada vuelo se almacena en el arreglo aviones[]
    -Se guarda la cantidad de aviones creados en cantidadAviones
    -Asigna un identificador único al avion y guarda sus características
    -Inicializa la capacidad y las banderas de estados
    -Inicializa las colas de equipajes
*/
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
            //Inicializar capacidad
            a.capacidad = a.capacidadMaxima; 

            //Inicializar banderas de estados
            a.estaLleno = 0;
            a.faltaDescargar = 0;
            //Si el avion es de conexion indicar que faltan descargar equipajes
            if(strcmp(a.estado, "Conexion") == 0){
                a.faltaDescargar = 1;
            }

            // Inicializar colas de equipajes
            crear(&a.enEspera);
            crear(&a.equipajeEsp);
            crear(&a.equipajeSD);
            crear(&a.equipajes);
            crear(&a.equipajeMano);

            //Guardar en arreglo
            aviones[i] = a;
            i++;
            *cantidadAviones = *cantidadAviones + 1;
        }
    }
    fclose(file);
}
/*
    Carga un equipaje a un avión designado
    -Si logra cargar el equipaje devuelve 1
    -Si no pudo cargar pero faltan por descargar equipajes devuelve 2
    -Si no pudo cargar y no falta nada por descargar devuelve 0
    -Ajusta la capacidad del avión automáticamente
    -Marca el avión como lleno si no hace falta descargar nada y su capacidad es menor a 1 (Peso mínimo de los equipajes)
*/
int cargarEquipaje(Avion *avion, Equipaje *e){
    int estado = 0;
    //Verificar que no se sobrepase la capacidad del avión
    if (e->peso <= avion->capacidad){
        //Cargar en la cola correspondiente a la prioridad del equipaje
        switch (e->prioridad){
            case 1:
                encolar(&avion->equipajeEsp, *e); //Especiales
                break;
            case 2:
                encolar(&avion->equipajes, *e); //Facturados
                break;
            case 3:
                encolar(&avion->equipajeSD, *e); //Sobredimensionados
                break;
            case 4:
                encolar(&avion->equipajeMano, *e); //De mano (Los llevan los pasajeros)
                break;
            default:
                break;
        }
        //Se disminuye la capacidad si el equipaje no es de mano
        if (e->prioridad != 4){
            avion->capacidad -= e->peso;
        }
        estado = 1;
    }else{
        //Si no se pudo cargar se verifica si faltan aún equipajes por descargarse
        //Si no falta descargar nada, se marca el avión como lleno
        if((avion->faltaDescargar == 0) && (avion->capacidad < 1.0f)){
            avion->estaLleno = 1;
        }else{
            //Si aún falta por descargar equipajes se indica el estado 2
            if(avion->faltaDescargar == 1){
                estado = 2;
            }
        }
    }

    return estado;
}

/*
    Guarda los equipajes que ya están cargados en los aviones de conexión
    -Cada uno en la cola correspondiente a su prioridad
    -Ajusta la capacidad inicial del avión en el proceso
    -Si se pudo guardar en el avión devuelve 1, sino devuelve 0
*/
int verificarEquipaje(Avion *avion, Equipaje *e, sem_t *semAvion){
    //Bloquea el avión para que no se efectuén operaciones en este mientras se guardan equipajes
    sem_wait(semAvion);
    int estado = 0;
    //Verificar que no se sobrepase la capacidad del avión
    if (e->peso <= avion->capacidad){
        if (strcmp(e->tipo, "Facturado") == 0){
            encolar(&avion->equipajes, *e);
        }
        else{
            if (strcmp(e->tipo, "Especial") == 0){
                encolar(&avion->equipajeEsp, *e);
            }
            else{
                if (strcmp(e->tipo, "Sobredimensionado") == 0){
                    encolar(&avion->equipajeSD, *e);
                }
            }
        }
        //Disminuir capacidad del avión
        avion->capacidad -= e->peso;
        estado = 1;
    }
    //Desbloquea el avión
    sem_post(semAvion);
    return estado;
}

//Función para descargar equipajes del avión
/*
    -Descarga en base a las prioridades de equipajes
    -Si encuentra equipajes devuelve 1, sino devuelve 0
    -Se ajusta la capacidad del avión al descargar
    -Si no encuentra equipajes marca el avión para indicar que ya no le hace falta descargar más
*/
int descargarEquipaje(Avion *avion, Equipaje *e){
    int encontrado = 0;
    int i = 0;
    if (buscarEquipajeEspecial(avion, e)){
        encontrado = 1;
    }
    else{
        if (buscarEquipajeFacturado(avion, e)){
            encontrado = 1;
        }
        else{
            encontrado = buscarEquipajeSobredimensionado(avion, e);
        }
    }
    //Aumentar capacidad al avión tras quitar un equipaje
    if (encontrado){
        avion->capacidad += e->peso;
    }else{
        //Indicar que ya no hace falta descargar más equipajes
        avion->faltaDescargar = 0;
    }
    return encontrado;
}

/*
    Busca equipajes especiales en el avión
    - Si encuentra devuelve 1, sino devuelve 0
    - Guarda en e el equipaje que se encuentre
*/
int buscarEquipajeEspecial(Avion *avion, Equipaje *e){
    int encontrado = 0;
    int i = 0;
    //Recorrer la cola mientras no encuentre un equipaje y no se hayan recorrido todos
    while (((encontrado == 0) && (longitud(avion->equipajeEsp) > 0)) && (i < longitud(avion->equipajeEsp))){
        //Si el estado del equipaje es Descargar, se guarda su valor
        if (strcmp(primero(avion->equipajeEsp).estado, "Descargar") == 0){
            encontrado = 1;
            *e = primero(avion->equipajeEsp);
        }
        else{
            //Devolver a la cola del avión si no es un equipaje a descargar
            encolar(&avion->equipajeEsp, primero(avion->equipajeEsp));
        }
        desencolar(&avion->equipajeEsp);
        i++;
    }
    return encontrado;
}

/*
    Busca equipajes facturados en el avión
    - Si encuentra devuelve 1, sino devuelve 0
    - Guarda en e el equipaje que se encuentre
*/
int buscarEquipajeFacturado(Avion *avion, Equipaje *e){
    int encontrado = 0;
    int i = 0;
    //Recorrer la cola mientras no encuentre un equipaje y no se hayan recorrido todos
    while (((encontrado == 0) && (longitud(avion->equipajes) > 0)) && (i < longitud(avion->equipajes))){
        //Si el estado del equipaje es Descargar, se guarda su valor
        if (strcmp(primero(avion->equipajes).estado, "Descargar") == 0){
            encontrado = 1;
            *e = primero(avion->equipajes);
        }
        else{
            //Devolver a la cola del avión si no es un equipaje a descargar
            encolar(&avion->equipajes, primero(avion->equipajes));
        }
        desencolar(&avion->equipajes);
        i++;
    }
    return encontrado;
}

/*
    Busca equipajes sobredimensionados en el avión
    - Si encuentra devuelve 1, sino devuelve 0
    - Guarda en e el equipaje que se encuentre
*/
int buscarEquipajeSobredimensionado(Avion *avion, Equipaje *e){
    int encontrado = 0;
    int i = 0;
    //Recorrer la cola mientras no encuentre un equipaje y no se hayan recorrido todos
    while (((encontrado == 0) && (longitud(avion->equipajeSD) > 0)) && (i < longitud(avion->equipajeSD))){
        //Si el estado del equipaje es Descargar, se guarda su valor
        if (strcmp(primero(avion->equipajeSD).estado, "Descargar") == 0){
            encontrado = 1;
            *e = primero(avion->equipajeSD);
        }
        else{
            //Devolver a la cola del avión si no es un equipaje a descargar
            encolar(&avion->equipajeSD, primero(avion->equipajeSD));
        }
        desencolar(&avion->equipajeSD);
        i++;
    }
    return encontrado;
}

/*
    Escribe en un archivo el estado final de los Aviones:
    -Identificador y Compañía
    -Estado (Despegue o Conexion)
    -Destino: (ciudad, país)
    -Capacidad inicial
    -Capacidad usada
    -Capacidad restante
    -Equipajes cargados en el avión
*/
void verAviones(Avion aviones[MAX_AVIONES], int cantidad){
    int i;

    for (i = 0; i < cantidad; i++){
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
        //Recorrer cada una de las colas de equipajes para registrar qué equipajes tiene el avión

        while (esVacio(aviones[i].equipajeEsp) == 0){
            fprintf(avionesLog, "%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajeEsp).id, primero(aviones[i].equipajeEsp).ciudad, primero(aviones[i].equipajeEsp).pais, primero(aviones[i].equipajeEsp).prioridad, primero(aviones[i].equipajeEsp).peso, primero(aviones[i].equipajeEsp).estado);
            desencolar(&aviones[i].equipajeEsp);
        }
        fprintf(avionesLog, "------------------\n"); //Separador

        while (esVacio(aviones[i].equipajes) == 0){
            fprintf(avionesLog, "%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajes).id, primero(aviones[i].equipajes).ciudad, primero(aviones[i].equipajes).pais, primero(aviones[i].equipajes).prioridad, primero(aviones[i].equipajes).peso, primero(aviones[i].equipajes).estado);
            desencolar(&aviones[i].equipajes);
        }
        
        fprintf(avionesLog, "----------------\n"); //Separador

        while (esVacio(aviones[i].equipajeSD) == 0){
            fprintf(avionesLog, "%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajeSD).id, primero(aviones[i].equipajeSD).ciudad, primero(aviones[i].equipajeSD).pais, primero(aviones[i].equipajeSD).prioridad, primero(aviones[i].equipajeSD).peso, primero(aviones[i].equipajeSD).estado);
            desencolar(&aviones[i].equipajeSD);
        }

        fprintf(avionesLog, "----------------\n"); //Separador

        while (esVacio(aviones[i].equipajeMano) == 0){
            fprintf(avionesLog, "%i (%s,%s) [prioridad: %i] peso: %f (%s)\n", primero(aviones[i].equipajeMano).id, primero(aviones[i].equipajeMano).ciudad, primero(aviones[i].equipajeMano).pais, primero(aviones[i].equipajeMano).prioridad, primero(aviones[i].equipajeMano).peso, primero(aviones[i].equipajeMano).estado);
            desencolar(&aviones[i].equipajeMano);
        }
    }
}
#endif