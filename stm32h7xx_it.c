/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h> // Per usare strcpy e strcmp
#include <stdbool.h> // Per usare true/false per i flag
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define interruttore_lampadina  GPIOB->BSRR
#define accendi_lampadina (1<<0)
#define spegni_lampadina (1<<(0+16))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
#define RX_BUFFER_SIZE 64
extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
extern volatile uint16_t rx_index;
extern volatile bool rx_string_ready;

// Dichiarazione del tipo 'enum'
typedef enum {
    TX_MODE_IDLE,
    TX_MODE_STRING,
    TX_MODE_BINARY
} TransmissionMode;

extern volatile TransmissionMode tx_mode;

#define TX_BUFFER_SIZE 64
extern volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
extern volatile uint16_t tx_char_index;

extern volatile uint8_t* tx_binary_ptr;
extern volatile uint16_t tx_bytes_remaining;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc3;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
	if ((USART3->ISR & USART_ISR_RXNE_RXFNE) != 0 && (USART3->CR1 & USART_CR1_RXNEIE) != 0)
		  {
		    char dato_ricevuto = USART3->RDR;
		    if (dato_ricevuto == '\n' || dato_ricevuto == '\r') {
		      if (rx_index > 0) {
		        rx_buffer[rx_index] = '\0';
		        rx_string_ready = true;
		      }
		      rx_index = 0;
		    } else {
		      if (rx_index < RX_BUFFER_SIZE - 1) {
		        rx_buffer[rx_index] = dato_ricevuto;
		        rx_index++;
		      }
		    }
		  }

		  /* --- GESTIONE DELLA TRASMISSIONE (CON MODALITÀ) --- */
		  if ((USART3->ISR & USART_ISR_TXE_TXFNF) != 0 && (USART3->CR1 & USART_CR1_TXEIE) != 0)
		  {
		    // Controlla in quale modalità siamo
		    if (tx_mode == TX_MODE_STRING)
		    {
		      if (tx_buffer[tx_char_index] != '\0') {
		        USART3->TDR = tx_buffer[tx_char_index];
		        tx_char_index++;
		      } else {
		        // Stringa finita, torna in IDLE e spegni l'interrupt
		        tx_mode = TX_MODE_IDLE;
		        USART3->CR1 &= ~USART_CR1_TXEIE;
		      }


		      //binary
		    }
		    else if (tx_mode == TX_MODE_BINARY)
		    {
		      if (tx_bytes_remaining > 0) {
		        USART3->TDR = *tx_binary_ptr;
		        tx_binary_ptr++;
		        tx_bytes_remaining--;
		      }

		      if (tx_bytes_remaining == 0) {
		        // Dati finiti, torna in IDLE e spegni l'interrupt
		        tx_mode = TX_MODE_IDLE;
		        USART3->CR1 &= ~USART_CR1_TXEIE;
		      }
		    }
		  }




  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1_CH1 and DAC1_CH2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
	if (GPIOB ->ODR & 1){
			  interruttore_lampadina = spegni_lampadina;
			  GPIOE -> ODR |= (1<<1);
			  GPIOB -> ODR &= ~(1<<14);
	    	  TIM6->SR =0;
	      } else {
	    	  interruttore_lampadina = accendi_lampadina;
	    	  GPIOE -> ODR &= ~(1<<1);
	    	  GPIOB -> ODR |= (1<<14);
	    	  TIM6 -> SR = 0 ;
	      }

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles ADC3 global interrupt.
  */
void ADC3_IRQHandler(void)
{
  /* USER CODE BEGIN ADC3_IRQn 0 */
  /* USER CODE END ADC3_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc3);
  /* USER CODE BEGIN ADC3_IRQn 1 */

  /* USER CODE END ADC3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
