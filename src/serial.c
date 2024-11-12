#include <stm32f031x6.h>

void eputchar(char c) {
    while( (USART1->ISR & (1 << 6))==0); // wait for ongoing transmission to finish
    USART1->TDR = c;
}

char egetchar() {
    while( (USART1->ISR & (1 << 5))==0); // wait for character to arrive
    return (char)USART1->RDR;
}

void eputs(char *String) {
    while(*String) // keep printing until a NULL is found
    {
        eputchar(*String);
        String++;
    }
}

void printDecimal(uint32_t Value) {
    char DecimalString[11] = {0}; // a 32 bit value can go up to about 4 billion
    // That's 10 digits plus a NULL character 
    int index = 9;

    while (index >= 0 && Value) {
        DecimalString[index] = (Value % 10) + '0';
        Value = Value / 10;
        index--;
    }
    while(index >= 0) DecimalString[index--] = ' ';

    eputs(DecimalString);
}

void printHex(uint32_t Value) {
    char HexString[9] = {0}; // a 32 bit value can go up to 8 hex digits
    // That's 8 digits plus a NULL character 
    int index = 7;
    const char hexDigits[] = "0123456789ABCDEF";

    while (index >= 0 && Value) {
        HexString[index] = hexDigits[Value % 16];
        Value = Value / 16;
        index--;
    }
    while(index >= 0) HexString[index--] = '0';

    eputs(HexString);
}

void initClockHSI16() {
    // Use the HSI16 clock as the system clock - allows operation down to 1.5V
    RCC->CR &= ~(1 << 24);
    RCC->CR |= (1 << 0); // turn on HSI16 (16MHz clock)
    while ((RCC->CR & (1 << 2))==0); // wait for HSI to be ready
    // set HSI16 as system clock source 
    RCC->CFGR |= (1<<0);
}

void initSerial() {
    /* On the nucleo board, TX is on PA2 while RX is on PA15 */
	RCC->AHBENR |= (1 << 17); // enable GPIOA
	RCC->APB2ENR |= (1 << 14); // enable USART1
	pinMode(GPIOA,2,2); // enable alternate function on PA2
	pinMode(GPIOA,15,2); // enable alternate function on PA15
	// AF1 = USART1 TX on PA2
	GPIOA->AFR[0] &= 0xfffff0ff;
	GPIOA->AFR[0] |= (1 << 8);
	// AF1 = USART1 RX on PA15
	GPIOA->AFR[1] &= 0x0fffffff;
	GPIOA->AFR[1] |= (1 << 28);
	// De-assert reset of USART1 
	RCC->APB2RSTR &= ~(1u << 14);
	
	USART1->CR1 = 0; // disable before configuration
	USART1->CR3 |= (1 << 12); // disable overrun detection
	USART1->BRR = 48000000/9600; // assuming 48MHz clock and 9600 bps data rate
	USART1->CR1 |= (1 << 2) + (1 << 3); // enable Transmistter and receiver
	USART1->CR1 |= 1; // enable the UART
}
