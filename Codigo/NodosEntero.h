#ifndef NodosEntero_h
#define NodosEntero_h
	typedef int ElementoEntero;

	typedef struct apuntadorEntero{
		ElementoEntero info;
		struct apuntadorEntero *prox; 	
	}NodoEntero;

	typedef struct {
		int log;
		NodoEntero *primero,*ultimo;
	}ColaEntero;
#endif