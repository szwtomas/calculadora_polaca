#include "pila.h"

#include <stdlib.h>
#include <stdio.h>

#define CAPACIDAD_INICIAL 10
#define FACTOR_AUMENTO 2
#define FACTOR_MINIMO 4
#define COCIENTE_DISMINUCION 2

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila
{
    void **datos;	   // Arreglo de punteros genericos
    size_t cantidad;   // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

typedef struct pila pila_t;

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...

pila_t* pila_crear()
{
	pila_t* pila = malloc(sizeof(pila_t));
	if(pila == NULL)
	{
		return NULL;
	}

	pila->datos = malloc(CAPACIDAD_INICIAL * sizeof(void*));
	if(pila->datos == NULL)
	{
		free(pila);
		return NULL;
	}

	pila->capacidad = CAPACIDAD_INICIAL;
	pila->cantidad = 0;
	return pila;
}

void pila_destruir(pila_t *pila)
{
	free(pila->datos);
	free(pila);	
}

bool pila_esta_vacia(const pila_t *pila)
{
	return (pila->cantidad) == 0;
}


bool pila_redimensionar(pila_t* pila, size_t cap_nueva)
{
	void* datos_nuevo = realloc(pila->datos, sizeof(void*) * cap_nueva);
	if(cap_nueva > 0 && datos_nuevo == NULL)
	{
		return false;
	}

	pila->datos = datos_nuevo;
	pila->capacidad = cap_nueva;
	return true;
}


bool pila_apilar(pila_t *pila, void *valor)
{
	if(pila->cantidad == pila->capacidad)
	{
		size_t cap_nueva = FACTOR_AUMENTO * pila->capacidad;
		bool redimension = pila_redimensionar(pila, cap_nueva);
		if(!redimension)
		{
			return false;
		} 
	}

	pila->datos[pila->cantidad] = valor;
	pila->cantidad++;
	return true;
}

void *pila_desapilar(pila_t *pila)
{
	if(pila->cantidad == 0)
	{
		return NULL;
	}

	if(((pila->cantidad * FACTOR_MINIMO) < (pila->capacidad)) && (pila->capacidad > CAPACIDAD_INICIAL))
	{
		size_t cap_nueva = pila->capacidad / COCIENTE_DISMINUCION;
		pila_redimensionar(pila, cap_nueva);
	}

	(pila->cantidad)--;
	return pila->datos[pila->cantidad];
}

void *pila_ver_tope(const pila_t *pila)
{
	if(pila_esta_vacia(pila))
	{
		return NULL;
	}

	return pila->datos[pila->cantidad - 1];
}
