#ifndef __str_H
#define __str_H

#include "stm8s.h"

/**
 * @brief  Definition of the Serial struct to communicate with serial monitor.
 */
typedef struct
{
    void (*prepend)(char *, const char *);
    size_t (*length)(const char *);
} String_Module;

void String_Prepend(char *s, const char *t);
size_t String_Length(const char *t);

extern String_Module str;

#endif