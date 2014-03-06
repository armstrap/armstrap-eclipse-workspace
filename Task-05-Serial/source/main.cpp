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
#include "stm32f4xx_usart.h"

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
	// enable the peripheral clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	// This sequence sets up the TX and RX GPIO pins so they work correctly
	// with the USART peripheral
	GPIO_InitTypeDef usartGPIO;
	usartGPIO.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; // Pin 5 (TX), Pin 6 (RX)
	usartGPIO.GPIO_Mode = GPIO_Mode_AF;
	usartGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	usartGPIO.GPIO_OType = GPIO_OType_PP;
	usartGPIO.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &usartGPIO);

	// The RX and TX pins are now connected to their AF
	// so that the USART1 can take over control of the pins
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

	// Define the properties of USART
	USART_InitTypeDef consoleUSART;
	consoleUSART.USART_BaudRate = 9600;
	consoleUSART.USART_WordLength = USART_WordLength_8b;
	consoleUSART.USART_StopBits = USART_StopBits_1;
	consoleUSART.USART_Parity = USART_Parity_No;
	consoleUSART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	consoleUSART.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &consoleUSART);

	// finally enable the USART peripheral
	USART_Cmd(USART2, ENABLE);
}

void loop()
{
    uint32_t counter = 0;

    while (1)
    {
        counter++;
    	size_t amountCopied = 0;
    	const size_t maxCommandSize = 32;
    	char command[maxCommandSize];

    	usart_print(USART2, "\nARMstrap-Eagle:~# ");

    	while(amountCopied != maxCommandSize - 1) // -1 because null character is always appended to 'command'
    	{
    		char c = usart_getchar(USART2);
    		if (c == '\r' || c == '\n')
    			break;

    		if (c >= 32 && c <= 126) // commit only printable ASCII characters to the buffer
    		{
    			command[amountCopied++] = c;
    			usart_putchar(USART2, c); // echo back to console to I can see what I type
    		}
    		else if (c == 127 && amountCopied > 0) // DEL removes an item from our buffer
    		{
    			amountCopied--;
    			usart_putchar(USART2, c); // echo back to console to I can see the deletion
    		}
    	}
    	command[amountCopied] = '\0';

    	// Only process the command if a command actually exists
    	if (amountCopied > 0)
    		process_command(command);
    }
}

void process_command(const char* command)
{
	// Put your processing code here.  All we do here is echo the command
	// back to the user.
	usart_print(USART2, "\nYou entered: ");
	usart_puts(USART2, command);
}

void usart_putchar(USART_TypeDef* USARTx, const char c)
{
	// wait until data register is empty
	while( !(USARTx->SR & USART_FLAG_TC) );
	USART_SendData(USARTx, c);
}

void usart_print(USART_TypeDef* USARTx, const char* s)
{
	while(*s){
		if (*s == '\n') {
			usart_putchar(USARTx, '\r');
		}
		usart_putchar(USARTx, *s);
		s++;
	}
}

void usart_puts(USART_TypeDef* USARTx, const char* s)
{
	usart_print(USARTx, s);
	usart_putchar(USARTx, '\r');
	usart_putchar(USARTx, '\n');
}

uint16_t usart_getchar(USART_TypeDef* USARTx)
{
	while( !(USARTx->SR & USART_FLAG_RXNE) );
	return USART_ReceiveData(USARTx);
}

size_t usart_gets(USART_TypeDef* USARTx, char* s, size_t size)
{
	size_t amountCopied = 0;
	while(amountCopied != size - 1) // -1 because null character is always appended
	{
		*s = usart_getchar(USARTx);
		if (*s == '\r' || *s == '\n') {
			break;
		}
		amountCopied++;
		s++;
	}
	s[amountCopied] = '\0';
	return amountCopied;
}
