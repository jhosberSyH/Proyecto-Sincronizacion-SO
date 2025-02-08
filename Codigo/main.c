#include <pthread.h>
#include <unistd.h>
#include "Cola.h"


#define MAX_MOSTRADORES 5000
#define MAX_CINTAS 500
#define MAX_EQUIPAJES 120736

void *mostrador(void *args);
void leer_entradas(const char *filename);

sem_t semMostrador;
Cola pasajeros,cinta[MAX_CINTAS];
int band = 1,nroEquipaje = 0,cantidad = 0;


int main() {
    int i,j = 0,k = 0,contador = 0;
    Cola res;
    pthread_t mostradores[MAX_MOSTRADORES];
    
    printf("\n\t\tBienvenido\n");
    sem_init(&semMostrador,1);

    crear(&pasajeros);
    leer_entradas("../Pruebas/text.txt");
    printf("\t===1 leido===\n");

    for (i = 0; i < MAX_MOSTRADORES; i++){
        pthread_create(&mostradores[i],NULL,mostrador,&i);
    }

    for (i = 0; i < MAX_MOSTRADORES; i++){
        pthread_join(mostradores[i],NULL);
    }

    sem_destroy(&semMostrador);
    printf("\t===2 numeros de equipajes y distribucion en cintas===\n");

    //verificaciones 
    for ( i = 0; i < MAX_CINTAS; i++){
        res =  cinta[i];
        j += 10;
        printf("mostrador %d-%d pasaron : %d equipajes\n",k+1,j,res.log);
        k += 10;
        if(res.log == 0){
            contador+=+1;
        }
    }

    printf("===hay esta cantidad de cintas sin utilizarse===%d\n",contador);
    printf("===cantidad===%d\n",cantidad);

    if(cantidad > MAX_EQUIPAJES){ 
            printf("===se esta leyendo mas elementos de los deseados en la zona critica===\n");
    }

    return 0;
}


void *mostrador(void *args){
    int id = *((int *)args),i,indice = 0;
    while(1){ 
        sem_wait(&semMostrador);
        if(band){
            Cola equipaje;
            nroEquipaje++;
            pasajeros.primero->info.id = nroEquipaje;

            indice = id/10; //distribucion de cintas
            equipaje = cinta[indice];

            encolar(&equipaje,primero(pasajeros));
            cinta[indice] = equipaje;
            desencolar(&pasajeros);

            cantidad++;
        }
        if (esVacio(pasajeros)){
            band = 0;
            sem_post(&semMostrador);
            pthread_exit(NULL);
        }else{
            sem_post(&semMostrador);
        }
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
            encolar(&pasajeros, equipaje);
        }
    }
    fclose(file);
}