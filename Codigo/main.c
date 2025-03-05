#include "semaforo.h"
#include "almacen.h"
#include "interfaz.h"
#include "ColaEntero.h"
#include "avion.h"


#define MAX_MOSTRADORES 5000 // numero de mostradores
#define MAX_CINTAS 500
#define MAX_ALMACEN 250
#define MAX_EQUIPAJES 120736
#define MAX_AVIONES 100

void *mostrador(void *args);
void *cinta(void *args);
void leer_entradas(const char *filename);
int existeVuelo(Equipaje *e);

sem_t semMostrador,mutexMostrador,semCinta,mutexAlmacen,mutexCintaInterfaz, mutexAlmacenes[MAX_ALMACEN], mutexAviones[MAX_AVIONES];
Cola pasajeros,cintas[MAX_CINTAS];
Almacen almacenes[MAX_ALMACEN];
Avion aviones[MAX_AVIONES];
int banderaFinMostrador = 1,nroEquipaje = 0;
int cintaInterfaz[MAX_CINTAS],mostradorInterfaz[MAX_MOSTRADORES],almacenInterfaz[MAX_ALMACEN],requisitoInterfaz = 0;
ColaEntero buscarInterfaz;



int main() {
    int i,t,w,p; //variables para bucles
    int op_menu; //opcion del menu seleccionada
    int cantAviones = 0; //Cantidad de aviones en el aeropuerto
    pthread_t mostradores[MAX_MOSTRADORES];
    pthread_t cintaHilo[MAX_CINTAS];

    fclose(fopen("../pruebas/logAlmacen.txt", "w")); //Vaciar archivo de log (Por si ya existe)
    
    usuario(&requisitoInterfaz,&buscarInterfaz);
    sem_init(&mutexAlmacen,1);
    sem_init(&mutexMostrador,1);
    sem_init(&mutexCintaInterfaz,1);
    sem_init(&semCinta,1);
    sem_init(&semMostrador,1);
    for(int i=0;i<MAX_ALMACEN;i++){
        sem_init(&mutexAlmacenes[i],1);
    }
    for(int i=0;i<MAX_AVIONES;i++){
        sem_init(&mutexAviones[i],1);
    }

    //inicializar listas con 0
    inicializarInt(MAX_CINTAS,cintaInterfaz);
    inicializarInt(MAX_MOSTRADORES,mostradorInterfaz);
    inicializarInt(MAX_ALMACEN,almacenInterfaz);

    //creacion de colas y almacenes
    crear(&pasajeros);
    constructorAlmacen(almacenes);

    //lectura de archivo
    crearAviones(aviones, &cantAviones);
    leer_entradas("../pruebas/text.txt");
    printf("\t===1 Entrada leida===\n");

    //creacion de hilos
    for (i = 0; i < MAX_MOSTRADORES; i++) {
        int *arg = malloc(sizeof(*arg));  
        *arg = i; 
        pthread_create(&mostradores[i], NULL, mostrador, arg);
    }
    for (t = 0; t < MAX_CINTAS; t++){
        int *arg = malloc(sizeof(*arg));  
        *arg = t; 
        pthread_create(&cintaHilo[t],NULL,cinta,arg);
        
    }
    for (p = 0; p < MAX_MOSTRADORES; p++){
        pthread_join(mostradores[p],NULL);
    }
    for (w = 0; w < MAX_CINTAS; w++){
        pthread_join(cintaHilo[w],NULL);
    }

    //destruccion de semaforos
    sem_destroy(&mutexAlmacen);
    sem_destroy(&mutexMostrador);
    sem_destroy(&mutexCintaInterfaz);
    sem_destroy(&semCinta);
    sem_destroy(&semMostrador);
    for(int i=0;i<MAX_ALMACEN;i++){
        sem_destroy(&mutexAlmacenes[i]);
    }

    //verificaciones 
    respuestasFinal(requisitoInterfaz,almacenInterfaz,cintaInterfaz,mostradorInterfaz);
    verColasAlmacenes(almacenes); //Escribir resultados almacen

    return 0;
}


void *mostrador(void *args){
    int id = *((int *)args),indice = 0;
    while(1){ 
        sem_wait(&semMostrador);
        if(banderaFinMostrador){
            Cola equipaje;

            sem_wait(&mutexMostrador);
            nroEquipaje++;
            pasajeros.primero->info.id = nroEquipaje; //asigna numero unico de equipaje
            incrementar(id,mostradorInterfaz);
            mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,1,pasajeros.primero->info);// tinee detalles no funciona al 100% todavia
            indice = id/10; //distribucion de cintas
            equipaje = cintas[indice];
            encolar(&equipaje,primero(pasajeros));
            cintas[indice] = equipaje;
            desencolar(&pasajeros);

            sem_post(&mutexMostrador);
        }
        if (esVacio(pasajeros)){
            banderaFinMostrador = 0;
            sem_post(&semMostrador);
            pthread_exit(NULL);
        }else{
            sem_post(&semMostrador);
        }
    }
}

void *cinta(void *args){
    int id = *((int *)args);
    Cola equipaje,vacia;
    int indice = 0,almacenado = 0,aux = 0;
    crear(&vacia);
    while (1){
        sem_wait(&semCinta);
        sem_wait(&mutexMostrador);

        equipaje = cintas[id];
        cintas[id] = vacia;

        sem_post(&mutexMostrador);

        while (esVacio(equipaje) != 1){

            sem_wait(&mutexCintaInterfaz);
            incrementar(id,cintaInterfaz);
            mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,2,equipaje.primero->info);// tinee detalles no funciona al 100% todavia
            sem_post(&mutexCintaInterfaz);

            while ((indice < MAX_ALMACEN) && (!almacenado)){
                if(compararPais(equipaje.primero->info.pais,&almacenes[indice])){
                    almacenado = 1;
                    sem_wait(&mutexAlmacenes[indice]);

                    equipaje.primero->info.prioridad = traducirPrioridad(equipaje.primero->info.tipo);
                    almacenado = almacenar(&almacenes[indice],primero(equipaje)); //encola con prioridad  
                    incrementar(indice,almacenInterfaz);
                    mostrarEspecificacion(requisitoInterfaz,indice,buscarInterfaz,3,equipaje.primero->info); // tinee detalles no funciona al 100% todavia
                    sem_post(&mutexAlmacenes[indice]);
                }
                indice++;
            }
            indice = 0;
            almacenado = 0;
            desencolar(&equipaje);
        }

        sem_post(&semCinta);
        sem_wait(&mutexMostrador);

        equipaje = cintas[id];
        if((!banderaFinMostrador) && (esVacio(equipaje) == 1)){
            sem_post(&mutexMostrador);
            pthread_exit(NULL);
        }

        sem_post(&mutexMostrador);
    }
}
void leer_entradas(const char *filename) {
    Equipaje equipaje;
    FILE *file = fopen(filename, "r");
    char buffer[256];
    construtorEquipaje(&equipaje);
    if (file == NULL) {
        perror("Error abriendo el archivo");
        exit(EXIT_FAILURE);
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        construtorEquipaje(&equipaje);
        if (sscanf(buffer, "%s %s %s %f ", equipaje.tipo, equipaje.ciudad, equipaje.pais,&equipaje.peso)) {            
            //Solo ir aceptando los de vuelos listos para ser cargados
            if(existeVuelo(&equipaje)){
                encolar(&pasajeros, equipaje);
            }
        }
    }
    printf("%i", longitud(pasajeros));
    fclose(file);
}
int existeVuelo(Equipaje *e){
    int i;
    for(i=0;i<20;i++){
        if((strcmp(e->ciudad,aviones[i].ciudad) == 0) && (strcmp(e->pais,aviones[i].pais) == 0)){
            e->idVuelo = i;
            //printf("El vuelo de (%s, %s) si existe\n", e->ciudad, e->pais);
            return 1;
        }
    }
    return 0;
}
