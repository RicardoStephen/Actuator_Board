#include "common.h"


uint8_t function;	///This must be from the list of supported functions
uint8_t count;		///The number of registers being accessed at once
Address address; 	///The address of the first register in this access. 
uint8_t received;	///bytes received
uint8_t checkSum;	///packet checksum
uint8_t badPacket;	///has the current packet failed

uint8_t polled;

uint16_t hostTimeout;
uint16_t hostTime;

//this stores the data for the registers available to the computer
uint16_t commRegu16[CUSTOM_REG_NUM+DEFUALT_REG_NUM];
//a different pointer to the data for the serial registers
uint8_t *commRegu8=(uint8_t*)commRegu16;


//buffer for data from unfinished incoming packet
static uint8_t dataBuff[CUSTOM_REG_NUM+DEFUALT_REG_NUM];
//indexes of read only registers
#if (NUM_READ_ONLY>0)
	static uint16_t readOnly[NUM_READ_ONLY+DEFUALT_READ_ONLY_NUM]={READ_ONLY,DEFUALT_READ_ONLY_ADDR};
#else
	static uint16_t readOnly[DEFUALT_READ_ONLY_NUM]={DEFUALT_READ_ONLY_ADDR};
#endif

static uint8_t varStarts[]={VAR_STARTS};

//Used to find variable lengths for endianess correction
#define IS_START(U) READ(varStarts[U>>3],(U&0b111))


//hase the current packet timed out
volatile uint8_t timedOut;

uint8_t isPolled(){
	uint8_t temp=polled;
	polled=0;
	return temp;
}

void packetHandlerInit(){
	//init variables
	received=0;
	timedOut=0;
	badPacket=0;
	//turn on watchdog to first interupt to mark a timeout
	//if handler isn't called for two timeouts then reset chip
	WD_SET(WD_RST_IRQ,SERIAL_TIMEOUT);
	//initialize default registes
	commRegu16[0]=DEVICE_ID;
	commRegu16[1]=PROTOCOL_NUM;
	//	*((uint32_t*)&commRegu16[2])=BUILD_DATE;
	((uint32_t*)commRegu16)[1]=BUILD_DATE; // ijt5's hack for -Werror strickt aliasing complain
	commRegu16[6]=MCUSR;
}

void sendPacket(uint8_t func,uint8_t count,Address addr){
	//put header in Tx buffer while calculating checksum
	uint8_t sum=0;
	uartAddToTxBuffer(USART_PRIMARY, func);
	sum+=func;
	uartAddToTxBuffer(USART_PRIMARY, count);
	sum+=count;
	uartAddToTxBuffer(USART_PRIMARY, addr.u1[1]);
	sum+=addr.u1[0];
	uartAddToTxBuffer(USART_PRIMARY, addr.u1[0]);
	sum+=addr.u1[1];
	
	//put payload in buffer in big endian
	uint16_t curByte;
	uint16_t nextByte=(addr.u2<<1);
	uint16_t lastByte=(addr.u2+count)<<1;
	while (nextByte<lastByte){
		curByte=nextByte;
		nextByte++;
		uint8_t data;
		if(nextByte==lastByte||IS_START(nextByte)){
			uint8_t data=commRegu8[curByte];
			sum+=data;
			uartAddToTxBuffer(USART_PRIMARY,data);
			continue;
		}
		nextByte++;
		if(nextByte==lastByte||IS_START(nextByte)){
			data=commRegu8[curByte+1];
			sum+=data;
			uartAddToTxBuffer(USART_PRIMARY,data);
			data=commRegu8[curByte];
			sum+=data;
			uartAddToTxBuffer(USART_PRIMARY,data);
			continue;
		}
		nextByte+=2;
		data=commRegu8[curByte+3];
		sum+=data;
		uartAddToTxBuffer(USART_PRIMARY,data);
		data=commRegu8[curByte+2];
		sum+=data;
		uartAddToTxBuffer(USART_PRIMARY,data);
		data=commRegu8[curByte+1];
		sum+=data;
		uartAddToTxBuffer(USART_PRIMARY,data);
		data=commRegu8[curByte];
		sum+=data;
		uartAddToTxBuffer(USART_PRIMARY,data);
	}
	//add checksum to Tx buffer 
	uartAddToTxBuffer(USART_PRIMARY, sum);
	//start transmitting packet
	uartSendTxBuffer(USART_PRIMARY);
}


uint8_t calcCheck(uint8_t*data,uint16_t len){
	uint8_t sum=0;
	for(uint16_t i=0;i<len;i++){
		sum+=data[i];
	}
	return sum;
}


void handleSerialRx(){
	uint8_t inByte;
	//if packet timed out start new packet and reset watchdog
	if(timedOut){
		WD_SET(WD_RST_IRQ,SERIAL_TIMEOUT);
		//if a packet was started, send error message reporting timeout
		if(received&&!badPacket){
			sendPacket(ERROR_LEN,0,address);
		}
		received=0;
		badPacket=0;
		checkSum=0;
		timedOut=0;
	}
	//while new bytes are ready reset watchdog and try to parse packet
	while(uartReceiveByte(USART_PRIMARY,&inByte)){
		wdt_reset();
		//increment received counter
		received++;
		if(!badPacket){
			checkPacket(inByte);
		}
	}
	
}

void checkPacket(uint8_t inByte ){
	//check packet based on how many bytes were received
	switch(received){
		//if this is the first byte check if it is a valid function
		case 1:
			if( inByte!=FUNC_NOP&&
				inByte!=FUNC_READ&&
				inByte!=FUNC_WRITE&&
				inByte!=FUNC_RESTART
			  ){
			  	//if not valid mark packet bad and send error
				badPacket=1;
				sendPacket(ERROR_FUNC,0,address);
			} else{
				function=inByte;
			}	
		break;
		//if second byte see that it is a sensible count
		case 2:
				count=inByte;
		break;
		//get MSB of address
		case 3:
			address.u1[1]=inByte;
		break;
		//get LSB of address and check if valid
		case 4:
			address.u1[0]=inByte;
			//if the address is in range and the function is write
			//check if the write affects read only registers
			if(address.u2+count<=(DEFUALT_REG_NUM+CUSTOM_REG_NUM)){
				if(function==FUNC_WRITE){
					for(int i=0;i<NUM_READ_ONLY+DEFUALT_READ_ONLY_NUM;i++){
						if(address.u2<=readOnly[i]&&address.u2+count>readOnly[i]){
							//if not valid mark packet bad and send error
							badPacket=1;
							sendPacket(ERROR_READ_ONLY,0,address);
						}
					}
				}
			}else{
				//if not valid mark packet bad and send error
				badPacket=1;
				sendPacket(ERROR_INVALID_REG,0,address);
			}
		break;
		//while there is more data to receive buffer it
		//once all data is received check checksum and perform
		//requested function
		default:
			if(function!= FUNC_WRITE || received==MSG_MIN_LEN+count*REG_SIZE){
				if(checkSum==inByte){
					polled=1;
					received=0;
					switch(function){
						case FUNC_READ:
							readReg();
						break;
						case FUNC_WRITE:
							writeReg();
						break;
						case FUNC_RESTART:
							WD_AVR_RESET();
						break;
						default:
						break;
					}
				}else{
					//if not valid mark packet bad and send error
					badPacket=1;
					sendPacket(ERROR_CHECK,0,address);
				}
			}
			else{
				dataBuff[received-(MSG_MIN_LEN)]=inByte;
			}
		break;		
	}
	checkSum+=inByte;
}

//send requested registers
void readReg(void){
	sendPacket(FUNC_READ_RESPONSE,count,address);
}

//take data and write to registers (data is big endian)
void writeReg(void){
	uint16_t curByte;
	uint16_t nextByte=address.u2*2;
	uint16_t lastByte=nextByte+count*2;
	uint16_t byteCount=0;
	while (nextByte<lastByte){
		curByte=nextByte;
		nextByte++;
		if(nextByte==lastByte||IS_START(nextByte)){
			commRegu8[curByte]=dataBuff[byteCount++];
			continue;
		}
		nextByte++;
		if(nextByte==lastByte||IS_START(nextByte)){
			commRegu8[curByte+1]=dataBuff[byteCount++];
			commRegu8[curByte]=dataBuff[byteCount++];
			continue;
		}
		nextByte+=2;
		commRegu8[curByte+3]=dataBuff[byteCount++];
		commRegu8[curByte+2]=dataBuff[byteCount++];
		commRegu8[curByte+1]=dataBuff[byteCount++];
		commRegu8[curByte]=dataBuff[byteCount++];
	}
	sendPacket(FUNC_WRITE_RESPONSE,0,address);
}

//Set the number of ticks before serial timeout
void serialSetTimeout(uint16_t timeout) {
  hostTimeout = timeout;
}

//Perform a tick on serial
void serialTick() {
  if (hostTime < hostTimeout) {
    hostTime++;
  }
}

//Return whether serial is active
int serialActive() {
  if (hostTimeout != 0) {
    return hostTime < hostTimeout;
  } else {
    return 1;
  }
}


//if watchdog timed out set timeOut flag

//ISR(SIG_WATCHDOG_TIMEOUT){
ISR(WDT_vect){ //NOAH hack. SIG_WATCHDOG_TIMEOUT was deprecated/poisoned in newest version of avr-libc so i changed this.
	timedOut=1;
}


