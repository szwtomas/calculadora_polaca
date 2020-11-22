#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "strutil.h"
#include "calc_helper.h"
#include "cola.h"

//Libera la memoria toda la memoria dinamica asociada al conversor
void liberar_mem(pila_t* pila, cola_t* cola, char** strv)
{
    if(pila) pila_destruir(pila);
    if(cola) cola_destruir(cola, NULL);
    if(strv) free_strv(strv);
}

//Parsea del token al caracter del operador
//Pre-Condiciones: El operador es de suma, resta, producto, division o potencia
//Post: Devuelve el caracter correspondiente al operador 
char seleccionar_operador(calc_operador oper)
{
    switch(oper.op)
    {
        case(OP_ADD): return '+';
        case(OP_SUB): return '-';
        case(OP_MUL): return '*';
        case(OP_DIV): return '/';
        case(OP_POW): return '^';
        default: return '\0';
    }
    return '\0'; //No deberia llegar nunca aca
}

//Devuelve la cantidad de espacios necesarios para parsear un numero a cadena
size_t cantidad_digitos(calc_num num)
{
    if(num > -9 && num < 9) return 1;
    return 1 + cantidad_digitos(num / 10);
}

//Encola un token recibido como cadena
//Pre-Condiciones: El token es una operacion o un numero
//Post: Encola el string en la cola recibida
void encolar_token(cola_t* cola, struct calc_token* tok)
{
    size_t tam;
    char* str;
    if(tok->type == TOK_OPER)
    {
        str = malloc(2 * sizeof(char));
        char operador = seleccionar_operador(tok->oper);
        sprintf(str, "%c", operador);
    }
    else
    {
        tam = cantidad_digitos(tok->value);
        if(tok->value < 0) tam++;
        str = malloc((tam+1) * sizeof(char));
        sprintf(str, "%li", tok->value);
    }
    cola_encolar(cola, str);
}

//Desapila tokens de la pila y los encola hasta llegar a un parentesis, que es desapilado pero no encolado
//Pre: Los parentesis en la expresion deben estar todos balanceados
//Post: Los elementos fueron desapilados y encolados
void desapilar_hasta_parentesis(pila_t* pila, cola_t* cola, size_t* cont)
{
    struct calc_token* tok = pila_desapilar(pila);
    int i=0;
    while(tok->type != TOK_LPAREN)
    {
        
        encolar_token(cola, tok);
        (*cont)++;
        tok = pila_desapilar(pila);
        i++;
    }
}


//Devuelve true si es necesario seguir desapilando operadores, devuelve false si hay que cortar
bool seguir_desapilando(calc_operador op_1, calc_operador op_2)
{
    if((op_1.asociatividad == ASSOS_IZQ) && (op_1.precedencia <= op_2.precedencia)) return true;
    if((op_1.asociatividad == ASSOS_DER) && (op_1.precedencia < op_2.precedencia)) return true;
    return false;
}

//Desapila operadores dependiendo de su precedencia y asociatividad
void desapilar_operaciones(pila_t* pila, cola_t* cola, struct calc_token* tok, size_t* cont)
{
    struct calc_token* tok_2;
    while(!pila_esta_vacia(pila))
    {
        tok_2 = (struct calc_token*) pila_ver_tope(pila);
        if(tok_2->type != TOK_OPER) break;
        if(seguir_desapilando(tok->oper, tok_2->oper))
        {
            encolar_token(cola, pila_desapilar(pila));
            (*cont)++;
        }
        else
        {
            break;
        }
    }
}

//Encola todos los elementos restantes en la pila, que queda vacia
//Post: La pila queda vacia, sus elementos fueron encolados
void encolar_restantes(pila_t* pila, cola_t* cola, size_t* cont)
{
    while(!pila_esta_vacia(pila))
    {
        encolar_token(cola, pila_desapilar(pila));
        (*cont)++;
    }
}

//desencola los elementos en un arreglo de cadenas
//Pre: La cola tiene encolados en orden correcto la expresion polaca inversa como tokens
//Post: El arreglo tiene los strings en el orden correcto, la cola queda vacia
void cola_a_arr(cola_t* cola, char* strv[])
{
    size_t i=0;
    while(!cola_esta_vacia(cola))
    {
        strv[i] = cola_desencolar(cola);
        i++;
    }
    strv[i] = NULL;
}

//Devuelve el tamanio del arreglo ya spliteado, para poder armar el arreglo de operadores
size_t tam_strv(char** strv)
{
    size_t i=0;
    while(strv[i])
    {
        i++;
    }
    return i;
}

//Convierte una cadena en notacion posfija en una de notacion polaca inversa. Devuelve una cadena DINAMICA 
//que luego debe ser liberada por quien llama la funcion
//Pre-Condiciones: La linea recibida solo tiene los operadores +, -, ^, *, ( y ), todos los 
//parentesis estan bien balanceados, los numeros son enteros y correctamente espaceados
//Post: Devuelve una cadena con memoria dinamica de la linea recibida en notacion polaca invesa
char* conversor(char* linea)
{
    char** strv = infix_split(linea);
    struct calc_token tok;
    
    pila_t* pila = pila_crear();
    cola_t* cola = cola_crear();
    if(!cola || !pila || !strv)
    {
        liberar_mem(pila, cola, strv);
        return NULL;
    }
    struct calc_token operadores[tam_strv(strv)]; //guardo los operadores en un array para no apilar el mismo puntero
    size_t cant_op=0;

    size_t i=0;
    size_t cont=0; //cuenta la cantidad de elementos encolados
    while(strv[i])
    {
        calc_parse(strv[i], &tok);
        switch(tok.type)
        {
            case(TOK_NUM):
                encolar_token(cola, &tok);
                cont++;
                break;
            case(TOK_OPER):
                operadores[cant_op] = tok;
                desapilar_operaciones(pila, cola, &tok, &cont);
                pila_apilar(pila, &(operadores[cant_op]));
                cant_op++;
                break;
            case(TOK_LPAREN):
                operadores[cant_op] = tok;
                pila_apilar(pila, &(operadores[cant_op]));
                cant_op++;
                break;
            case(TOK_RPAREN):
                desapilar_hasta_parentesis(pila, cola, &cont);
                break;
            default:
                liberar_mem(pila, cola, strv);
                return NULL;
        }
        i++;
    }
    encolar_restantes(pila, cola, &cont);

    char** str_ordenado = malloc((cont+1) * sizeof(char*));
    cola_a_arr(cola, str_ordenado);

    liberar_mem(pila, cola, strv);
    char* str_polaca_inv = join(str_ordenado, ' ');
    free_strv(str_ordenado);
    return str_polaca_inv;
}

int main(int argc, char* argv[])
{
    char* linea = NULL;
    size_t tam;
    char* convertida;

    while(getline(&linea, &tam, stdin) != EOF)
    {
        convertida = conversor(linea);
        if(!convertida) break;
        fprintf(stdout, "%s\n", convertida);
        free(convertida);
    }

    free(linea);
    return 0;
}