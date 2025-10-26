/*
 * temperatura.h
 *
 *  Created on: Oct 22, 2025
 *      Author: oboss
 */

#ifndef INC_TEMPERATURA_H_
#define INC_TEMPERATURA_H_

#define RX_BUFFER_SIZE 64
extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
extern volatile uint16_t rx_index;
extern volatile bool rx_string_ready;

typedef enum {
    TX_MODE_IDLE,
    TX_MODE_STRING,
    TX_MODE_BINARY
} TransmissionMode;


// Dichiarazione del tipo 'enum'


#define TX_BUFFER_SIZE 64
extern volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
extern volatile uint16_t tx_char_index;

extern volatile uint8_t* tx_binary_ptr;
extern volatile uint16_t tx_bytes_remaining;

void set_adc_3();
void read_adc();

#endif /* INC_TEMPERATURA_H_ */
