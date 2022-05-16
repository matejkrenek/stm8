#ifndef __Serial_H
#define __Serial_H

typedef struct
{
    void (*begin)(uint32_t);
    int (*write)(const char *, ...);
} Serial_Module;

void Serial_Begin(uint32_t BaudRate);
const extern Serial_Module Serial;

#endif