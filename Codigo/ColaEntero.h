#ifndef ColaEntero_h
#define ColaEntero_h
#include <stdlib.h>
#include "NodosEntero.h"

		// Cola Entero
void crearEntero(ColaEntero *c);
int longitudEntero(ColaEntero c);
int esVacioEntero(ColaEntero c);
void encolarEntero(ColaEntero *c,ElementoEntero elem);
ElementoEntero primeroEntero(ColaEntero c);
ElementoEntero ultimoEntero(ColaEntero c);
void desencolarEntero(ColaEntero *c);
void eliminarTodoEntero(ColaEntero *c);

void crearEntero(ColaEntero *c){
	c->primero = NULL;
	c->ultimo = NULL;
	c->log = 0;
}
int longitudEntero(ColaEntero c){
	int res;
	res = c.log;
	return (res);
}

int esVacioEntero(ColaEntero c){
	int res = 0;
	if(c.log == 0){
		res = 1;
	}
	return (res);
}

void encolarEntero(ColaEntero *c,ElementoEntero elem){
	NodoEntero *q;
	q = (NodoEntero*)malloc(sizeof(NodoEntero));
	q->info = elem;
	q->prox = NULL;
	if(c->log == 0){
		c->primero = q;
	}else{
		c->ultimo->prox = q;
	}
	c->ultimo = q;
	c->log = c->log+1;
}

ElementoEntero primeroEntero(ColaEntero c){
	ElementoEntero res;
	res = c.primero->info;
	return (res);
}
ElementoEntero ultimoEntero(ColaEntero c){
	ElementoEntero res;
	res = c.ultimo->info;
	return (res);
}
void desencolarEntero(ColaEntero *c) {
    NodoEntero *q;
	q = c->primero;
    c->primero = q->prox;
	free(q);
	if (c->primero == NULL) {
        c->ultimo = NULL;
    }
    c->log = c->log - 1;
}

void eliminarTodoEntero(ColaEntero *c){
	int i,n;
	n = longitudEntero(*c);
	for(i=0;i < n;i++){
		desencolarEntero(c);
	}
	c->log = 0;
}



#endif