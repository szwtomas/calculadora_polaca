#include "strutil.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

char* substr(const char* str, size_t n)
{
    char* substr = malloc((n+1) * sizeof(char));
    if(!substr) return NULL;
    size_t largo = strlen(str);
    strcpy(substr, "");
    
    size_t i=0;
    while((i < largo) && (i < n))
    {
        substr[i] = str[i];
        i++;
    }
    substr[i] = '\0';
    return substr;
}

void free_strv(char *strv[])
{
    size_t i=0;
    while(strv[i])
    {
        free(strv[i]);
        i++;
    }
    free(strv);
}

//Devuelve la cantidad de apariciones del separador en el string
size_t cant_separadores(const char *str, char sep)
{
    size_t i=0;
    size_t cant=0;
    while(str[i] != '\0')
    {
        if(str[i] == sep) cant++;
        i++;
    }
    return cant;
}

char **split(const char *str, char sep)
{
    size_t cant = cant_separadores(str, sep);
    char** strv = malloc((cant+2) * sizeof(char*));
    if(!strv) return NULL;

    size_t i=0, j=0, k=0;
    while(str[i] != '\0')
    {
        if(str[i] == sep)
        {
            strv[k] = substr(str + j, i - j);
            if(!strv[k])
            {
                free_strv(strv);
                return NULL;
            }
            k++;
            j=i+1;
        }
        i++;
    }
    strv[k] = substr(str + j, i - j);
    if(!strv[k])
    {
        free_strv(strv);
        return NULL;
    }
    strv[k+1] = NULL;
    return strv;
}

//devuelve la cantidad de caracteres del string original, y la cantidad de separaciones del array split por referencia
size_t largo_total(char **strv, size_t* cant_sep)
{
    size_t i=0;
    size_t largo=0;
    while(strv[i])
    {
        largo += strlen(strv[i]);
        i++;
    }
    *cant_sep = i;
    return largo;
}

char *join(char **strv, char sep)
{
    size_t cant_sep;
    size_t largo = largo_total(strv, &cant_sep);
    
    if(cant_sep == 0)
    {
        char* str = malloc(sizeof(char));
        if(!str) return NULL;
        str[0] = '\0';
        return str;
    }
    char* str = malloc((largo + cant_sep) * sizeof(char));
    if(!str) return NULL;

    size_t j=0, k=0;
    for(size_t i=0; i < cant_sep; i++)
    {
        j=0;
        while(strv[i][j] != '\0')
        {
            str[k] = strv[i][j];
            k++;
            j++;
        }
        if(sep != '\0' || i == (cant_sep - 1))
        {
            str[k] = sep;
            k++;    
        }
    }
    str[k-1] = '\0';
    return str;
}
