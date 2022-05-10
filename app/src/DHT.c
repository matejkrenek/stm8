#include "stm8s.h"
#include "DHT.h"

const DHT_Module DHT = {
    .init = DHT_Init,
    .begin = DHT_Begin,
    .readHumididy = DHT_ReadHumidity,
    .readTemperature = DHT_ReadTemperature,
    .computeHeatIndex = DHT_ComputeHeatIndex,
};