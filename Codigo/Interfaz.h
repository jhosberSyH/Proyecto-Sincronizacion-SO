#ifndef Interfaz_h
#define Interfaz_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Cola.h"
#include "ColaEntero.h"
#include "Almacen.h"

#define MAX_MOSTRADORES 5000
#define MAX_CINTAS 500
#define MAX_ALMACEN 250
#define MAX_EQUIPAJES 120736

void usuario(int *requisito,ColaEntero *valores);
void menu(int *requisito);
int validarNumero(char num[]);
int opcion_valida(char num[]);
void respuestasFinal(int requisito,int almacenes[],int cintas[],int mostradores[]);
void incrementar(int id,int valores[]);
void inicializarInt(int n,int valores[]);
void mostrarInformacion(int id,int valores[],int n);
void especificaciones(int requisito,ColaEntero *valores);
void mostrarEspecificacion(int requisito,int hilo,ColaEntero valores,int etapa,Equipaje equipaje);

void usuario(int *requisito,ColaEntero *valores){
    menu(requisito);
    if(*requisito > 4){

        crearEntero(valores);
        especificaciones(*requisito,valores);
    }
    system("clear");
    printf("\t\tCargando....\n");
    printf("\t\tHey bro la funcion 5 esta en matenimiento\n");
}

void menu(int *requisito){
    int numero = -1;
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
    if (numero == 5){
        printf("\t+-------- MENU DE ESPECIFICACIONES --------+\n");
        printf("\t|                                          |\n");
        printf("\t| [5] Buscar Equipajes con Numero          |\n");
        printf("\t| [7]                                      |\n");
        printf("\t| [8]                                      |\n");
        printf("\t| [9]                                      |\n");
        printf("\t| [10]                                     |\n");
        printf("\t|                                          |\n");
        printf("\t+------------------------------------------+\n");
        printf("Opcion: ");
        do{
            scanf("%s",val);
            numero = validarNumero(val);
        }while(numero == 0);
        
        numero = atoi(val);
        system("clear");
    }
    *requisito = numero;
    
}

//verifica si la opcion de menu ingresada es valida
int opcion_valida(char num[]){
    int i = 0; 
    int len = strlen(num); 
    int valido = 1;

    valido = validarNumero(num);

    if ((valido == 1) && (atoi(num) < 1) || (atoi(num) > 5) ){ 
        printf("!No se ingreso un valor valido!\n"); 
        valido = 0;
    }

    if(valido == 0){
        printf("Intente de nuevo: "); 
    }

	return valido; 
}

int validarNumero(char num[]){
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

    if(valido == 0){
        printf("Intente de nuevo: "); 
    }

	return valido; 
}


void respuestasFinal(int requisito,int almacenes[],int cintas[],int mostradores[]){
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
void mostrarInformacion(int id,int valores[],int n){
    int cantidad = 0,total = 0,sinUso = 0,i;
    char nombrePlural[13],nombre[11];
    if(id == 1){
        strcpy(nombrePlural,"Mostradores");
        strcpy(nombre,"Mostrador");
    }else if(id == 2){
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

void especificaciones(int requisito,ColaEntero *valores){ // tinee detalles no funciona al 100% todavia
    int numero,n,i; 
    char val[20];
    switch (requisito){
    case 5:
        /*printf("Ingrese la cantidad de Equipaje que desea buscar:");
        do{
            scanf("%s",val);
            n = validarNumero(val);
        }while(n == 0);
        
        n = atoi(val);
        for (i = 0; i < n; i++){
            printf("\nIngrese el Numero de Equipaje %d que desee seguir:",i+1);
            do{
                scanf("%s",val);
                numero = validarNumero(val);
            }while(numero == 0);
            numero = atoi(val);
            encolarEntero(valores,numero);
        }
        */
        break;
    
    default:
        break;
    }
}
void mostrarEspecificacion(int requisito,int hilo,ColaEntero valores,int etapa,Equipaje equipaje){  // tinee detalles no funciona al 100% todavia
    /*if(requisito > 4){
        switch (requisito){
        case 5:
        
            int conseguido = 0,i = 0;
            while ((i < longitudEntero(valores)) && (!conseguido)){
                if(equipaje.id == primeroEntero(valores)){
                    conseguido = 1;
                    printf("\t+-------------------------------------------------------------------------+\n");
                    switch (etapa){
                    case 1:
                        printf("\t|El Equipaje Numero %d con destino a %s paso por el Mostrador %d\n",equipaje.id,equipaje.pais,hilo);
                        break;
                    case 2:
                        printf("\t|El Equipaje Numero %d con destino a %s paso por la Cinta %d\n",equipaje.id,equipaje.pais,hilo);
                        break;
                    case 3:
                        printf("\t|El Equipaje Numero %d con destino a %s paso por el Almacen %d\n",equipaje.id,equipaje.pais,hilo);
                        break;
                    case 4:
                        //avion
                        break;
                    case 5:
                        // destino 
                        break;
                    
                    default:
                        printf("\t\t\tNo existe esa etapa!\n");
                        break;
                    }
                }
                encolarEntero(&valores,valores.primero->info);
                desencolarEntero(&valores);
                i++;
            }
            
            break;
        
        default:
            break;
        }
    }*/
}
#endif