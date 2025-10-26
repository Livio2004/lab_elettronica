/*
 * temperatura.c
 *
 *  Created on: Oct 22, 2025
 *      Author: oboss
 */

#define ADC_BUFFER_SIZE 100
#include "stm32h7xx_it.h"
#include "main.h"
#include "stdbool.h"
#include "temperatura.h"
#include <string.h>


extern volatile TransmissionMode tx_mode;


extern int16_t adc_vector[ADC_BUFFER_SIZE];


// 'volatile' dice al compilatore che queste variabili possono cambiare inaspettatamente
// (ad es. da un interrupt), quindi non deve ottimizzare via gli accessi.
volatile uint16_t adc_measurements[ADC_BUFFER_SIZE];
volatile uint32_t adc_measurement_index = 0;


void set_adc_3(){


	// --- FASE 1: USCITA DA DEEP POWER DOWN E ABILITAZIONE REGOLATORE ---


	// Disattiva la modalità "Deep Power Down" (spegnimento profondo) per l'ADC3.
	// È il primo passo per "risvegliare" il convertitore.
	ADC3->CR &=  ~(ADC_CR_DEEPPWD);

	// Abilita il regolatore di tensione interno dell'ADC (ADC Voltage Regulator).
	// Questo passaggio è obbligatorio dopo essere usciti dalla modalità Deep Power Down.
	ADC3->CR |= ADC_CR_ADVREGEN;


	// --- FASE 2: CALIBRAZIONE ADC ---


	// Seleziona la calibrazione per gli ingressi in modalità single-ended.
	// Lo fa mettendo a 0 il bit ADCALDIF (Differential Calibration).
	ADC3->CR &= ~(ADC_CR_ADCALDIF);

	// Abilita la calibrazione di linearità (Linearity Calibration).
	// Su alcuni microcontrollori, migliora la precisione della calibrazione principale.
	ADC3->CR |= ADC_CR_ADCALLIN;

	// Avvia la procedura di calibrazione dell'ADC (ADC Calibration).
	// L'hardware resetterà questo bit a 0 quando la calibrazione sarà completata.
	ADC3->CR |= ADC_CR_ADCAL;


	while ((ADC3->CR & ADC_CR_ADCAL) != 0) {
	    // Ciclo vuoto: l'unica azione è controllare la condizione.
	}


	// --- FASE 3: ABILITAZIONE ADC ---



	// Pulisce (resettando a 0) il flag "ADC Ready" (ADRDY) nel registro di stato (ISR).
	// I flag di stato si cancellano scrivendovi '1'.
	ADC3->ISR |= ADC_ISR_ADRDY;

	// Abilita l'ADC (ADC Enable). Da questo momento l'ADC è attivo.
	// L'hardware imposterà il flag ADRDY quando sarà pronto per le conversioni.
	ADC3->CR |= ADC_CR_ADEN;

	while (!(ADC3->ISR & ADC_ISR_ADRDY)) {
	    // Ciclo vuoto: attende che il flag diventi '1'.
	}

	// Pulisce di nuovo il flag "ADC Ready". Utile per evitare stati inattesi.
	ADC3->ISR |= ADC_ISR_ADRDY;

	// Imposta il tempo di campionamento per il canale 19.
	// Il canale 19 è gestito dal registro SMPR2.
	// I valori possibili sono da 0 a 7, dove 7 è il tempo più lungo.
	// Questo dà al condensatore interno tutto il tempo di caricarsi correttamente.

	ADC3->SMPR2 &= ~(ADC_SMPR2_SMP19); // Pulisci i bit esistenti
	ADC3->SMPR2 |= (6 << ADC_SMPR2_SMP19_Pos); // Imposta il nuovo valore (es. 6)


	// --- FASE 4: CONFIGURAZIONE INTERRUPT ---


	// Abilita l'interrupt di "End of Conversion" (Fine della Conversione).
	// Ogni volta che una conversione ADC termina, verrà generata una richiesta di interrupt.
	//ADC3->IER |= ADC_IER_EOCIE; NON UTILIZZIAMO


	ADC3->SQR1=0;
	ADC3->SQR1 |= (19<<ADC_SQR1_SQ1_Pos);
	ADC3->SQR1 |= (1<< ADC_SQR1_L_Pos);
	ADC3->PCSEL |= ADC_PCSEL_PCSEL_19 ; //indichi quali canali stai leggendo



}



void read_adc(){



	if (rx_string_ready == true){
		 		rx_string_ready= false;
		 		if (strcmp((char*)rx_buffer, "e") == 0){
		 			for (int i =0; i<ADC_BUFFER_SIZE;i++){
		 				ADC3->CR |= ADC_CR_ADSTART;

		 				while(!(ADC3->ISR & ADC_ISR_EOC)){

		 				}
		 				adc_vector[i]= ADC3->DR;

		 				ADC3->ISR |= ADC_ISR_EOC;

		 				}
		 			tx_mode = TX_MODE_BINARY;
		 			tx_binary_ptr = (volatile uint8_t*)adc_vector;
		 			tx_bytes_remaining = ADC_BUFFER_SIZE * sizeof(int16_t);

		 			// Innesca la trasmissione
		 			USART3->CR1 |= USART_CR1_TXEIE;
		 		}
		 	}






}

