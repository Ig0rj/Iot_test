/*
 *#include "main.h" uart.cpp
 *
 *  Created on: Sep 14, 2024
 *      Author: Igor
 */

#include "uart.h"

void Uart::init(int baudrate)
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Set PA2 as USART2_TX, PA3 as USART2_RX
    GPIOA->MODER |= (2 << GPIO_MODER_MODE2_Pos) | (2 << GPIO_MODER_MODE3_Pos);
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos) | (7 << GPIO_AFRL_AFSEL3_Pos);

    // Configure USART parameters: 8 data bits, no parity, 1 stop bit
    USART2->BRR = SystemCoreClock / baudrate;
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

inline void Uart::write_byte(const char c)
 {
	 while (!(USART2->ISR & USART_ISR_TXE));
	 USART2->TDR = c;
 }

inline void Uart::write(const char *data, size_t length)
 {
	 while(length--)
	 {
		 while (!(USART2->ISR & USART_ISR_TXE));
		 USART2->TDR = *data++;

	 }
 }

 char Uart::read_byte(void)
 {
	 while(!(USART2->ISR & USART_ISR_RXNE));

	 return static_cast<char>(USART2->RDR & 0xFF);
 }

inline void Uart::read(char *data, size_t lenght)
 {
	 size_t count=0;

	 while(count<lenght)
	 {
		 while(!(USART2->ISR & USART_ISR_RXNE))
			 data[count++]=static_cast<char>(USART2->RDR & 0xFF);
	 }
 }

 void Uart::ACK_OK(void)
 {
	 Uart::write("OK\r\n",4);
 }

 void Uart::ACK_ERROR(void)
 {
	 Uart::write("ERROR\r\n",7);
 }

 void Uart::LED_OFF_ACK(int led_id)
 {
	 Uart::write("led-off: ",9);
	 Uart::write_byte(static_cast<char>(led_id)+48); //digit to ascii digit
	 Uart::write_byte('\r');
	 Uart::write_byte('\n');

 }
