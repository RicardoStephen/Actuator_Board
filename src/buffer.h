//*****************************************************************************
//
///		This byte-buffer structure provides an easy and efficient way to store
///		and process a stream of bytes.  You can create as many buffers as you
///		like (within memory limits), and then use this common set of functions to
///		access each buffer.  The buffers are designed for FIFO operation (first
///		in, first out).  This means that the first byte you put in the buffer
///		will be the first one you get when you read out the buffer.  Supported
///		functions include buffer initialize, get byte from front of buffer, add
///		byte to end of buffer, check if buffer is full, and flush buffer.  The
///		buffer uses a circular design so no copying of data is ever necessary.
///		This buffer is not dynamically allocated, it has a user-defined fixed
///		maximum size that must be powers of 2 to make modular division more
///		efficient. 
//
//*****************************************************************************

#ifndef BUFFER_H
#define BUFFER_H

//No buffer
#define SIZE0 0
//buffer size 16 bytes
#define SIZE16 0xF
//buffer size 32 bytes
#define SIZE32 0x1F
//buffer size 64 bytes
#define SIZE64 0x3F
//buffer size 128 bytes
#define SIZE128 0x7F
//max size 256 bytes
#define SIZE256 0xFF

// structure/typdefs
//! cBuffer structure
typedef struct struct_cBuffer
{
	unsigned char *dataptr;			///< the physical memory address where the buffer is stored
	uint16_t size;					///< the allocated size of the buffer
	unsigned short datalength;		///< the length of the data currently in the buffer
	unsigned short dataindex;		///< the index into the buffer where the data starts
} cBuffer;

// function prototypes

//! initialize a buffer to start at a given address and have given size
void			bufferInit(cBuffer* buffer, unsigned char *start, uint16_t size);

//! get the first byte from the front of the buffer 
//return 0 if unsucessful
uint8_t  bufferGetFromFront(cBuffer* buffer,uint8_t *data);

//! dump (discard) the first numbytes from the front of the buffer
void bufferDumpFromFront(cBuffer* buffer, unsigned short numbytes);

//! get a byte at the specified index in the buffer (kind of like array access)
// ** note: this does not remove the byte that was read from the buffer
unsigned char	bufferGetAtIndex(cBuffer* buffer, unsigned short index);

//! add a byte to the end of the buffer
//returns 0 if full
unsigned char	bufferAddToEnd(cBuffer* buffer, unsigned char data);

//! flush (clear) the contents of the buffer
void			bufferFlush(cBuffer* buffer);

#endif
//@}
