#include "7_segment.h"

uint8_t Seven_Segment_Values[] = {
  0b11000000,
  0b11111001,
  0b10100100,
  0b10110000,
  0b10011001,
  0b10010010,
  0b10000010,
  0b11111000,
  0b10000000,
  0b10010000,
  0b10001000,
  0b10000011,
  0b11000110,
  0b10100001,
  0b10000110,
  0b10001110,
};

Pin* Seven_Segment_Init(Pin* pin, uint8_t value) {
    if(value >= len(Seven_Segment_Values)) {
        pin->value = 0;
    } else {
        pin->value = value;
    }

    GPIO.write(pin, Seven_Segment_Values[pin->value]);

    return pin;
}

void Seven_Segment_Write(Pin* pin, uint8_t value) {
    if(value >= len(Seven_Segment_Values)) {
        pin->value = 0;
    } else {
        pin->value = value;
    }

    GPIO.write(pin, Seven_Segment_Values[pin->value]);
}

const Seven_Segment_Module SevenSegment = {
  .init = Seven_Segment_Init,
  .write = Seven_Segment_Write
};