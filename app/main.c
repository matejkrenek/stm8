#include "stm8s.h"
#include "Serial.h"
#include "timer.h"
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))

Pin *DHT11_Pin;

int Humidity = 0;
int Temp = 0;
int TempComma = 0;
bool DHTError = FALSE;

void DelayTimer(uint32_t DelayValue)
{
    uint32_t DelayTime = timer.micros();
    do
    {

    } while (timer.micros() - DelayTime < DelayValue);
}

// Main DHT Void

void DHT11()
{

    uint32_t DataTime = 0;

    uint8_t Result[45];
    uint8_t DataArray = 0;
    uint8_t DataCounter = 0;
    uint8_t DHTData[5];

    bool BlockDHT = FALSE;

    // Trigger Sensor (described in the Datasheet)

    DHT11_Pin = GPIO.init(GPIOD, PIN_1, OUTPUT_PP_LOW_FAST);
    GPIO.writeHigh(DHT11_Pin);
    DelayTimer(250000);
    GPIO.writeLow(DHT11_Pin);
    DelayTimer(30000);
    GPIO.writeHigh(DHT11_Pin);
    DelayTimer(50);
    DHT11_Pin = GPIO.init(GPIOD, PIN_1, INPUT_FL_NO_IT);

    // read the Bits and put them into a Result array (It will count 42 bits. The first two one are useless due my code)

    do
    {
        if (!GPIO.read(DHT11_Pin) && BlockDHT == FALSE)
        {
            BlockDHT = TRUE;
            Result[DataArray] = (timer.micros() - DataTime);
            DataArray++;
            DataTime = timer.micros();
        } // If DHT pin is low, go to next Dataset
        if (GPIO.read(DHT11_Pin))
        {
            BlockDHT = FALSE;
        } // As long as DHT pin is Hight add time in Microseconds to Result

    } while ((timer.micros() - DataTime) < 150); // if DTH Sensor high for more than 150 usec, leave loop

    // Asign 1 or 0 to Result variable. If more than 80uS Data as "1"
    // Starting at Data set 02. First two Datasets are ignored!

    for (int i = 2; i < DataArray; i++)
    {
        if (Result[i] <= 90)
            Result[i] = 0;
        else
            Result[i] = 1;
        // Serial.print(Result[i]);Serial.print(" ");
    }
    // Serial.println();

    for (int j = 0; j < 5; j++)
    { // redo it for the 5 Bytes (40 Databits /8 = 5)
        for (int i = 0; i < 8; i++)
        {
            bitWrite(DHTData[j], 7 - i, Result[i + 2 + (j * 8)]);
        } // Create 5 Databytes from the 40 Databits (Ignoring the 2 first Databits)
    }
    // check checksum                            }

    Humidity = DHTData[0];
    Temp = DHTData[2];
    TempComma = DHTData[3];
    DHTError = FALSE;
}

void main(void)
{
    CLK.HSI(HSIDIV1);
    Serial.begin(9600);
    timer.init();

    while (1)
    {
        DHT11();
        if (DHTError == FALSE)
        {
            Serial.write("Humidity = ");
            Serial.write("%d", Humidity);
            Serial.write("%%");
            Serial.write(" Temp = ");
            Serial.write("%d", Temp);
            Serial.write(",");
            Serial.write("%d", TempComma);
            Serial.write("°C \n");
        }
        else
            Serial.write("Error\n");
        DelayTimer(500000); // wait 2,5 sec
    }
}