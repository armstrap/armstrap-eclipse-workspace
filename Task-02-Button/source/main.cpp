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

	// Initialize the GPIO Pins below...

    // For the ARMstrap Eagle board, the USER Button is hooked-up to GPIOC Pin 0.
    // You can see this by looking at the freely available schematics on armstrap.org
    GPIO_InitTypeDef buttonGPIO;
	GPIO_StructInit(&buttonGPIO);
	buttonGPIO.GPIO_Mode = GPIO_Mode_IN;
	buttonGPIO.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOC, &buttonGPIO);

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
    uint32_t counter = 0;

    while (1) {
        counter++;

       	bool isButtonPressed = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) != 0;

       	// If the user button is pressed, then light the user LED on the board.
		if (isButtonPressed)
			GPIO_SetBits(GPIOC, GPIO_Pin_1);
		else
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
    };
}

void delay(uint32_t ms)
{
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}

