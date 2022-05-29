// #include "DHT.h"
// #include "Serial.h"

// Pin *DHT11_Pin;

// DHT11_Module DHT11 = {
//     .init = DHT11_Init,
//     .read = DHT11_Read,
//     .readByte = DHT11_ReadByte,
//     .readBit = DHT11_ReadBit,
//     .reset = DHT11_Reset,
//     .check = DHT11_Check,
// };

// uint8_t DHT11_Init(GPIO_TypeDef *port, GPIO_Pin pin)
// {
//     DHT11_Pin = GPIO.init(port, pin, OUTPUT_PP_HIGH_FAST);
//     DHT11.reset();

//     return DHT11.check();
// }

// uint8_t DHT11_ReadByte()
// {
//     uint8_t data = 0;

//     for (uint8_t i = 0; i < 8; i++)
//     {
//         data <<= 1;
//         data |= DHT11.readBit();
//     }
//     return data;
// }

// uint8_t DHT11_ReadBit()
// {
//     uint8_t retry = 0;

//     while (GPIO.read(DHT11_Pin) && retry < 100)
//     {
//         retry++;
//         delay.us(1);
//     }

//     retry = 0;

//     while (!GPIO.read(DHT11_Pin) && retry < 100)
//     {
//         retry++;
//         delay.us(1);
//     }

//     delay.us(40);
//     if (GPIO.read(DHT11_Pin))
//     {
//         return 1;
//     }
//     else
//     {
//         return 0;
//     }
// }

// uint8_t DHT11_Read()
// {
//     uint8_t buf[5];

//     DHT11.reset();

//     if (!DHT11.check())
//     {
//         for (uint8_t i = 0; i < 5; i++)
//         {
//             buf[i] = DHT11.readByte();
//         }
//         if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4])
//         {
//             for (uint8_t i = 0; i < 5; i++)
//             {
//                 Serial.write("%u", buf[i]);
//             }
//         }
//     }
//     else
//     {
//         return 1;
//     }

//     return 0;
// }

// void DHT11_Reset()
// {
//     DHT11_Pin = GPIO.init(DHT11_Pin->port, DHT11_Pin->pin, OUTPUT_PP_HIGH_FAST);
//     GPIO.writeLow(DHT11_Pin);
//     delay.ms(20);
//     GPIO.writeHigh(DHT11_Pin);
//     delay.us(30);
// }

// uint8_t DHT11_Check()
// {
//     uint8_t retry = 0;
//     DHT11_Pin = GPIO.init(DHT11_Pin->port, DHT11_Pin->pin, INPUT_FL_NO_IT);

//     while (!GPIO.read(DHT11_Pin) && retry < 100)
//     {
//         retry++;
//         delay.us(1);
//     }

//     if (retry >= 100)
//     {
//         return 1;
//     }
//     else
//     {
//         retry = 0;
//     }

//     while (GPIO.read(DHT11_Pin) && retry < 100)
//     {
//         retry++;
//         delay.us(1);
//     }

//     if (retry >= 100)
//     {
//         return 1;
//     }

//     return 0;
// }
