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
#include "stm32f4xx_tim.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_exti.h"
#include "misc.h"

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
	initialize_led();
    initialize_button();
    initialize_ext_interrupt();
}

void loop()
{
	// Nothing to do here because application is interrup based.
	__NOP();
}

void initialize_led()
{
	// On startup, all peripheral clocks are disabled.  Before using a GPIO
	// pin, its peripheral clock must be enabled.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // For the ARMstrap Eagle board, the USER LED is hooked-up to GPIOC Pin 1.
    // You can see this by looking at the freely available schematics on armstrap.org
    GPIO_InitTypeDef ledGPIO;
    GPIO_StructInit(&ledGPIO);
    ledGPIO.GPIO_Mode = GPIO_Mode_OUT;
    ledGPIO.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &ledGPIO);
}

void initialize_button()
{
	// On startup, all peripheral clocks are disabled.  Before using a GPIO
	// pin, its peripheral clock must be enabled.
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // For the ARMstrap Eagle board, the USER Button is hooked-up to GPIOC Pin 0.
    // You can see this by looking at the freely available schematics on armstrap.org
    GPIO_InitTypeDef buttonGPIO;
	GPIO_StructInit(&buttonGPIO);
	buttonGPIO.GPIO_Mode = GPIO_Mode_IN;
	buttonGPIO.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOC, &buttonGPIO);
}

void initialize_ext_interrupt()
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource0);

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void delay(uint32_t ms)
{
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}

extern "C" void EXTI0_IRQHandler()
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_1);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

