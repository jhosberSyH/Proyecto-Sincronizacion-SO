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

int menu();
int opcion_valida(char num[]);
void respuestasFinal(int requisito,int almacenes[],int cintas[],int mostradores[]);
void incrementar(int id,int valores[]);
void inicializarInt(int n,int valores[]);
void mostrarInformacion(int identificador,int valores[],int n);

//interfaz del menu
int menu(){
    char val[20];
    system("clear");
    printf("\n\t\t\tBienvenido\n\n\n");
    printf("\t+------------------ MENU ------------------+\n");
    printf("\t|                                          |\n");
    printf("\t| [1] Informacion Mostrador                |\n");
    printf("\t| [2] Informacion Cintas                   |\n");
    printf("\t| [3] Informacion Almacen                  |\n");
    printf("\t| [4] Mostrar Todo                         |\n");
    printf("\t| [5] Informacion Especifica               |\n");
    printf("\t|                                          |\n");
    printf("\t+------------------------------------------+\n");
    printf("Opcion: ");

    do{
        scanf("%s",val);
    }while(opcion_valida(val) == 0);
    
    system("clear");
    printf("\t\tCargando....\n");
    return (atoi(val));
}

//verifica si la opcion de menu ingresada es valida
int opcion_valida(char num[]){
    int i = 0; 
    int len = strlen(num); 
    int valido = 1;

    while(i < len && valido == 1){
        if (!(isdigit(num[i]))){ 
            printf("!Solo se permiten numeros!\n"); 
            valido = 0;
        }
        i++;
    }

    if ((valido == 1) && (atoi(num) < 1) || (atoi(num) > 5) ){ 
        printf("!No se ingreso un valor valido!\n"); 
        valido = 0;
    }

    if(valido == 0){
        printf("Intente de nuevo: "); 
    }

	return valido; 
}


void respuestasFinal(int requisito,int almacenes[],int cintas[],int mostradores[]){
    system("clear");
    switch (requisito){
    case 1:
        mostrarInformacion(1,mostradores,MAX_MOSTRADORES);
        break;

    case 2:
        mostrarInformacion(2,cintas,MAX_CINTAS);
        break;
    
    case 3:
        mostrarInformacion(3,almacenes,MAX_ALMACEN);
            break;
    case 4:
        mostrarInformacion(1,mostradores,MAX_MOSTRADORES);
        mostrarInformacion(2,cintas,MAX_CINTAS);
        mostrarInformacion(3,almacenes,MAX_ALMACEN);
        break;
    default:
        break;
    }
}

//funciones necesarias 
void incrementar(int id,int valores[]){
    valores[id] = valores[id] + 1;
}

//rellenar un arreglo con 0
void inicializarInt(int n,int valores[]){
    int i;
    for (i = 0; i < n; i++){
        valores[i] = 0;
    }
    
}
void mostrarInformacion(int identificador,int valores[],int n){
    int cantidad = 0,total = 0,sinUso = 0,i;
    char nombrePlural[13],nombre[11];
    if(identificador == 1){
        strcpy(nombrePlural,"Mostradores");
        strcpy(nombre,"Mostrador");
    }else if(identificador == 2){
        strcpy(nombrePlural,"Cintas");
        strcpy(nombre,"Cinta");
    }else{
        strcpy(nombrePlural,"Almacenes");
        strcpy(nombre,"Almacen");
    }
    printf("\t+----------------------------------------------------------------+\n");
    for ( i = 0; i < n; i++){
        cantidad =  valores[i];
        printf("\t|En el %s %d pasaron %d equipajes\n",nombre,i+1,cantidad);
        total += cantidad;
        if(cantidad == 0){
            sinUso += 1;
        }
    }
    printf("\t+----------------------------------------------------------------+\n");
    printf("\t|Se usaron esta cantidad de %s = %d                   |\n",nombrePlural,n-sinUso);
    printf("\t|Pasaron un total de %d equipajes por los %d %s   |\n",total,n,nombrePlural);
    printf("\t+----------------------------------------------------------------+\n");
}
#endif