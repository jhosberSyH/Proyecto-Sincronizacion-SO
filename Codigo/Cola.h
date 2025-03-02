#ifndef Cola_h
#define Cola_h
#include <stdlib.h>
#include "Nodos.h"

		// COLA
void crear(Cola *c);
int longitud(Cola c);
int esVacio(Cola c);
void encolar(Cola *c,Elemento elem);
Elemento primero(Cola c);
Elemento ultimo(Cola c);
void desencolar(Cola *c);
void eliminarTodo(Cola *c);
void encolarPrioridad(Cola *c, Elemento p);

void crear(Cola *c){
	c->primero = NULL;
	c->ultimo = NULL;
	c->log = 0;
}
int longitud(Cola c){
	int res;
	res = c.log;
	return (res);
}

int esVacio(Cola c){
	int res = 0;
	if(c.log == 0){
		res = 1;
	}
	return (res);
}

void encolar(Cola *c,Elemento elem){
	Nodo *q;
	q = (Nodo*)malloc(sizeof(Nodo));
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

Elemento primero(Cola c){
	Elemento res;
	res = c.primero->info;
	return (res);
}
Elemento ultimo(Cola c){
	Elemento res;
	res = c.ultimo->info;
	return (res);
}
void desencolar(Cola *c) {
    Nodo *q;
	q = c->primero;
    c->primero = q->prox;
	free(q);
	if (c->primero == NULL) {
        c->ultimo = NULL;
    }
    c->log = c->log - 1;
}

void eliminarTodo(Cola *c){
	int i,n;
	n = longitud(*c);
	for(i=0;i < n;i++){
		desencolar(c);
	}
	c->log = 0;
}

void encolarPrioridad(Cola *c, Elemento p) {
    Nodo *nuevo;
	nuevo = (Nodo*)malloc(sizeof(Nodo));
    nuevo->info = p;
    nuevo->prox = NULL;
    if ((c->primero == NULL) || (p.prioridad < c->primero->info.prioridad)) {
        if(c->log == 0){
            c->ultimo = nuevo;
        }else{
            nuevo->prox = c->primero;
        }
        c->primero = nuevo;
    } else {
        Nodo *actual = c->primero;
        while ((actual->prox != NULL) && (actual->prox->info.prioridad <= p.prioridad)) {
            actual = actual->prox;
        }
        nuevo->prox = actual->prox;
        actual->prox = nuevo;
        if(nuevo->prox == NULL){
            c->ultimo = nuevo;
        }
    }
    c->log++;
}
#endif
