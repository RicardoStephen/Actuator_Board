#include "common.h"

#include "buffer.h"
#include "uart.h"


// UART global variables
// receive and transmit buffers
cBuffer uartRxBuffer[2];
cBuffer uartTxBuffer[2];
unsigned short uartRxOverflow[2];
// using internal ram,
// automatically allocate space in ram for each buffer
static char uart0RxData[UART0_RX_BUFFER_SIZE+1];
static char uart0TxData[UART0_TX_BUFFER_SIZE+1];
#ifdef TWO_UART
  static char uart1RxData[UART1_RX_BUFFER_SIZE+1];
  static char uart1TxData[UART1_TX_BUFFER_SIZE+1];
#endif


void uartInit(void)
{
	// initialize both uarts
	uart0Init();
	#ifdef TWO_UART
		uart1Init();
	#endif
}

void uart0Init(void)
{
	// initialize the buffers
	uart0InitBuffers();
	//set speed doubler
	UCSR0A = (1<<U2X0);
	// enable RxD/TxD and interrupts
	UCSR0B= _BV(RXCIE)|_BV(TXCIE)|_BV(RXEN)|_BV(TXEN);
	// set default baud rate
	uartSetBaudRate(0, UART0_DEFAULT_BAUD_RATE); 
	// clear overflow count
	uartRxOverflow[0] = 0;
	// enable interrupts
	sei();
}

#ifdef TWO_UART


void uart1Init(void)
{
	// initialize the buffers
	uart1InitBuffers();
	//set speed doubler
	UCSR1A = (1<<U2X1);
	// enable RxD/TxD and interrupts
	UCSR1B=_BV(RXCIE)|_BV(TXCIE)|_BV(RXEN)|_BV(TXEN);
	// set default baud rate
	uartSetBaudRate(1, UART1_DEFAULT_BAUD_RATE);
	// clear overflow count
	uartRxOverflow[1] = 0;
	// enable interrupts
	sei();
}
#endif

void uart0InitBuffers(void)
{
  // initialize the UART0 buffers
  bufferInit(&uartRxBuffer[0], (uint8_t*)uart0RxData, UART0_RX_BUFFER_SIZE);
  bufferInit(&uartTxBuffer[0], (uint8_t*)uart0TxData, UART0_TX_BUFFER_SIZE);
}

#ifdef TWO_UART

void uart1InitBuffers(void)
{
  // initialize the UART1 buffers
  bufferInit(&uartRxBuffer[1], (uint8_t*)uart1RxData, UART1_RX_BUFFER_SIZE);
  bufferInit(&uartTxBuffer[1], (uint8_t*)uart1TxData, UART1_TX_BUFFER_SIZE);
	
}
#endif

void uartSetBaudRate(uint8_t nUart, uint32_t baudrate)
{
	// calculate division factor for requested baud rate, and set it
	uint16_t bauddiv = (uint16_t)(((float)F_CPU)/((float)baudrate*8L)-1+.5);
	if(nUart)
	{
	
		#ifdef TWO_UART
			UBRR1= bauddiv;
		#endif
	}
	else
	{
		UBRR0= bauddiv;
	}
}

cBuffer* uartGetRxBuffer(uint8_t nUart)
{
	// return rx buffer pointer
	return &uartRxBuffer[nUart];
}

cBuffer* uartGetTxBuffer(uint8_t nUart)
{
	// return tx buffer pointer
	return &uartTxBuffer[nUart];
}

void uartSendByte(uint8_t nUart, uint8_t txData)
{
	// wait for the transmitter to be ready
	// send byte
	if(nUart)
	{
		#ifdef TWO_UART
			while(!(UCSR1A & (1<<UDRE1)));
			UDR1=txData;
		#endif
	}
	else
	{
		while(!(UCSR0A & (1<<UDRE0)));
		UDR0= txData;
	}
}

void uart0SendByte(uint8_t data)
{
	// send byte on UART0
	uartSendByte(0, data);
}

void uart1SendByte(uint8_t data)
{
	// send byte on UART1
	uartSendByte(1, data);
}

int uart0GetByte(void)
{
	// get single byte from receive buffer (if available)
	uint8_t c;
	if(uartReceiveByte(0,&c))
		return c;
	else
		return -1;
}

int uart1GetByte(void)
{
	// get single byte from receive buffer (if available)
	uint8_t c;
	if(uartReceiveByte(1,&c))
		return c;
	else
		return -1;
}


uint16_t getNumBytesReceived(uint8_t nUart){
	return uartRxBuffer[nUart].datalength;
}

uint8_t uartReceiveByte(uint8_t nUart, uint8_t* rxData)
{
	// get byte from beginning of buffer
	return bufferGetFromFront(&uartRxBuffer[nUart],rxData);
}

void uartFlushReceiveBuffer(uint8_t nUart)
{
	// flush all data from receive buffer
	bufferFlush(&uartRxBuffer[nUart]);
}


void uartAddToTxBuffer(uint8_t nUart, uint8_t data)
{
	// add data byte to the end of the tx buffer
	bufferAddToEnd(&uartTxBuffer[nUart], data);
}

void uart0AddToTxBuffer(uint8_t data)
{
	uartAddToTxBuffer(0,data);
}

void uart1AddToTxBuffer(uint8_t data)
{
	uartAddToTxBuffer(1,data);
}

void uartSendTxBuffer(uint8_t nUart)
{
	uint8_t start;
	//if uart isn't busy send a byte from FIFO buffer
	//else it is already sending so no need to do anything
	if(nUart)
	{
		#ifdef TWO_UART
			if((UCSR1A & (1<<UDRE1))&&bufferGetFromFront(&uartTxBuffer[nUart],&start)){
				UDR1=start;
			}
		#endif
	}
	else
	{
		if((UCSR0A & (1<<UDRE0))&&bufferGetFromFront(&uartTxBuffer[nUart],&start)){
				UDR0=start;
		}
	}
}


ISR(USART0_TX_vect)      
{
	uint8_t data;
	// send byte from top of buffer if any are available
	if(bufferGetFromFront(&uartTxBuffer[0],&data)){
		UDR0=data;
	}
}



ISR(USART0_RX_vect)      
{
	uint8_t c = UDR0;
		
	// put received char in buffer
	// check if there's space
	if(!bufferAddToEnd(&uartRxBuffer[0], c) )
	{
		// no space in buffer
		// count overflow
		uartRxOverflow[0]++;
	}
}

#ifdef TWO_UART

	ISR(USART1_TX_vect)      
	{
		uint8_t data;
		// send byte from top of buffer
		if(bufferGetFromFront(&uartTxBuffer[1],&data)){
			UDR1=data;
		}
	}

	ISR(USART1_RX_vect)      
	{
		uint8_t c = UDR1;
		
		// otherwise do default processing
		// put received char in buffer
		// check if there's space
		if(!bufferAddToEnd(&uartRxBuffer[1], c) )
		{
			// no space in buffer
			// count overflow
			uartRxOverflow[1]++;
		}
	}
#endif


