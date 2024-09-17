/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "uart.h"

void LED_Init(uint32_t pin, GPIO_TypeDef* port)
{
    if (port == GPIOA)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    }
    else if (port == GPIOB)
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    }

    port->MODER &= ~(0x3 << (pin * 2)); // Clear mode bits
    port->MODER |= (0x1 << (pin * 2));  // Set as output
}

void LED_On(uint32_t pin, GPIO_TypeDef* port)
{
    port->ODR |= (1 << pin);  // Set pin high to turn on LED
}

void LED_Off(uint32_t pin, GPIO_TypeDef* port)
{
    port->ODR &= ~(1 << pin); // Set pin low to turn off LED
}

void set_led_on(int led_id, int duration)
{

	if(led_id==0)
	{
		 LED_On(led0_pin, LED0_PORT);
		 led0_timer_init(duration);
	}

	if(led_id==1)
	{
		 LED_On(led1_pin, LED1_PORT);
		 led1_timer_init(duration);
	}

}


void process_command(char* buffer, size_t buffer_size)
{
	char *data_start;
	const char *resp="data: ";

	if((buffer[0]=='e') && (buffer[1]=='c') && (buffer[2]=='h') && (buffer[3]=='o') && (buffer[4]==' '))
	{
		int data_count=strtol(static_cast<char*>(&buffer[5]),&data_start,10);

		if(data_count> MAX_DATA_LEN)
		{
			 Uart::ACK_ERROR();
			  return;
		}

		data_start++;

		Uart::write(resp,strlen(resp));
		Uart::write(data_start,data_count);
		Uart::write_byte('\r');
		Uart::write_byte('\n');

		Uart::ACK_OK();
		return;
	}

	if((buffer[0]=='s') && (buffer[1]=='e') && (buffer[2]=='t') && (buffer[3]=='-') && (buffer[4]=='l') && (buffer[5]=='e') && (buffer[6]=='d') && (buffer[7]==' '))
	{
		int led=strtol(static_cast<char*>(&buffer[8]),&data_start,10);
		data_start++;
		int duration=strtol(data_start,NULL,10);

		if(((duration>=MIN_DURATION) && (duration<MAX_DURATION)) && ((led==0) ||(led==1)))
		{
			set_led_on(led,duration);
			Uart::ACK_OK();
		}
		else
		{
			 Uart::ACK_ERROR();
		}

		return;
	}

	 Uart::ACK_ERROR();

}


bool read_command_from_uart(char* buffer, size_t buffer_size)
{
	bool raw_data=false;
	char end_ch='\r';
	int data_count=0;
	size_t pos = 0;

	while (1)
	{
		char received_char=Uart::read_byte();

	    if(raw_data)
	    	data_count--;

	    if(received_char == end_ch)
	    {
	    	if(end_ch==',')
	        {
	    		data_count=std::strtol(static_cast<char*>(&buffer[5]),NULL,10);

	    		if((data_count> MAX_DATA_LEN) || (data_count< MIN_DATA_LEN))
	    		{
	    			Uart::ACK_ERROR();
				memset(buffer,0,buffer_size);
	    			return false;
	    		}
 		
	    		data_count+=1;
	    		end_ch='\r';
	    		raw_data=true;
	         }
	    		else
	         {
	            if(data_count==0)
	            {
	            	if(pos < buffer_size)
	            	{
	            		buffer[pos] = received_char;
	            		return true;
	            	}
	            	else
	            	{
				memset(buffer,0,buffer_size);
	            		return false;
	            	}
	            }

	            if(data_count<0)
	            {
	            	Uart::ACK_ERROR(); //if echo <data> > <len>:
			memset(buffer,0,buffer_size);    
	            	return false;
	            }

	        }
	     }

	    if(pos < buffer_size - 1)
	    	buffer[pos++] = received_char;
	     else
	    {
		memset(buffer,0,buffer_size);    
	        return false;

	    }

	     if(pos==5)
	     {
	    	 if((buffer[0]=='e') && (buffer[1]=='c') && (buffer[2]=='h') && (buffer[3]=='o') && (buffer[4]==' ')) //echo command detected
	    		 end_ch=',';
	     }

	   }
    return false; // should never happen
}

int main()
{

	led0_pin = LED0_PIN_0;
	led1_pin = LED1_PIN_0;

	LED_Init(led0_pin, LED0_PORT);
	LED_Init(led1_pin, LED1_PORT);

    uint32_t baudrate = BAUDRATE;

    Uart::init(baudrate);

    const int BUFF_SIZE=320;
    char command_buffer[BUFF_SIZE]={0};

    while (1)
    {
        if (read_command_from_uart(command_buffer, BUFF_SIZE))
        {
        	process_command(command_buffer,BUFF_SIZE);
        	memset(command_buffer,0,BUFF_SIZE);
        }
    }
    return 0;
}


void led0_timer_init(uint32_t milliseconds)
{

    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    uint32_t timer_clock = TM_CLOCK;
    uint32_t prescaler = (timer_clock / 1000000) - 1;
    TIM2->PSC = prescaler;

    uint32_t autoreload_value = (1000 * milliseconds) - 1;
    TIM2->ARR = autoreload_value;

    TIM2->CNT=0;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 &= ~TIM_CR1_DIR;

    TIM2->CR1 |= TIM_CR1_CEN|TIM_CR1_OPM;
}

extern "C" void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        TIM2->CR1 &= ~TIM_CR1_CEN;
        LED_Off(led0_pin, LED0_PORT);
        Uart::LED_OFF_ACK(0);//
    }
}

void led1_timer_init(uint32_t milliseconds)
{

    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;

    uint32_t timer_clock = TM_CLOCK;
    uint32_t prescaler = (timer_clock / 1000000) - 1;
    TIM6->PSC = prescaler;

    uint32_t autoreload_value = (1000 * milliseconds) - 1;
    TIM6->ARR = autoreload_value;

    TIM6->CNT=0;
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_IRQn);

    TIM6->CR1 &= ~TIM_CR1_DIR;
    TIM6->CR1 |= TIM_CR1_CEN |TIM_CR1_OPM;
}

extern "C" void TIM6_IRQHandler(void)
{
    if (TIM6->SR & TIM_SR_UIF)
    {
        TIM6->SR &= ~TIM_SR_UIF;
        TIM6->CR1 &= ~TIM_CR1_CEN;
        LED_Off(led1_pin, LED1_PORT);
        Uart::LED_OFF_ACK(1);
    }
}
