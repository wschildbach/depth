#include <stdlib.h>
#include <stdio.h>
#include "bitstream.h"

#include <stdint.h>

uint32_t _lfsr = 1;
static uint32_t lfsr(void)
{
 	/* taps: 32 31 29 1; characteristic polynomial: x^32 + x^31 + x^29 + x + 1 */
	return _lfsr = (_lfsr >> 1) ^ (-(_lfsr & 1u) & 0xD0000001u); 
}

#define CHECK(t,size) if (t != (lfsr() & ((1<<size)-1))) {fprintf(stderr,"error at i=%d\n",i); exit(20);}

int main(int ac, char *av[])
{
	int offset = atoi(av[1]);
	int size   = atoi(av[2]);
	int i;
	int t;

	_lfsr = 1;

	writeBits(lfsr() & ((1<<offset)-1),offset);
	for (i=0; i<1000; i++) {
		writeManyBits(lfsr() & ((1<<size)-1),size);
	}
	flushBits();
	
	resetBits();
	
	_lfsr = 1;
	t = readBits(offset);
	CHECK(t, offset);
	for (i=0; i<1000; i++) {
		t = readBits(size);
		CHECK(t,size);
	}	
}
