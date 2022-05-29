// #ifndef __DHT_H
// #define __DHT_H

// #include "stm8s.h"

// typedef struct
// {
//     uint8_t (*init)(GPIO_TypeDef *, GPIO_Pin);
//     uint8_t (*read)();
//     uint8_t (*readByte)();
//     uint8_t (*readBit)();
//     void (*reset)();
//     uint8_t (*check)();
//     uint8_t temperature;
//     uint8_t humidity;
// } DHT11_Module;

// extern Pin *DHT11_Pin;

// uint8_t DHT11_Init(GPIO_TypeDef *port, GPIO_Pin pin);
// void DHT11_Reset();
// uint8_t DHT11_Check();
// uint8_t DHT11_Read();
// uint8_t DHT11_ReadByte();
// uint8_t DHT11_ReadBit();
// extern DHT11_Module DHT11;

// #endif