/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/*if (rx_string_ready == true)
	      {
	        // Stringa ricevuta! Abbassa subito il flag.
	        rx_string_ready = false;

	        // Processa il comando ricevuto
	        if (strcmp((char*)rx_buffer, "LED_ON") == 0) {
	        	GPIOA -> ODR |= (1<<5);
	            strcpy((char*)tx_buffer, "OK: LED acceso\r\n");
	        }
	        else if (strcmp((char*)rx_buffer, "LED_OFF") == 0) {
	        	GPIOA -> ODR &= ~(1<<5);
	            strcpy((char*)tx_buffer, "OK: LED spento\r\n");
	        }
	        else {
	            strcpy((char*)tx_buffer, "Errore: Comando non valido\r\n");
	        }

	        // Prepara e innesca la trasmissione della risposta
	        tx_index = 0;
	        USART3->CR1 |= USART_CR1_TXEIE; // Abilita l'interrupt di trasmissione
	      }*/
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
unsigned int numero_cicli;
unsigned int led_status = 0 ;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile unsigned int interruttore; //prendere tempo in maniera più razionale
volatile char dato;
#define RX_BUFFER_SIZE 64
 volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
 volatile uint16_t rx_index;
 volatile bool rx_string_ready;

#define TX_BUFFER_SIZE 64
volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
 volatile uint16_t tx_index;

#define N_CAMPIONI 100
int32_t vet_dif[N_CAMPIONI];
volatile uint16_t campione_da_inviare_idx = 0; // Indice per vet_dif
// --- Buffer e variabili GLOBALI per la comunicazione USART ---
// DEFINIZIONE (con inizializzazione)



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void CalcolaEPreparaDati(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void CalcolaEPreparaDati(void)
{
    float tau1 = 15.0f;
    float tau2 = 10.0f;
    float delta_t = 1.0f;
    int A_m = 60000;
    int scala = 10;
    int coeff = 1 << scala;

    uint16_t costante_1 = (uint16_t)(floor(exp(-delta_t / tau1) * coeff));
    uint16_t costante_2 = (uint16_t)(floor(exp(-delta_t / tau2) * coeff));

    uint16_t vet1[N_CAMPIONI];
    uint16_t vet2[N_CAMPIONI];

    vet1[0] = A_m;
    vet2[0] = A_m;

    for (int k = 1; k < N_CAMPIONI; k++) {
        uint32_t moltiplicazione1 = (uint32_t)vet1[k - 1] * costante_1;
        uint32_t moltiplicazione2 = (uint32_t)vet2[k - 1] * costante_2;
        vet1[k] = (uint16_t)(moltiplicazione1 >> scala);
        vet2[k] = (uint16_t)(moltiplicazione2 >> scala);
    }

    for (int i = 0; i < N_CAMPIONI; i++) {
        vet_dif[i] = (int32_t)vet1[i] - (int32_t)vet2[i];
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

//Queste sono variabili che useremo
//Abituiamoci ad inserire commenti
	unsigned int cc=2000;
	unsigned int attesa= 1;
    unsigned int il_clock_impostato,  ripetizioni;
    char led_acceso=1;



//Un indirizzo che possiamo usare per un attimo: 0x2407fff8



//Cominciamo ad abituarci ad evitare l'uso dei numeri, quando possibile. Usiamo invece definizioni o
//dichiarazioni di variabili e/o costanti
	#define interruttore_lampadina  GPIOB->BSRR
	#define accendi_lampadina (1<<0)
	#define spegni_lampadina (1<<(0+16))


//Useremo tra poco per misurare il tempo
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk ; //Abilitiamo il conta cicli macchina
//	DBGMCU->APB1LFZ1 |= DBGMCU_APB1LFZ1_DBG_TIM6_Msk; //Stoppiamo TIM6 al breakpoint
//	DWT->CYCCNT=0;  //Azzeriamo il contacicli macchina per comodità
//
	il_clock_impostato = HAL_RCC_GetSysClockFreq ()  ;
//	ripetizioni = il_clock_impostato / 12;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  /*accendiamo orologio e timer*/
  TIM6-> ARR=10000;
  TIM6 -> CR1 |= TIM_CR1_CEN; //counter enable nomefamiglia_cassettino_nomeflag
  //TIM6 -> CR1 = TIM6 -> CR1 || TIM6_CR1_CEN << OPERATORE di scorrimento quindi posso (1<<k)
  TIM6-> CNT= 0 ; //Accendiamo il timer primo di entrare nel ciclo while
  TIM6 -> DIER	|= TIM_DIER_UIE;
  TIM6 -> PSC = 500 ;
  USART3 -> CR1 |= USART_CR1_FIFOEN;
  USART3-> CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE ;

  USART3 -> CR1 |= USART_CR1_UE;
  const char* hello_message = "Hello from STM32!\n";
  CalcolaEPreparaDati();



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // Controlla se l'interrupt di ricezione ha segnalato una nuova stringa
	      /*if (rx_string_ready == true)
	      {
	        // Stringa ricevuta! Abbassa subito il flag.
	        rx_string_ready = false;

	        // Processa il comando ricevuto
	        if (strcmp((char*)rx_buffer, "LED_ON") == 0) {
	        	GPIOA -> ODR |= (1<<5);
	            strcpy((char*)tx_buffer, "OK: LED acceso\r\n");
	        }
	        else if (strcmp((char*)rx_buffer, "LED_OFF") == 0) {
	        	GPIOA -> ODR &= ~(1<<5);
	            strcpy((char*)tx_buffer, "OK: LED spento\r\n");
	        }
	        else {
	            strcpy((char*)tx_buffer, "Errore: Comando non valido\r\n");
	        }

	        // Prepara e innesca la trasmissione della risposta
	        tx_index = 0;
	        USART3->CR1 |= USART_CR1_TXEIE; // Abilita l'interrupt di trasmissione
	      }*/


	  if (rx_string_ready == true)
	      {
	        // Comando ricevuto! Abbassa subito il flag.
	        rx_string_ready = false;

	        // Processa il comando
	        if (strcmp((char*)rx_buffer, "s") == 0)
	        {
	          // Controlla se ci sono ancora campioni da inviare
	          if (campione_da_inviare_idx < N_CAMPIONI)
	          {
	            // Converti il numero corrente in una stringa di testo
	            // %ld è il formato corretto per un long int (int32_t)
	            sprintf((char*)tx_buffer, "%ld\r\n", vet_dif[campione_da_inviare_idx]);

	            // Avanza l'indice per la prossima richiesta
	            campione_da_inviare_idx++;
	          }
	          else
	          {
	            // Abbiamo finito di inviare tutti i campioni
	            strcpy((char*)tx_buffer, "Fine trasmissione.\r\n");
	            campione_da_inviare_idx = 0; // Resetta per permettere di ricominciare
	          }

	          // Innesca la trasmissione della stringa preparata
	          tx_index = 0;
	          USART3->CR1 |= USART_CR1_TXEIE;
	        }
	      }



	  /*if (rx_string_ready == true) {

		  rx_string_ready = false;

		  strcpy((char*)tx_buffer, (const char*)rx_buffer);
		  //strcat((char*)tx_buffer, "\r\n");

		  tx_index = 0;
		  USART3->CR1 |= USART_CR1_TXEIE;

	  }*/

	  /* 1. Copia il messaggio nel buffer di trasmissione.
	      strcpy((char*)tx_buffer, hello_message);

	      // 2. Resetta l'indice.
	      tx_index = 0;

	      // 3. INNESCA LA TRASMISSIONE abilitando l'interrupt TXE.
	      USART3->CR1 |= USART_CR1_TXEIE;

	      // 4. Aspetta 2 secondi.
	      HAL_Delay(2000);*/
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
