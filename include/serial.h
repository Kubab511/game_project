#include <stdint.h>

void delay(volatile uint32_t dly);
void eputchar(char c);
char egetchar(void);
void eputs(char *String); 
void printDecimal(uint32_t Value);
void initSerial(void);