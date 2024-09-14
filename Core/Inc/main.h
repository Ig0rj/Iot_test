/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#include <cstdlib>
#include <cstdint>
#include <cstring>

#define MAX_DATA_LEN 300
#define MIN_DATA_LEN 0

#define MIN_DURATION 1
#define MAX_DURATION 5000
#define TM_CLOCK 80000000
#define BAUDRATE 9600

#define LED0_PORT GPIOA
#define LED1_PORT GPIOB

#define LED0_PIN_0 5   // Example using Pin 5 of GPIOA
#define LED0_PIN_1 6   // Example using Pin 6 of GPIOA
#define LED0_PIN_2 7   // Example using Pin 7 of GPIOA
#define LED0_PIN_3 8   // Example using Pin 8 of GPIOA

#define LED1_PIN_0 0   // Example using Pin 0 of GPIOB
#define LED1_PIN_1 1   // Example using Pin 1 of GPIOB
#define LED1_PIN_2 2   // Example using Pin 2 of GPIOB
#define LED1_PIN_3 3   //

volatile uint32_t led0_pin, led1_pin;  // Configure as needed
void led0_timer_init(uint32_t milliseconds);
void led1_timer_init(uint32_t milliseconds);

#endif /* __MAIN_H */
