#ifndef __DSRTC_H
#define __DSRTC_H

#include "stm8s.h"

typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t dayOfWeek;
    uint8_t dayOfMonth;
    uint8_t month;
    uint8_t year;
} DateTime;

typedef struct
{
    void (*init)(uint8_t);
    void (*read)(uint8_t *);
    void (*write)();
    DateTime *(*dateTime)();
    void (*setDateTime)(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
} DSRTC_Module;

extern uint8_t _dsrtc_address;
extern Pin *DSRTC_SDA;
extern Pin *DSRTC_SCL;

void DSRTC_Init(uint8_t address);
void DSRTC_Read(uint8_t *buffer);
void DSRTC_Write();
DateTime *DSRTC_DateTime();
void DSRTC_SetDateTime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint8_t year);
extern DSRTC_Module DSRTC;

#endif