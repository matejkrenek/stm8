#include "stm8s.h"
#include "__assert__.h"

#define A GPIOA
#define B GPIOB
#define C GPIOC
#define D GPIOD
#define E GPIOE
#define F GPIOF
#define G GPIOG
#define I GPIOI
#define Pin GPIO_Pin_TypeDef
#define Port GPIO_TypeDef
#define Mode GPIO_Mode_TypeDef

typedef enum
{
  _0    = ((uint8_t)0x01),
  _1    = ((uint8_t)0x02),
  _2    = ((uint8_t)0x04),
  _3    = ((uint8_t)0x08),
  _4    = ((uint8_t)0x10),
  _5    = ((uint8_t)0x20),
  _6    = ((uint8_t)0x40),
  _7    = ((uint8_t)0x80), 
  _all  = ((uint8_t)0xFF)
} Pin;

typedef enum
{
  INPUT   = (uint8_t)0x00,  /*!< Input floating, no external interrupt */
  OUTPUT  = (uint8_t)0xC0,  /*!< Output push-pull, low level, 2MHz */
} Mode;


typedef struct {
    Pin (*pin)(Port*, Pin, Mode);
    int (*read)(Port*, Pin);
    void (*low)(Port*, Pin);
    void (*high)(Port*, Pin);
    void (*reverse)(Port*, Pin);
    void (*disable)(Port*, Pin);
    void (*enable)(Port*, Pin);
    void (*deinit)();
} GPIO;

Pin pin_func(Port* port, Pin pin, Mode mode) {
    GPIO_Init(port, pin, mode);

    return pin;
}

void pin_low(Port* port, Pin pin) {
    port->ODR &= (uint8_t)(~pin);
}

void pin_high(Port* port, Pin pin) {
    port->ODR |= (uint8_t)pin;
}

void pin_reverse(Port* port, Pin pin) {
    port->ODR ^= (uint8_t)pin;
}

int pin_read(Port* port, Pin pin) {
    switch (GPIO_ReadInputPin(port, pin)) {
        case RESET:
            return 1;
        default:
            return 0;
    }
}

void pin_disable(Port* port, Pin pin) {
    GPIO_ExternalPullUpConfig(port, pin, DISABLE);
}

void pin_enable(Port* port, Pin pin) {
    GPIO_ExternalPullUpConfig(port, pin, ENABLE);
}

void deinit() {
    GPIO_DeInit(_all);
}

GPIO gpio = {
    .pin = pin_func,
    .low = pin_low,
    .high = pin_high,
    .reverse = pin_reverse,
    .read = pin_read,
    .disable = pin_disable,
    .enable = pin_enable,
    .deinit = deinit
};