//*****************************************************************************
//
///		This is a UART driver for AVR-series processors with two hardware
///		UARTs such as the mega161 and mega128. This library provides both
///		buffered and unbuffered transmit and receive functions for the AVR
///		processor UART. Buffered access means that the UART can transmit
///		and receive data in the "background", while your code continues
///		executing.  Also included are functions to initialize the UARTs,
///		set the baud rate, flush the buffers, and check buffer status.
//
//*****************************************************************************

#ifndef UART2_H
#define UART2_H

#include "common.h"
#include "buffer.h"

//if the device only has one UART changed the definitions
#ifdef UCSR1B
	#define TWO_UART
#elif !(defined(USART0_TX_vect) || defined(USART0_RX_vect))
	#define USART0_TX_vect USART_TX_vect
	#define USART0_RX_vect USART_RX_vect
#endif

// compatibility for the mega161
#ifndef RXCIE
	#define RXCIE	RXCIE0
	#define TXCIE	TXCIE0
	#define UDRIE	UDRIE0
	#define RXEN	RXEN0
	#define TXEN	TXEN0
	#define CHR9	CHR90
	#define RXB8	RXB80
	#define TXB8	TXB80
#endif
#ifndef UBRR0L
	#define UBRR0L	UBRR0
	#define UBRR1L	UBRR1
#endif

// functions

//! Initializes UARTs.
/// \note	After running this init function, the processor
/// I/O pins that used for uart communications (RXD, TXD)
/// are no long available for general purpose I/O.
void uartInit(void);

//! Initializes UART0 only.
void uart0Init(void);

//! Initializes UART1 only.
void uart1Init(void);

//! Initializes transmit and receive buffers.
/// Automatically called from uartInit()
void uart0InitBuffers(void);
void uart1InitBuffers(void);

//! Sets the uart baud rate.
/// Argument should be in bits-per-second, like \c uartSetBaudRate(9600);
void uartSetBaudRate(uint8_t nUart, uint32_t baudrate);

//! Returns pointer to the receive buffer structure.
///
cBuffer* uartGetRxBuffer(uint8_t nUart);

//! Returns pointer to the transmit buffer structure.
///
cBuffer* uartGetTxBuffer(uint8_t nUart);

//! Sends a single byte over the uart.
///
void uartSendByte(uint8_t nUart, uint8_t data);

//! SendByte commands with the UART number hardcoded
/// Use these with printfInit() - example: \c printfInit(uart0SendByte);
void uart0SendByte(uint8_t data);
void uart1SendByte(uint8_t data);

//! Gets a single byte from the uart receive buffer.
/// Returns the byte, or -1 if no byte is available (getchar-style).
int uart0GetByte(void);
int uart1GetByte(void);

//! Gets a single byte from the uart receive buffer.
/// Function returns TRUE if data was available, FALSE if not.
/// Actual data is returned in variable pointed to by "data".
/// Example usage:
/// \code
/// char myReceivedByte;
/// uartReceiveByte(0, &myReceivedByte );
/// \endcode
uint8_t uartReceiveByte(uint8_t nUart, uint8_t* data);

//! Flushes (deletes) all data from receive buffer.
///
void uartFlushReceiveBuffer(uint8_t nUart);

//! Add byte to end of uart Tx buffer.
///
void uartAddToTxBuffer(uint8_t nUart, uint8_t data);

//! AddToTxBuffer commands with the UART number hardcoded
/// Use this with printfInit() - example: \c printfInit(uart0AddToTxBuffer);
void uart0AddToTxBuffer(uint8_t data);
void uart1AddToTxBuffer(uint8_t data);

//! Begins transmission of the transmit buffer under interrupt control.
///
void uartSendTxBuffer(uint8_t nUart);

//returns number of bytes in buffer
uint16_t getNumBytesReceived(uint8_t nUart);


#endif

