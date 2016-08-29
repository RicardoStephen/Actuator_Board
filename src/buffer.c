#include "common.h"
#include "buffer.h"

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif

// global variables

// initialization

void bufferInit(cBuffer* buffer, unsigned char *start, uint16_t size)
{
	// begin critical section
	CRITICAL_SECTION_START;
	// set start pointer of the buffer
	buffer->dataptr = start;
	buffer->size = size;
	// initialize index and length
	buffer->dataindex = 0;
	buffer->datalength = 0;
	// end critical section
	CRITICAL_SECTION_END;
}

// access routines
uint8_t  bufferGetFromFront(cBuffer* buffer,uint8_t *data)
{
	// begin critical section
	CRITICAL_SECTION_START;
	// check to see if there's data in the buffer
	if(buffer->datalength){

		// get the first character from buffer
		*data = buffer->dataptr[buffer->dataindex++];
		//if index overflowed wrap around
		buffer->dataindex&=buffer->size;
		buffer->datalength--;
		CRITICAL_SECTION_END;
		return TRUE;
	}
	// end critical section
	CRITICAL_SECTION_END;
	// return
	return FALSE;
}

void bufferDumpFromFront(cBuffer* buffer, unsigned short numbytes)
{
	// begin critical section
	CRITICAL_SECTION_START;
	// dump numbytes from the front of the buffer
	// are we dumping less than the entire buffer?
	if(numbytes < buffer->datalength)
	{
		// move index by numbytes and decrement length by numbytes
		buffer->dataindex += numbytes;
		//if index overflowed wrap around
		buffer->dataindex&= buffer->size;
		buffer->datalength -= numbytes;
	}
	else
	{
		// flush the whole buffer
		buffer->datalength = 0;
	}
	// end critical section
	CRITICAL_SECTION_END;
}

unsigned char bufferGetAtIndex(cBuffer* buffer, unsigned short index)
{
	// begin critical section
	CRITICAL_SECTION_START;
	// return character at index in buffer
	unsigned char data = buffer->dataptr[(buffer->dataindex + index)&buffer->size];
	// end critical section
	CRITICAL_SECTION_END;
	return data;
}

unsigned char bufferAddToEnd(cBuffer* buffer, unsigned char data)
{
	// begin critical section
	CRITICAL_SECTION_START;
	// make sure the buffer has room
	if(buffer->datalength <= buffer->size)
	{		
		// save data byte at end of buffer
		buffer->dataptr[(buffer->dataindex + buffer->datalength)&buffer->size] = data;
		// increment the length
		buffer->datalength++;
		// end critical section
		CRITICAL_SECTION_END;
		// return success
		return TRUE;
	}
	// end critical section
	CRITICAL_SECTION_END;
	// return failure
	return FALSE;
}

void bufferFlush(cBuffer* buffer)
{
	// begin critical section
	CRITICAL_SECTION_START;
	// flush contents of the buffer
	buffer->datalength = 0;
	// end critical section
	CRITICAL_SECTION_END;
}

