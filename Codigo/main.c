#include "semaforo.h"
#include "almacen.h"
#include "interfaz.h"
#include "ColaEntero.h"
#include "avion.h"
#include <sys/sysinfo.h> //Obtiene informacion del sistema
#include <sys/resource.h> //Obtiene informacio del proceso
#include <time.h>

// Cantidades de hilos
#define MAX_MOSTRADORES 5000 
#define MAX_CINTAS 500
#define MAX_ALMACEN 250
#define MAX_EQUIPAJES 120736

// Enumerando Etapas Del Proyecto
#define MAX_ETAPA 3
#define ETAPA_MOSTRADOR 1
#define ETAPA_CINTA 2
#define ETAPA_ALMACEN 3

//Enumenrando Otros
#define ENTRADA 1
#define SALIDA 2
#define TIEMPO 2

void *mostrador(void *args);
void *cinta(void *args);
void *almacen(void *args);
void *descargadorAvion(void *args);
void *supervisor();
void leer_entradas(const char *filename);
int existeVuelo(Equipaje *e);

//Semaforos y Mutex
sem_t semMostrador,mutexMostrador,semCinta[MAX_CINTAS],semTerminal, semPerdidos;
sem_t mutexSupervisor[5],mutexAlmacen,mutexCintaInterfaz, mutexAlmacenes[MAX_ALMACEN], mutexAviones[MAX_AVIONES];

//Contadores para saber cuantos hilos hay activos
int contadorHiloMostrador = 0,contadorHiloCinta = 0,contadorHiloAlmacen = 0;

Cola pasajeros,cintas[MAX_CINTAS];
Almacen almacenes[MAX_ALMACEN], objetosPerdidos[MAX_ALMACEN];
Avion aviones[MAX_AVIONES];
int banderaFinMostrador = 1,nroEquipaje = 0;
FILE *fileMostrador,*fileCinta,*fileAlmacen;
int cantAviones = 0; //Cantidad de aviones en el aeropuerto
Cola terminal;

//variables para Interfaz
int cintaInterfaz[MAX_CINTAS],mostradorInterfaz[MAX_MOSTRADORES],almacenInterfaz[MAX_ALMACEN],perdidosInterfaz[MAX_ALMACEN],requisitoInterfaz = 0;
int buscarInterfaz[5];



int main() {
    int i,t,w,p; //variables para bucles

    pthread_t mostradores[MAX_MOSTRADORES];
    pthread_t cintaHilo[MAX_CINTAS];
    pthread_t hilosAlmacenes[MAX_ALMACEN];
    pthread_t hilosAviones[MAX_AVIONES];
    pthread_t supervisorHilo;

    //creando archivos de escritura
    almacenLog = fopen("../salidas/logAlmacen.txt", "w");
    avionesLog = fopen("../salidas/finalAviones.txt", "w");
    fileMostrador = fopen("../salidas/mostrador.txt", "w");
    fileCinta = fopen("../salidas/cinta.txt", "w");
    fileAlmacen = fopen("../salidas/almacen.txt", "w");
    finalAlmacen = fopen ("../salidas/finalAlmacen.txt", "w");

    if((fileMostrador == NULL) || (fileCinta == NULL) || (fileAlmacen == NULL) || (finalAlmacen == NULL) || (avionesLog == NULL) || (almacenLog == NULL)){
        perror("Error Creando los archivos\n");
        exit(EXIT_FAILURE);
    }
    
    usuario(&requisitoInterfaz,buscarInterfaz);

    //inicializar semaforos
    sem_init(&mutexAlmacen,1);
    sem_init(&mutexMostrador,1);
    sem_init(&mutexCintaInterfaz,1);
    sem_init(&semMostrador,1);
    sem_init(&semTerminal, 1);
    sem_init(&semPerdidos, 1);
    for(int i=0;i<MAX_ALMACEN;i++){
        sem_init(&mutexAlmacenes[i],1);
    }
    for(int i=0;i<MAX_AVIONES;i++){
        sem_init(&mutexAviones[i],1);
    }
    for(int i=0;i<MAX_CINTAS;i++){
        sem_init(&semCinta[i],1);
    }

    //inicializar listas con 0
    inicializarInt(MAX_CINTAS,cintaInterfaz);
    inicializarInt(MAX_MOSTRADORES,mostradorInterfaz);
    inicializarInt(MAX_ALMACEN,almacenInterfaz);
    inicializarInt(MAX_ALMACEN,perdidosInterfaz);

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
    if (requisitoInterfaz == 0){
        pthread_t supervisorHilo;
        for(int i=0;i < MAX_ETAPA;i++){
            sem_init(&mutexSupervisor[i],1);
        }
        pthread_create(&supervisorHilo,NULL,supervisor,NULL);
    }
    
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

    //Espera de hilos
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
    if (requisitoInterfaz == 0){
        for(int i=0; i < MAX_ETAPA;i++){
            sem_destroy(&mutexSupervisor[i]);
        }
    }
    sem_destroy(&mutexAlmacen);
    sem_destroy(&mutexMostrador);
    sem_destroy(&mutexCintaInterfaz);
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
    respuestasFinal(requisitoInterfaz,almacenInterfaz,cintaInterfaz,mostradorInterfaz,perdidosInterfaz);
    return 0;
}


void *mostrador(void *args){
    int id = *((int *)args),indice = 0;
    Equipaje aux;
    if (requisitoInterfaz == 0){ //activa el Modo Supervisor
        sem_wait(&mutexSupervisor[ETAPA_MOSTRADOR]);
        contadorHiloMostrador++;
        sem_post(&mutexSupervisor[ETAPA_MOSTRADOR]);
    }
    while(1){
        //INICIO SECCION CRITICA
        //Bloquear acceso a bandera de finalización de los equipajes
        if (requisitoInterfaz == 0){ //activar tiempo de espera para el modo supervisor
            sleep(TIEMPO);
        }
        sem_wait(&semMostrador);
        if(banderaFinMostrador){
            //Bloquear acceso a la cola de equipajes
            sem_wait(&mutexMostrador);

            nroEquipaje++;
            pasajeros.primero->info.id = nroEquipaje; //asigna numero unico de equipaje
            
            //Registrar en la interfaz la entrada al mostrador
            mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_MOSTRADOR,ENTRADA,pasajeros.primero->info); //Mostrar Entrada
            incrementar(id,mostradorInterfaz);
            registrar(id,ETAPA_MOSTRADOR,pasajeros.primero->info,fileMostrador);
            
            //Guardar equipaje en aux y quitarlo de la cola
            aux = primero(pasajeros);
            desencolar(&pasajeros);

            //Colocar bandera de finalizacion de equipajes si está vacía
            if(esVacio(pasajeros)){
                banderaFinMostrador = 0;
            }
            
            //Desbloquear accesos a equipajes y a la bandera
            sem_post(&semMostrador);
            sem_post(&mutexMostrador);
            //FINAL SECCION CRITICA

            //PROCESAMIENTO EN EL MOSTRADOR()

            if(strcmp(aux.tipo,"Mano") == 0){
                //Salida del mostrador
                aux.prioridad = traducirPrioridad(aux.tipo);
                cargarEquipaje(&aviones[aux.idVuelo],&aux,&mutexAviones[aux.idVuelo]);
                mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_MOSTRADOR,SALIDA,aux); //Mostrar Salida

            }else{
                indice = (nroEquipaje - 1) % MAX_CINTAS; //distribucion de cintas por modulo para distribuir equitativamente
                //Salida del mostrador
                mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_MOSTRADOR,SALIDA,aux); //Mostrar Salida
                //Bloquear cinta para poder encolar el equipaje
                sem_wait(&semCinta[indice]); 
                encolar(&cintas[indice],aux);
                sem_post(&semCinta[indice]);
                //Registrar entrada a la cinta
                mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_CINTA,ENTRADA,aux);
            }

        
        }else{
            sem_post(&semMostrador);
            if (requisitoInterfaz == 0){ //desactiva el Modo Supervisor
                sem_wait(&mutexSupervisor[ETAPA_MOSTRADOR]);
                contadorHiloMostrador--;
                sem_post(&mutexSupervisor[ETAPA_MOSTRADOR]);
            }
            pthread_exit(NULL);
        }
    }
}

void *cinta(void *args){
    int id = *((int *)args);
    Cola equipaje; //equipajes en la cinta
    int indice = 0;
    int almacenado = 0; //bandera para saber si se pudo almacenar el equipaje
    if (requisitoInterfaz == 0){ //activa el Modo Supervisor
        sem_wait(&mutexSupervisor[ETAPA_CINTA]);
        contadorHiloCinta++;
        sem_post(&mutexSupervisor[ETAPA_CINTA]);
    }
    while (1){
        while (esVacio(cintas[id]) != 1){
            //Escribir en la interfaz
            sem_wait(&mutexCintaInterfaz);

            incrementar(id,cintaInterfaz);
            mostrarEspecificacion(requisitoInterfaz,id,buscarInterfaz,ETAPA_CINTA,SALIDA,primero(cintas[id]));
            registrar(id,ETAPA_CINTA,primero(cintas[id]),fileCinta);

            sem_post(&mutexCintaInterfaz);

            //Bloquear cinta
            sem_wait(&semCinta[id]);
            if (requisitoInterfaz == 0){ //activar tiempo de espera para el modo supervisor
                sleep(TIEMPO);
            }
            //Descargar cinta y mover a un almacen
            while ((indice < MAX_ALMACEN) && (!almacenado)){
                //Buscar almacen con el país destino del equipaje o uno vacío
                if(compararPais(primero(cintas[id]).pais,&almacenes[indice])){
                    //Bloquear almacen para poder mover el equipaje a este
                    sem_wait(&mutexAlmacenes[indice]);
                    //Clasificar por prioridad y moverlo al almacen
                    cintas[id].primero->info.prioridad = traducirPrioridad(primero(cintas[id]).tipo);
                    almacenado = almacenar(&almacenes[indice],primero(cintas[id]));
                    //Registrar almacenaje si se pudo realizar la operacion
                    if (almacenado == 1){
                        incrementar(indice,almacenInterfaz);
                        mostrarEspecificacion(requisitoInterfaz,indice,buscarInterfaz,ETAPA_ALMACEN,ENTRADA,primero(cintas[id])); // tinee detalles no funciona al 100% todavia
                        registrar(indice,ETAPA_ALMACEN,primero(cintas[id]),fileAlmacen);
                    }
                    //Desbloquear almacen
                    sem_post(&mutexAlmacenes[indice]);
                }
                indice++;
            }
            indice = 0;
            almacenado = 0;
            desencolar(&cintas[id]);
            //Liberar cinta
            sem_post(&semCinta[id]);
        }
        sem_wait(&semMostrador);

        if((!banderaFinMostrador) && (esVacio(cintas[id]) == 1)){
            sem_post(&semMostrador);
            if (requisitoInterfaz == 0){ //desactiva el Modo Supervisor
                sem_wait(&mutexSupervisor[ETAPA_CINTA]);
                contadorHiloCinta--;
                sem_post(&mutexSupervisor[ETAPA_CINTA]);
            }
            pthread_exit(NULL);
        }

        sem_post(&semMostrador);
    }
}

void *almacen(void *args){
    int id = *((int *)args);
    int almacenado = 0,indice = 0;
    if (requisitoInterfaz == 0){ //activa el Modo Supervisor
        sem_wait(&mutexSupervisor[ETAPA_ALMACEN]);
        contadorHiloAlmacen++;
        sem_post(&mutexSupervisor[ETAPA_ALMACEN]);
    }
    while (1){
        sem_wait(&mutexAlmacenes[id]);
        if (requisitoInterfaz == 0){ //activar tiempo de espera para el modo supervisor
            sleep(TIEMPO);
        }
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
                        if(almacenado){
                            incrementar(indice,perdidosInterfaz);
                            fprintf(almacenLog,"NO CABE EN EL VUELO el equipaje %i se envio al almacen de perdidos %d \n", tmpEquipaje.id,indice);
                        }
                        indice++;
                    }
                    sem_post(&semPerdidos);
                }
            }
        }
        if(almacenes[id].lleno <= 0){
            sem_post(&mutexAlmacenes[id]);
            if (requisitoInterfaz == 0){ //desactiva el Modo Supervisor
                printf("mucha esencia ahhhhhhhhh\n");
                sem_wait(&mutexSupervisor[ETAPA_ALMACEN]);
                contadorHiloAlmacen--;
                sem_post(&mutexSupervisor[ETAPA_ALMACEN]);
            }
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

void *supervisor(){
    int i = 0;
    FILE *fileSupervisor = fopen("../salidas/supervisor.txt", "w");
    while (1){
        struct sysinfo info;
        struct rusage usado;

        // Obtener información del sistema y del programa
        if ((sysinfo(&info) != 0) || (getrusage(RUSAGE_SELF,&usado) != 0)) {
            perror("Error al obtener la información del sistema");
        }

        // Obtener la fecha y hora actual
        time_t now = time(NULL);
        char date[26];
        ctime_r(&now, date);

        // Calculos de tiempo
        double tiempoUsoCpuUsuario = usado.ru_utime.tv_sec + (usado.ru_utime.tv_usec / 1e6) ;
        double tiempoUsoCpuSistema = usado.ru_stime.tv_sec + (usado.ru_stime.tv_usec / 1e6);

        //Calculos de memoria
        unsigned long total = info.totalram * info.mem_unit;
        unsigned long libre = info.freeram * info.mem_unit;
        unsigned long uso = total - libre;

        system("clear");

        for (i = 0; i < MAX_ETAPA; i++){
            sem_wait(&mutexSupervisor[i]);
        }

        //Calculos de Procesos e Hilos
        int totalProcesos = info.procs;
        int totalHilos = (contadorHiloAlmacen + contadorHiloCinta + contadorHiloMostrador) + 2; // 2 = hilo principal y hilo supervisor

        printf("\t+------------------------------------------------------+\n");
        printf("\t|Fecha: %s", date);
        printf("\t|\tConsumo del CPU\n");
        printf("\t|Tiempo Usuario: %.6f segundos\n", tiempoUsoCpuUsuario);
        printf("\t|Tiempo Sistema: %.6f segundos\n", tiempoUsoCpuSistema);
        printf("\t|Tiempo Total: %.6f segundos\n", tiempoUsoCpuSistema + tiempoUsoCpuUsuario);
        printf("\t|\tConsumo del Memoria\n");
        printf("\t|Memoria Total: %ld Megabytes\n", total/(1024*1024));
        printf("\t|Memoria Libre: %ld Megabytes\n", libre/(1024*1024));
        printf("\t|Memoria Usada en General: %ld Megabytes\n", uso/(1024*1024));
        printf("\t|Memoria Usada por el programa: %ld Megabytes\n", usado.ru_maxrss/1024);
        printf("\t|\tCantidades de Procesos\n");
        printf("\t|Hay %d Procesos de los cuales %d son del Proyecto\n",totalProcesos,totalHilos);
        printf("\t|Cantidad de Hilos trabajando en Mostrador: %d\n", contadorHiloMostrador);
        printf("\t|Cantidad de Hilos trabajando en Cinta: %d\n", contadorHiloCinta);
        printf("\t|Cantidad de Hilos trabajando en Almacen: %d\n", contadorHiloAlmacen);
        printf("\t+------------------------------------------------------+\n");
        
        //lleva el registro
        fprintf(fileSupervisor,"\t+------------------------------------------------------+\n");
        fprintf(fileSupervisor,"\t|Fecha: %s", date);
        fprintf(fileSupervisor,"\t|\tConsumo del CPU\n");
        fprintf(fileSupervisor,"\t|Tiempo Usuario: %.6f segundos\n", tiempoUsoCpuUsuario);
        fprintf(fileSupervisor,"\t|Tiempo Sistema: %.6f segundos\n", tiempoUsoCpuSistema);
        fprintf(fileSupervisor,"\t|Tiempo Total: %.6f segundos\n", tiempoUsoCpuSistema + tiempoUsoCpuUsuario);
        fprintf(fileSupervisor,"\t|\tConsumo del Memoria\n");
        fprintf(fileSupervisor,"\t|Memoria Total: %ld Megabytes\n", total/(1024*1024));
        fprintf(fileSupervisor,"\t|Memoria Libre: %ld Megabytes\n", libre/(1024*1024));
        fprintf(fileSupervisor,"\t|Memoria Usada en General: %ld Megabytes\n", uso/(1024*1024));
        fprintf(fileSupervisor,"\t|Memoria Usada por el programa: %ld Megabytes\n", usado.ru_maxrss/1024);
        fprintf(fileSupervisor,"\t|\tCantidades de Procesos\n");
        fprintf(fileSupervisor,"\t|Hay %d Procesos de los cuales %d son del Proyecto\n",totalProcesos,totalHilos);
        fprintf(fileSupervisor,"\t|Cantidad de Hilos trabajando en Mostrador: %d\n", contadorHiloMostrador);
        fprintf(fileSupervisor,"\t|Cantidad de Hilos trabajando en Cinta: %d\n", contadorHiloCinta);
        fprintf(fileSupervisor,"\t|Cantidad de Hilos trabajando en Almacen: %d\n", contadorHiloAlmacen);
        fprintf(fileSupervisor,"\t+------------------------------------------------------+\n");
        
        for (i = 0; i < MAX_ETAPA; i++){
            sem_post(&mutexSupervisor[i]);
        }

        fclose(fileSupervisor);
        sleep(30); //escribe cada 30 segundos
        FILE *fileSupervisor = fopen("../salidas/supervisor.txt", "a");
    }
}
