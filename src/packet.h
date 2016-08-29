//*****************************************************************************
//
///		This provides an implementation of the universal serial protocol
///		(https://cuauv.org/wiki/Electrical/UnifiedSerialProtocol)
///		using the serial library. It initializes buffers to handle 
///		sending and recieving without blocking the rest of the code.
///		packetHandlerInit initializes the handler, and handleSerialRx
///     must then be called faster then the serial timeout to check for
///     requests from the computer. Using this packet handler means that
///		the main code must use the commReg to make data available and read
///		data sent from the computer.
//
//*****************************************************************************

#ifndef PACKET_H
#define PACKET_H

//Error definitions
//*****************************************************************************

//Invalid Register(s)
//The read/write request specified registers that are non-existent.
#define ERROR_INVALID_REG 0x81

//Registers are Read Only
//The write request included a register that is read-only.
#define ERROR_READ_ONLY 0x82	

//Incorrect length
//The packet did not have internal consistency about length.
//For example, specifying a register count of five and then 
//only having data for four registers (this is detected via 
//timeout, most likely, and will be the most common error
//whenever we don't latch on the first byte correctly).
#define ERROR_LEN 0x83

//Bad checksum
//The checksum for the incoming packet did not match the calculated checksum.
#define ERROR_CHECK 0x84

//Invalid Function
//An Invalid function was specified 
#define ERROR_FUNC 0x85

//Unknown failure
//An unknown failure has occurred. Check for fires in or around the hydrophones. 	
#define ERROR_UNKNOWN 0xBF


//Serial Function Definitions
//*****************************************************************************

//NOP
//size=0
//Don't do anything. This really shouldn't be sent, but if we have some magical 
//screw up, it could happen.
#define FUNC_NOP	0x00

//Read Registers
//size=0
//This function tells the embedded target that the computer wishes to read a 
//specific number of data registers. Max of 255 at a time.
#define FUNC_READ	0x01
	
//Read Registers Response
//size=# registers * 2 bytes of data
//This function contains the requested registers, 2 bytes per register
#define FUNC_READ_RESPONSE	0x41
	
//Write Registers
//size=# registers * 2 bytes of data
//This function transmits data to be written to the given registers,
//a max of 255 at a time.	
#define FUNC_WRITE 0x02
	
//Write Registers Response
//size=0
//This indicates that the registers were successfully written.
#define FUNC_WRITE_RESPONSE 0x42

//Restart Chip
//size=0
//This forces a watchdog reset
#define FUNC_RESTART 0xAA

//Serial General Definitions
//*****************************************************************************
//number of bytes of overhead per packet
//header(1)+count(1)+address(2)+checksum(1)=5 
#define MSG_MIN_LEN 5
//the communication register in RAM used to implement the register
//map used in protocol
extern uint16_t commRegu16[];
//a point to the communication reg with a 1 byte stride
extern uint8_t *commRegu8;
//a union for accessing an address as two bytes or one two byte int
typedef union Addressu
{
  uint8_t u1[2];
  uint16_t u2;
}Address;

//size of register in bytes
#define REG_SIZE 2
//number of register reserved by the protocol
#define DEFUALT_REG_NUM 16
//read only default registers
#define DEFUALT_READ_ONLY_NUM 5
#define DEFUALT_READ_ONLY_ADDR 0,1,2,3,5

//Register == uint16_t
#define Register uint16_t
//sums the bytes to get the checksum from a array of data
uint8_t calcCheck(uint8_t*data,uint16_t len);
//method called to periodically handle requests from computer
void handleSerialRx(void);
//initializes packet handler commReg
void packetHandlerInit(void);
//internal method for checking bytes received so far
void checkPacket(uint8_t inByte);
//internal method for transmitting a packet
void sendPacket(uint8_t func,uint8_t count,Address addr);
//internal method to handle a read
void readReg(void);
//internal method to handle a write
void writeReg(void);
//check if a packet has been received since isPolled was last called
uint8_t isPolled(void);

//Set number of ticks before serial timeout
void serialSetTimeout(uint16_t timeout);
// Perform a tick on serial
void serialTick();
// Return whether serial is active
int serialActive();

#endif
