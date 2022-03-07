#include "stm8s.h"

// FUNCTIONS
/**
  * @brief  Deinitializes the GPIO registers to their default reset values.
  * @param  Port: Select the GPIO port.
  * @retval None
  * GPIO.deinit(Port) 
  * **/

 /**
  * @brief  Initializes the GPIO pin to act as output.
  * @param  PortPin: Select the GPIO portpin.
  * @param  Mode: Select the GPIO mode.
  * @retval Port
  * GPIO.output(PortPin, Mode) 
  * **/

  /**
  * @brief  Initializes the GPIO pin to act as input.
  * @param  PortPin: Select the GPIO portpin.
  * @param  Mode: Select the GPIO mode.
  * @retval Port
  * GPIO.input(Portv, Mode) 
  * **/

   /**
  * @brief  Write value in uint_8 to pin
  * @param  Port: Select the GPIO portpin.
  * @param  Data: 8 bit data to write.
  * @retval None
  * GPIO.write(Port, Data) 
  * **/

    /**
  * @brief  Write high to pin port
  * @param  v: Select the GPIO portpin.
  * @retval None
  * GPIO.writeLow(Pin) 
  * **/

    /**
  * @brief  Write low to pin port
  * @param  Pin: Select the GPIO pin.
  * @retval None
  * GPIO.writeHigh(Pin) 
  * **/

    /**
  * @brief  Write reverse value to pin port
  * @param  Pin: Select the GPIO pin.
  * @retval None
  * GPIO.writeReverse(Pin) 
  * **/

     /**
  * @brief  Read 0 or 1 from pin
  * @param  Pin: Select the GPIO pin.
  * @retval int (0, 1)
  * GPIO.read(Pin) 
  * **/
    /**
  * @brief  Read whole port
  * @param  Port: Select the GPIO port.
  * @retval uint8_t
  * GPIO.readData(Port) 
  * **/

void main(void)
{
    GPIO.deinit(ALL);

    Pin* Led1 = GPIO.output(PC5, PUSH_PULL_HIGH_FAST);
    Pin* Led2 = GPIO.output(PC4, PUSH_PULL_HIGH_FAST);
    Pin* Button1 = GPIO.input(PE4, PULL_UP_NO_IT);

    while(1) {
        if(GPIO.read(Button1)) {
            GPIO.writeLow(Led1);
        } else {
            GPIO.writeHigh(Led1);
        }

        GPIO.write(PB, 0b00000000);
        GPIO.readData(PB) // 0b00000000;

        GPIO.writeReverse(Led2);
        delay(1000);
    }
}

