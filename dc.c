#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "strutil.h"
#include "calc_helper.h"

void mostrar_resultado(bool res_valido, long int resultado)
{
    if(!res_valido)
    {
        fprintf(stdout, "%s\n", "ERROR");
        return;
    }
    fprintf(stdout, "%li\n", resultado);
}

void liberar_memoria(pilanum_t* pila, char** strv)
{
    pilanum_destruir(pila);
    free_strv(strv);
}

bool suma(calc_num sum_1, calc_num sum_2, calc_num* res)
{
    *res = sum_1 + sum_2;
    return true;
}

bool resta(calc_num sustraendo, calc_num minuendo, calc_num* res)
{
    *res = minuendo - sustraendo;
    return true;
}

bool multiplicacion(calc_num fac_1, calc_num fac_2, calc_num* res)
{
    *res = fac_1 * fac_2;
    return true;
}

bool division_entera(calc_num divisor, calc_num dividendo, calc_num* res)
{
    if(divisor == 0) return false;
    *res = dividendo / divisor;
    return true;
}

bool potencia(calc_num exponente, calc_num base, calc_num* res)
{
    if(exponente < 0) return false;
    if(exponente == 0)
    {
        *res = 1;
        return true;
    }
    *res = base;
    for(size_t i=0; i < exponente-1; i++)
    {
        *res *= base;
    }
    return true;
}

bool ternario(calc_num opcion_cero, calc_num opcion_no_cero, calc_num condicion, calc_num* res)
{
    *res = (condicion == 0) ? opcion_cero : opcion_no_cero;
    return true;
}

bool logaritmo(calc_num base, calc_num argumento, calc_num* res)
{
    if(base < 2) return false;

    double log_1 = log((double) argumento);
    double log_2 = log((double) base);
    double cociente = (double) log_1 / log_2;

    *res = (calc_num) cociente;
    return true;
}

calc_num cuadrado(calc_num n)
{
    return n * n;
}

calc_num _raiz_entera(calc_num inicio, calc_num fin, calc_num n)
{
	long int mitad = (inicio + fin) / 2;
	if((cuadrado(mitad) <= n) && (cuadrado(mitad+1) > n)) return mitad;
	if(cuadrado(mitad) > n) return _raiz_entera(inicio, mitad - 1, n);
	return _raiz_entera(mitad+1, fin, n);
}

bool raiz_entera(calc_num n, calc_num* res)
{
    if(n < 0) return false;
    *res = _raiz_entera(0, n, n);
    return true;
}


bool calcular(calc_num operandos[], enum oper_type op, calc_num* res)
{
    switch(op)
    {
        case(OP_ADD): return suma(operandos[0], operandos[1], res);
        case(OP_SUB): return resta(operandos[0], operandos[1], res);
        case(OP_MUL): return multiplicacion(operandos[0], operandos[1], res);
        case(OP_DIV): return division_entera(operandos[0], operandos[1], res);
        case(OP_POW): return potencia(operandos[0], operandos[1], res);
        case(OP_LOG): return logaritmo(operandos[0], operandos[1], res);
        case(OP_RAIZ): return raiz_entera(operandos[0], res);
        case(OP_TERN): return ternario(operandos[0], operandos[1], operandos[2], res);
        default: return false;
    }
}


//Desapila los elementos necesarios, y apila el resultado de la operacion
//En caso de error devuelve false
bool realizar_operacion(pilanum_t* pila, calc_operador oper)
{
    calc_num res;
    calc_num operandos[oper.cant_operandos];
    for(size_t i=0; i<oper.cant_operandos; i++)
    {
        if(!desapilar_num(pila, &(operandos[i])))
        {
            return false;
        }
    }
    bool ok = calcular(operandos, oper.op, &res);
    if(!ok) return false;

    apilar_num(pila, res);
    return true;
}

//Parsea y calcula el resultado de la linea pasada
//Almacena el resultado final en 'resultado'
//En caso de tener cualquier error, devuelve false
bool calcular_linea(char* linea, long int* resultado)
{
    char** strv = dc_split(linea);
    if(!strv) return false;

    pilanum_t* pila = pilanum_crear();
    struct calc_token tok;
    bool token_valido;

    size_t i=0;
    while(strv[i])
    {
        
        token_valido = calc_parse(strv[i], &tok);
        if(!token_valido)
        {
            liberar_memoria(pila, strv);
            return false;
        }

        if(tipo_token(tok) == TOK_NUM)
        {
            apilar_num(pila, tok.value);
        }
        else if(tipo_token(tok) == TOK_OPER)
        {
            cantidad_operandos(&(tok.oper));
            token_valido = realizar_operacion(pila, tok.oper);
            if(!token_valido)
            {
                liberar_memoria(pila, strv);
                return false;
            }
        }
        else
        {   
            liberar_memoria(pila, strv);
            return false;
        }
        i++;
    }   
    calc_num res;
    desapilar_num(pila, &res);
    *resultado = (long int) res;
    if(!pila_esta_vacia(pila))
    {
        liberar_memoria(pila, strv);
        return false;
    }

    liberar_memoria(pila, strv);
    return true;
}

int main(int argc, char* argv[])
{
    char* linea = NULL;
    size_t tam = 0;
    long int resultado;
    bool res_valido;
    while(getline(&linea, &tam, stdin) != EOF)
    {
        res_valido = calcular_linea(linea, &resultado);
        mostrar_resultado(res_valido, resultado);
    }
    free(linea);

    return 0;
}