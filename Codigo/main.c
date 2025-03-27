#include "semaforo.h"
#include "almacen.h"
#include "interfaz.h"
#include "ColaEntero.h"
#include "avion.h"


#define MAX_MOSTRADORES 5000 // numero de mostradores
#define MAX_CINTAS 500
#define MAX_EQUIPAJES 120736
#define ETAPA_MOSTRADOR 1
#define ETAPA_CINTA 2
#define ETAPA_ALMACEN 3
#define ENTRADA 1
#define SALIDA 2


void *mostrador(void *args);
void *cinta(void *args);
void *almacen(void *args);
void *descargadorAvion(void *args);
void leer_entradas(const char *filename);
int existeVuelo(Equipaje *e);

sem_t semMostrador,mutexMostrador,semCinta[MAX_CINTAS],mutexAlmacen,mutexCintaInterfaz, mutexAlmacenes[MAX_ALMACEN], mutexAviones[MAX_AVIONES];
sem_t semTerminal, semPerdidos;
Cola pasajeros,cintas[MAX_CINTAS];
Almacen almacenes[MAX_ALMACEN], objetosPerdidos[MAX_ALMACEN];
Avion aviones[MAX_AVIONES];
int banderaFinMostrador = 1,nroEquipaje = 0;
int cintaInterfaz[MAX_CINTAS],mostradorInterfaz[MAX_MOSTRADORES],almacenInterfaz[MAX_ALMACEN],requisitoInterfaz = 0;
int buscarInterfaz[5];
FILE *fileMostrador,*fileCinta,*fileAlmacen;
int cantAviones = 0; //Cantidad de aviones en el aeropuerto
Cola terminal;


int main() {
    int i,t,w,p; //variables para bucles
    int op_menu; //opcion del menu seleccionada
    pthread_t mostradores[MAX_MOSTRADORES];
    pthread_t cintaHilo[MAX_CINTAS];
    pthread_t hilosAlmacenes[MAX_ALMACEN];
    pthread_t hilosAviones[MAX_AVIONES];

    //creando archivos de escritura
    almacenLog = fopen("../salidas/logAlmacen.txt", "w");
    avionesLog = fopen("../salidas/finalAviones.txt", "w");
    fileMostrador = fopen("../salidas/mostrador.txt", "w");
    fileCinta = fopen("../salidas/cinta.txt", "w");
    fileAlmacen = fopen("../salidas/almacen.txt", "w");
    finalAlmacen = fopen ("../salidas/finalAlmacen.txt", "w");
    if((fileMostrador == NULL) || (fileCinta == NULL) || (fileAlmacen == NULL)) {
        perror("Error Creando los archivos\n");
        exit(EXIT_FAILURE);
    }
    
    usuario(&requisitoInterfaz,buscarInterfaz);
    sem_init(&mutexAlmacen,1);
    sem_init(&mutexMostrador,1);
    sem_init(&mutexCintaInterfaz,1);
    sem_init(&semCinta,1);
    sem_init(&semMostrador,1);
    sem_init(&semTerminal, 1);
    sem_init(&semPerdidos, 1);
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
    crear(&terminal);
    constructorAlmacen(almacenes);
    constructorAlmacen(objetosPerdidos);

    //lectura de archivo
    crearAviones(aviones, &cantAviones);
    leer_entradas("../pruebas/informacion_equipajes.txt");
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
    for (int j = 0; j < MAX_ALMACEN; j++){
        int *arg = malloc(sizeof(*arg));  
        *arg = j; 
        pthread_create(&hilosAlmacenes[j],NULL,almacen,arg); 
    }
    for (int j = 0; j < MAX_AVIONES; j++){
        int *arg = malloc(sizeof(*arg));  
        *arg = j; 
        pthread_create(&hilosAviones[j],NULL,descargadorAvion, arg); 
    }
    for (p = 0; p < MAX_MOSTRADORES; p++){
        pthread_join(mostradores[p],NULL);
    }
    for (w = 0; w < MAX_CINTAS; w++){
        pthread_join(cintaHilo[w],NULL);
    }
    for(int k=0; k< MAX_ALMACEN;k++){
        pthread_join(hilosAlmacenes[k], NULL);
    }
    for(int k=0; k< MAX_AVIONES;k++){
        pthread_join(hilosAviones[k], NULL);
    }

    //destruccion de semaforos
    sem_destroy(&mutexAlmacen);
    sem_destroy(&mutexMostrador);
    sem_destroy(&mutexCintaInterfaz);
    sem_destroy(&semCinta);
    sem_destroy(&semMostrador);
    sem_destroy(&semTerminal);
    for(int i=0;i<MAX_ALMACEN;i++){
        sem_destroy(&mutexAlmacenes[i]);
    }
    for(int i=0;i<MAX_CINTAS;i++){
        sem_destroy(&semCinta[i]);
    }
    for(int i=0;i<MAX_AVIONES;i++){
        sem_destroy(&mutexAviones[i]);
    }    //Escribiendo estado final de aviones y almacenes
    verAviones(aviones, cantAviones);
    verColasAlmacenes(almacenes);
    //VER TERMINAL DE LLEGADA
    /*Equipaje e;
    while(esVacio(terminal) == 0) {
        printf("Longitud %i\n", longitud(terminal));
        e = primero(terminal);
        printf("Equipaje [%s] en la terminal\n", e.estado);
        desencolar(&terminal);
    }*/
    //cerrando archivos
    fclose(fileMostrador);
    fclose(fileCinta);
    fclose(fileAlmacen);
    fclose(almacenLog);
    fclose(avionesLog);
    fclose(finalAlmacen);

    //verificaciones 
    respuestasFinal(requisitoInterfaz,almacenInterfaz,cintaInterfaz,mostradorInterfaz);
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
            mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_MOSTRADOR,ENTRADA,pasajeros.primero->info); //Mostrar Entrada
            incrementar(id,mostradorInterfaz);
            registrar(id,ETAPA_MOSTRADOR,pasajeros.primero->info,fileMostrador);

            indice = (nroEquipaje - 1) % MAX_CINTAS; //distribucion de cintas por modulo para distribuir equitativamente
            equipaje = cintas[indice];
            encolar(&equipaje,primero(pasajeros));
            cintas[indice] = equipaje;

            mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_MOSTRADOR,SALIDA,pasajeros.primero->info); //Mostrar Salida
            mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_CINTA,ENTRADA,pasajeros.primero->info);
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
            mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_CINTA,SALIDA,equipaje.primero->info);
            registrar(id,ETAPA_CINTA,equipaje.primero->info,fileCinta);

            sem_post(&mutexCintaInterfaz);

            while ((indice < MAX_ALMACEN) && (!almacenado)){
                if(compararPais(equipaje.primero->info.pais,&almacenes[indice])){
                    sem_wait(&mutexAlmacenes[indice]);
                    equipaje.primero->info.prioridad = traducirPrioridad(equipaje.primero->info.tipo);
                    almacenado = almacenar(&almacenes[indice],primero(equipaje)); //encola con prioridad  
                    if (almacenado == 1){
                        incrementar(indice,almacenInterfaz);
                        mostrarEspecificacion(requisitoInterfaz,indice,buscarInterfaz,ETAPA_ALMACEN,ENTRADA,equipaje.primero->info); // tinee detalles no funciona al 100% todavia
                        registrar(indice,ETAPA_ALMACEN,equipaje.primero->info,fileAlmacen);
                    }
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

void *almacen(void *args){
    int id = *((int *)args);
    int almacenado = 0,indice = 0;
    while (1){
        sem_wait(&mutexAlmacenes[id]);
        //CONSUMIR
        if(almacenes[id].lleno > 0){
            //printf("DESCARGA DE ALMACEN %i (%i) \n", id, almacenes[id].lleno);
            Equipaje tmpEquipaje;
            tmpEquipaje.id = -1;
            //el 4to parametro de descargarAlmacen retorna el valor de tmpEquipaje, esto por cercanía, ya que usar el return causa overflow
            descargarAlmacen(&almacenes[id], aviones, mutexAviones,&tmpEquipaje);
            if(tmpEquipaje.id != -1){
                int descarga = cargarEquipaje(&aviones[tmpEquipaje.idVuelo], &tmpEquipaje, &mutexAviones[tmpEquipaje.idVuelo]);
                if(descarga == 0){
                    sem_wait(&semPerdidos);
                    while ((indice < MAX_ALMACEN) && (!almacenado)){
                        almacenado = almacenar(&objetosPerdidos[indice],tmpEquipaje);
                        if(almacenado)
                            fprintf(almacenLog,"NO CABE EN EL VUELO el equipaje %i se envio al almacen de perdidos %d \n", tmpEquipaje.id,indice);
                        indice++;
                    }
                    sem_post(&semPerdidos);
                }
            }
        }
        if(almacenes[id].lleno <= 0){
            sem_post(&mutexAlmacenes[id]);
            pthread_exit(NULL);
        }
        sem_post(&mutexAlmacenes[id]);
    }
}

void *descargadorAvion(void *args){
    int id = *((int *)args);
    //printf("Descargador %i\n", id);
    //return;
    int cargado;
    while(1){
        sem_wait(&mutexAviones[id]);
        Equipaje e;
        cargado = descargarEquipaje(&aviones[id], &e);
        if(cargado){
            sem_wait(&semTerminal);
            //encolar en terminal
            encolar(&terminal, e);
            sem_post(&semTerminal);
        }else{
            //printf("Ya se descargo el avión %i (%s)\n", id, aviones[id].estado);
            sem_post(&mutexAviones[id]);
            pthread_exit(NULL);
        }
        sem_post(&mutexAviones[id]);
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
        if (sscanf(buffer, "%s %s %s %s %f %s", equipaje.codVuelo, equipaje.tipo, equipaje.ciudad, equipaje.pais,&equipaje.peso, equipaje.estado)) {            
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
    for(i=0;i<cantAviones;i++){
        if((strcmp(e->codVuelo,aviones[i].codigoVuelo) == 0)){
            e->idVuelo = i;
            //Guardar el equipaje que ya se encuentra en el avión de conexión
            if((strcmp(e->estado, "Descargar") == 0) || (strcmp(e->estado, "Mantener") == 0)){
                verificarEquipaje(&aviones[i], e, &mutexAviones[i]);
                return 0;
            }
            //printf("El vuelo de (%s, %s) si existe\n", e->ciudad, e->pais);
            return 1;
        }
    }
    return 0;
}
