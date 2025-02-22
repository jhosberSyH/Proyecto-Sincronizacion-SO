#ifndef Interfaz_h
#define Interfaz_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Cola.h"
#include "Almacen.h"

#define MAX_MOSTRADORES 5000
#define MAX_CINTAS 500
#define MAX_ALMACEN 250
#define MAX_EQUIPAJES 120736

int usuario();
int menu();
int validarNumero(char num[]);
void respuestasFinal(int requisito,Almacen almacenes[],Cola cintas[]);

int usuario(){
    int requisitos = 0,indice;
    requisitos = menu();
    return (requisitos);
}

int menu(){
    int numero = -1;
    char val[20];
    printf("\t+------------------ MENU ------------------+\n");
    printf("\t|                                          |\n");
    printf("\t| [1] Informacion Almacenes                |\n");
    printf("\t| [2] Informacion Cintas                   |\n");
    printf("\t| [3]                                      |\n");
    printf("\t| [4]                                      |\n");
    printf("\t| [5] Mostrar Todo                         |\n");
    printf("\t|                                          |\n");
    printf("\t+------------------------------------------+\n");
    printf("Opcion: ");
    do{
        scanf("%s",val);
        numero = validarNumero(val);
    }while(numero == 0);
    
    numero = atoi(val);
    system("clear");
    
    return (numero);
}

int validarNumero(char num[]){
    int i, len; 
    len = strlen(num); 
    for (i = 0; i < len ; i++){ 
        if (!(isdigit(num[i]))){ 
            printf("!Solo se permiten numeros!\nIntente de nuevo: "); 
            return (0);
        }
    }
	return (1); 
}


void respuestasFinal(int requisito,Almacen almacenes[],Cola cintas[]){
    int cantidad = 0,i,k = 0,j = 0,equipajeAtorado = 0,cintasUso = 0;
    Cola res;
    switch (requisito){
    case 1:
        for ( i = 0; i < MAX_ALMACEN; i++){
            Cola res =  almacenes[i].equipajes;
            printf("El almacen %d tiene %d Equipajes del pais %s\n",almacenes[i].id + 1,res.log,almacenes[i].pais);
            cantidad += res.log;
        }
        printf("===Almacenados===%d\n",cantidad);
        printf("===Equipajes Perdidos===%d\n",MAX_EQUIPAJES-cantidad);
        break;

    case 2:
        for ( i = 0; i < MAX_CINTAS; i++){
            res =  cintas[i];
            j += 10;
            printf("mostrador %d-%d pasaron : %d equipajes\n",k+1,j,res.log);
            k += 10;
            equipajeAtorado += res.log;
            if(res.log == 0){
                cintasUso+=1;
            }
        }
        printf("===Se usaron esta cantidad de cintas===%d\n",cintasUso);
        printf("===Se atoraron en la cinta esta cantidad de Equipajes ===%d\n",equipajeAtorado);
        break;

    case 5:
        printf("\t\tAlmacenes:\n\n");
        for ( i = 0; i < MAX_ALMACEN; i++){
            Cola res =  almacenes[i].equipajes;
            printf("El almacen %d tiene %d Equipajes del pais %s\n",almacenes[i].id + 1,res.log,almacenes[i].pais);
            cantidad += res.log;
        }
        printf("\t\tCintas:\n\n");
        for ( i = 0; i < MAX_CINTAS; i++){
            res =  cintas[i];
            j += 10;
            printf("mostrador %d-%d pasaron : %d equipajes\n",k+1,j,res.log);
            k += 10;
            equipajeAtorado += res.log;
            if(res.log == 0){
                cintasUso+=1;
            }
        }
        printf("===Almacenados===%d\n",cantidad);
        printf("===Equipajes Perdidos===%d\n",MAX_EQUIPAJES-cantidad);
        printf("===Se usaron esta cantidad de cintas===%d\n",cintasUso);
        printf("===Se atoraron en la cinta esta cantidad de Equipajes ===%d\n",equipajeAtorado);

        break;

    default:
        break;
    }
}
#endif