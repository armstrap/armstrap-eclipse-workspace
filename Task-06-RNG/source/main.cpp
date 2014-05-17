/*
The MIT License (MIT)

Copyright (c) 2014 ARMstrap Community

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "main.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rng.h"

int main()
{
    setup();
    do
    {
        loop();
    }
    while (1);

    return 0; // never reached
}

void setup()
{
	// On startup, all peripheral clocks are disabled.  Before using a GPIO
	// pin, its peripheral clock must be enabled.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // Initialize RNG
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_ITConfig(ENABLE);
	RNG_Cmd(ENABLE);

	// Initialize the GPIO Pins below...

    // For the ARMstrap Eagle board, the USER LED is hooked-up to GPIOC Pin 1.
    // You can see this by looking at the freely available schematics on armstrap.org
    GPIO_InitTypeDef ledGPIO;
    GPIO_StructInit(&ledGPIO);
    ledGPIO.GPIO_Mode = GPIO_Mode_OUT;
    ledGPIO.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &ledGPIO);
}

void loop()
{

    while (1) {

        GPIO_ToggleBits(GPIOC, GPIO_Pin_1);

        // random delay
		while (!RNG_GetFlagStatus(RNG_FLAG_DRDY))
			__NOP();
		delay(RNG_GetRandomNumber() % 1000);
    };
}

void delay(uint32_t ms)
{
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}

