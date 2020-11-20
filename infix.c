#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "strutil.h"
#include "calc_helper.h"
#include "cola.h"

void liberar_memoria(pila_t* pila, cola_t* cola, char** strv)
{
    if(pila) pila_destruir(pila);
    if(cola) cola_destruir(cola, NULL);
    if(strv) free_strv(strv);
}

//devuelve falso si hay que seguir desapilando, verdadero si hay que cortar
bool comparar_operadores(calc_operador op_1, calc_operador op_2)
{
    if((op_1.asociatividad == ASSOS_IZQ) && (op_1.precedencia <= op_2.precedencia)) return false;
    if((op_1.asociatividad == ASSOS_DER) && (op_1.precedencia < op_2.precedencia)) return false;
    return true;
}

void desapilar_operaciones(pila_t* pila, cola_t* cola, struct calc_token* tok, size_t* cont)
{
    asignar_precedencia(tok);
    asignar_asociatividad(tok);
    struct calc_token* tok_2;
    bool cortar = false;

    while(!pila_esta_vacia(pila) && !cortar)
    {
        tok_2 = (struct calc_token*) pila_ver_tope(pila);
        if(tok_2->type == TOK_OPER)
        {
            asignar_precedencia(tok_2);
            asignar_asociatividad(tok_2);
            cortar = comparar_operadores(tok->oper, tok_2->oper);
            if(!cortar)
            {
                cola_encolar(cola, pila_desapilar(pila));
                (*cont)++;
            }
        }
        else
        {
            cortar = true;
        }
        
    }
}

void encolar_operaciones(pila_t* pila, cola_t* cola, size_t* cont)
{
    struct calc_token* aux;
    if(pila_esta_vacia(pila)) return;
    aux = (struct calc_token*) pila_ver_tope(pila);
    while(aux->type != TOK_LPAREN || !pila_esta_vacia(pila)) //Si los parentesis estan completos no se vacia la pila
    {
        cola_encolar(cola, pila_desapilar(pila));
        *(cont)++;
        aux = (struct calc_token*) pila_ver_tope(pila);
    }
    pila_desapilar(pila); //desapilo el parentesis
}

char* conversor(char* linea)
{
    char** strv = infix_split(linea);
    struct calc_token tok;
    
    pila_t* pila = pila_crear();
    cola_t* cola = cola_crear();
    if(!cola || !pila || !strv)
    {
        liberar_memoria(pila, cola, strv);
        return NULL;
    }
    size_t cont=0;
    size_t i=0;
    while(strv[i])
    {
        calc_parse(strv[i], &tok);
        if(tok.type == TOK_NUM)
        {
            cola_encolar(cola, &tok);
            cont++;
        }
        else if(tok.type == TOK_OPER)
        {
            desapilar_operaciones(pila, cola, &tok, &cont);
            pila_apilar(pila, &tok);
        }
        else if(tok.type == TOK_LPAREN)
        {
            pila_apilar(pila, &tok);
        }
        else
        {
            encolar_operaciones(pila, cola, &cont);
        }
        while(!pila_esta_vacia(pila))
        {
            cola_encolar(cola, pila_desapilar(pila));
            cont++;
        }
        i++;
    }
}


int main(int argc, char* argv[])
{
    char* linea = NULL;
    size_t tam = 0;
    char* convertida;

    while(getline(&linea, &tam, stdin) != EOF)
    {
        convertida = conversor(linea); //con memoria dinamica
        if(!convertida)
        {
            free(linea);
            return 1;
        }
        fprintf(stdout, "%s\n", convertida);
        free(convertida);
    }
    free(linea);
    return 0;
}

