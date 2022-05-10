#ifndef __DHT_H
#define __DHT_H

typedef struct
{
    void (*init)(uint8_t, uint8_t);
    void (*begin)();
    float (*readHumidity)();
    float (*readTemperature)(void);
    void (*computeHeatIndex)(float, float);
} DHT_Module;

void DHT_Init(uint8_t pin, uint8_t type);
void DHT_Begin();
float DHT_ReadHumidity(void);
float DHT_ReadTemperature(void);
void DHT_ComputeHeatIndex(float temperature, float humidity);
extern const DHT_Module DHT;

#endif