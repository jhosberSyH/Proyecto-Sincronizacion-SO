#ifndef Equipaje_h
#define Equipaje_h
#include <stdio.h>
#include <string.h>
#include "Cola.h"

typedef struct
{
    int id;
    char tipo[20]; //Tipo de equipaje (Mano, Sobredimensionado, Facturado, Especial)
    char ciudad[50];
    char pais[50];
    char vuelo[10];
    int idVuelo; //Identificador del vuelo al que pertenece el equipaje
    char codVuelo[10]; //Codigo del vuelo al que pertenece el equipaje
    int prioridad;
    char estado[20]; //Estado del equipaje (Descargar, Mantener, Listo)
    char fragilidad[5];
    float peso;
} Equipaje;

void construtorEquipaje(Equipaje *equipaje);
int traducirPrioridad(char prioridad[]);

void construtorEquipaje(Equipaje *equipaje)
{
    equipaje->id = 0;
    equipaje->peso = 0;
    equipaje->prioridad = 0;
}

int traducirPrioridad(char prioridad[])
{
    int res = 0;
    char Mano[] = "Mano", MANO[] = "MANO", mano[] = "mano";
    char Sb[] = "Sobredimensionado", SB[] = "SOBREDIMENSIONADO", sb[] = "sobredimensionado";
    char Facturado[] = "Facturado", FACTURADO[] = "FACTURADO", facturado[] = "facturado";
    if (((strcmp(Mano, prioridad) == 0) || (strcmp(MANO, prioridad) == 0) || (strcmp(mano, prioridad) == 0)))
    {
        res = 4;
    }
    else if (((strcmp(Sb, prioridad) == 0) || (strcmp(SB, prioridad) == 0) || (strcmp(sb, prioridad) == 0)))
    {
        res = 3;
    }
    else if (((strcmp(Facturado, prioridad) == 0) || (strcmp(FACTURADO, prioridad) == 0) || (strcmp(facturado, prioridad) == 0)))
    {
        res = 2;
    }
    else
    {
        res = 1;
    }
    return (res);
}

#endif
