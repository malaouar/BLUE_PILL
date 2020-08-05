#ifndef UART
#define UART

void uart_init(void);
char uart_getc(void);
void uart_putc(char c);
void hexstring(unsigned int d);
void uart_putstr(char *str);

#endif 