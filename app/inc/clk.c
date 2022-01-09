#include "stm8s.h"
#include "__assert__.h"

typedef enum {
  hsi1   = (uint8_t)0x00,
  hsi2   = (uint8_t)0x08,
  hsi4   = (uint8_t)0x10,
  hsi8   = (uint8_t)0x18,
  cpu1   = (uint8_t)0x80,
  cpu2   = (uint8_t)0x81,
  cpu4   = (uint8_t)0x82,
  cpu8   = (uint8_t)0x83,
  cpu16  = (uint8_t)0x84,
  cpu32  = (uint8_t)0x85,
  cpu64  = (uint8_t)0x86,
  cpu128 = (uint8_t)0x87
} CLK_Prescaler_TypeDef;

typedef struct {
    void (*hse)(bool);
    void (*hsi)(bool);
    void (*hsi_config)(CLK_Prescaler_TypeDef);
    void (*lsi)(bool);
    void (*cco)(bool);
    void (*deinit)();
} CLK_s;

void clk_deinit() {
    CLK_DeInit();
}

void clk_hse(bool status) {
    FunctionalState states[] = {DISABLE, ENABLE};

    CLK_HSECmd(states[status]);
}

void clk_hsi(bool status) {
    FunctionalState states[] = {DISABLE, ENABLE};

    CLK_HSICmd(states[status]);
}

void clk_hsi_config(CLK_Prescaler_TypeDef config) {
    CLK_HSIPrescalerConfig(config);
}

void clk_lsi(bool status) {
    FunctionalState states[] = {DISABLE, ENABLE};

    CLK_LSICmd(states[status]);
}

void clk_cco(bool status) {
    FunctionalState states[] = {DISABLE, ENABLE};

    CLK_CCOCmd(states[status]);
}

CLK_s clk = {
    .hse = clk_hse,
    .hsi = clk_hsi,
    .hsi_config = clk_hsi_config,
    .lsi = clk_lsi,
    .cco = clk_cco,
    .deinit = clk_deinit,
};