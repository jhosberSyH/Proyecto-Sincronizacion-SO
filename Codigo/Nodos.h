#ifndef Nodos_h
#define Nodos_h
#include "Equipaje.h"
	typedef Equipaje Elemento;

	typedef struct apuntador{
		Elemento info;
		struct apuntador *prox; 	
	}Nodo;

	typedef struct {
		int log;
		Nodo *primero,*ultimo;
	}Cola;

#endif
