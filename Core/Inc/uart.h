#ifndef __UART_H
#define __UART_H


#include "stm32l4xx.h"
#include <cstdio>

class Uart
{
public:

	static void init(int baudrate);
	static void write_byte(const char c);
	static void write(const char *data, size_t length);
	static char read_byte(void);
	static void read(char *data, size_t lenght);

	static void ACK_OK(void);
	static void ACK_ERROR(void);
	static void LED_OFF_ACK(int led_id);

private:
    Uart();

};

#endif /* __MAIN_H */
