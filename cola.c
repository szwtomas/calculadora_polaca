#include "cola.h"
#include <stdlib.h>

typedef struct nodo
{
    void* dato;
    struct nodo* siguiente;
} nodo_t;

typedef struct cola
{
    nodo_t* primero;
    nodo_t* ultimo;
} cola_t;


nodo_t* nodo_crear(void* dato)
{
    nodo_t* nodo = malloc(sizeof(nodo_t));
    if(nodo == NULL) return NULL;

    nodo->dato = dato;
    nodo->siguiente = NULL;
    return nodo;
}

void nodo_destruir(nodo_t* nodo)
{
    free(nodo);
}

cola_t* cola_crear()
{
    cola_t* cola = malloc(sizeof(cola_t));
    if(cola == NULL) return NULL;

    cola->primero = NULL;
    cola->ultimo = NULL;
    return cola;
}

bool cola_esta_vacia(const cola_t* cola)
{
    return cola->primero == NULL;
}

void cola_destruir(cola_t *cola, void (*destruir_dato)(void *))
{
    while(!cola_esta_vacia(cola))
    {
        if(destruir_dato)
        {
            destruir_dato(cola->primero->dato);
        }    
        cola_desencolar(cola);
    }
    free(cola);
}

void *cola_ver_primero(const cola_t *cola)
{
    if(cola_esta_vacia(cola)) return NULL;

    return cola->primero->dato;
}

bool cola_encolar(cola_t *cola, void *valor)
{
    nodo_t* nodo = nodo_crear(valor);
    if(nodo == NULL) return false;

    if(cola_esta_vacia(cola))
    {
        cola->primero = nodo;
    }
    else
    {
        cola->ultimo->siguiente = nodo;
    }
    
    cola->ultimo = nodo;
    return true;
}

void* cola_desencolar(cola_t* cola)
{
    if(cola_esta_vacia(cola)) return NULL;

    nodo_t* nodo_desencolar = cola->primero;

    cola->primero = cola->primero->siguiente;
    if(nodo_desencolar->siguiente == NULL)
    {
        cola->ultimo = NULL;
    }

    void* dato = nodo_desencolar->dato;
    nodo_destruir(nodo_desencolar);

    return dato;
}
