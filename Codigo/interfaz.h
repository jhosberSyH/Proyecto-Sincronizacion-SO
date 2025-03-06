#ifndef Interfaz_h
#define Interfaz_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "Cola.h"
#include "almacen.h"

#define MAX_MOSTRADORES 5000
#define MAX_CINTAS 500
#define MAX_ALMACEN 250
#define MAX_EQUIPAJES 120736
#define MAX_BUSQUEDA 3

void usuario(int *requisito,int valores[]);
void menu(int *requisito);
int validarNumero(char num[]);
int opcion_valida(char num[]);
void respuestasFinal(int requisito,int almacenes[],int cintas[],int mostradores[]);
void incrementar(int id,int valores[]);
void inicializarInt(int n,int valores[]);
void mostrarInformacion(int id,int valores[],int n);
void especificaciones(int requisito,int valores[]);
void mostrarEspecificacion(int requisito,int hilo,int valores[],int etapa,int situacion,Equipaje equipaje);
void registrar(int id,int etapa,Equipaje equipaje,FILE *file);
void entradaMostrarEspecificacion(int etapa,int hilo,Equipaje equipaje);
void salidaMostrarEspecificacion(int etapa,int hilo,Equipaje equipaje);

void usuario(int *requisito,int valores[]){
    menu(requisito);
    if(*requisito > 4){
        system("clear");
        especificaciones(*requisito,valores);
    }
    system("clear");
    printf("\t\tCargando....\n");
}
//Menu para interacturar con el usuario
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
    printf("\t| [5] Buscar 3 Equipajes por numero        |\n");
    printf("\t|                                          |\n");
    printf("\t+------------------------------------------+\n");
    printf("Opcion: ");
    do{
        scanf("%s",val);
    }while(opcion_valida(val) == 0);
    numero = atoi(val);
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

//Muestra todas las repuesta que tienen que esperar a que termine el programa
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

//Incrementar contenido del vector
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
//Mustra los resultados de respuestasFinal
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

//llena el vector de los equipajes que se van a buscar
void especificaciones(int requisito,int valores[]){ 
    int numero,n,i; 
    char val[20];
    printf("\tIngresa 3 numeros de equipaje que quieras ver su recorrido\n");
    for (i = 0; i < 3; i++){
        printf("\nIngrese el Numero de Equipaje %d que desee seguir:",i+1);
        do{
            scanf("%s",val);
            numero = validarNumero(val);
        }while(numero == 0);
        numero = atoi(val);
        valores[i]  = numero;
    }
}
int limpiar = -1;
//Muestra los equipajes que se buscan por numero
void mostrarEspecificacion(int requisito,int hilo,int valores[],int etapa,int situacion,Equipaje equipaje){
    if(requisito > 4){
        int conseguido = 0,i = 0;
        if(limpiar < 0){ //limpiar la pantalla de carga 
            limpiar++;
            system("clear");
        }
        while ((i < MAX_BUSQUEDA) && (!conseguido)){
            if(equipaje.id == valores[i]){
                conseguido = 1;
                if(situacion == 1 ){
                    entradaMostrarEspecificacion(etapa,hilo,equipaje);
                }else{
                    salidaMostrarEspecificacion(etapa,hilo,equipaje);
                }  
            }
            i++;
        }
    }
}

//Escribe el log en su respectivo archivo
void registrar(int id,int etapa,Equipaje equipaje,FILE *file){
    char nombre[10];
    switch (etapa){
    case 1:
        strcpy(nombre,"Mostrador");
        break;
    case 2:
        strcpy(nombre,"Cinta");
        break;
    case 3:
        strcpy(nombre,"Almacen");
        break;
    default:
        printf("\t\tError al registar");
        break;
    }
    fprintf(file,"El Equipaje %s numero %d con peso %.2f y destino a %s %s paso por %s %d\n",equipaje.tipo,equipaje.id,equipaje.peso ,equipaje.ciudad, equipaje.pais,nombre,id+1);
}

//Muestra la entrada del equipaje a su respectiva etapa
void entradaMostrarEspecificacion(int etapa,int hilo,Equipaje equipaje){
    int random = (rand() % 3) + 1;
    printf("\t+---------------------------------------------------------------------------------------------------------+\n");
    switch (etapa){
        case 1:
            printf("\t|El Equipaje Numero %d con destino a %s Entro al Mostrador %d\n",equipaje.id,equipaje.pais,hilo + 1);
            sleep(random);
            break;
        case 2:
            printf("\t|El Equipaje Numero %d con destino a %s Entro a la Cinta %d\n",equipaje.id,equipaje.pais,hilo + 1);
            sleep(random);
            break;
        case 3:
            printf("\t|El Equipaje Numero %d con destino a %s Entro al Almacen %d\n",equipaje.id,equipaje.pais,hilo + 1);
            sleep(random);
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

//Muestra la salida del equipaje de su respectiva etapa
void salidaMostrarEspecificacion(int etapa,int hilo,Equipaje equipaje){
    printf("\t+========================================================================================================+\n");
    switch (etapa){
        case 1:
            printf("\t|El Equipaje Numero %d con destino a %s se le asigno un numero de equipaje unico y salio del Mostrador %d\n",equipaje.id,equipaje.pais,hilo + 1);
            break;
        case 2:
            printf("\t|El Equipaje Numero %d con destino a %s se clasifico y salio de la Cinta %d\n",equipaje.id,equipaje.pais,hilo + 1);
            break;
        case 3:
            printf("\t|El Equipaje Numero %d con destino a %s se organizo y salio del Almacen %d\n",equipaje.id,equipaje.pais,hilo + 1);
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
#endif