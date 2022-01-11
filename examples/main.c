void main() {
    Pin Led1 = gpio.pin(C5, OUTPUT);
    Pin Button = gpio.pin(E4, INPUT);

    while(true) {
        if(gpio.read(Button) == 0) {
            delay(1000);
            gpio.reverse(Led1);
        }
    }

    //gpio.low(Led1);
    //gpio.high(Led1);
}

