#include <stdio.h>
#include <stdlib.h>

#include "bitstream.h"


static int dbg=0;


int bufferSize(struct BITSTREAM *bs)
{
	return (bs->bp-bs->bitstream)*8+bs->bbFull;
}

void flushBits(struct BITSTREAM *bs)
{
	writeBits(bs, 0,7);
}

void resetBits(struct BITSTREAM *bs)
{
	bs->bp=bs->bitstream;
	bs->cache = 0;
	bs->bbFull = 0;
}

int readBits(struct BITSTREAM *bs, int nBits)
{
	int r ;

	if (nBits > 24) {
		nBits -= 24;
		return (readBits(bs, 24)<<nBits) | readBits(bs, nBits) ;
	}

	while (bs->bbFull < 24) {
		bs->cache = (bs->cache << 8) | *(bs->bp);
		++(bs->bp);
		bs->bbFull += 8;
	}

	r = bs->cache >> (bs->bbFull-nBits);
	bs->cache = bs->cache ^ (r << (bs->bbFull-nBits));
	bs->bbFull -= nBits;

	return r ;
}

void _writeBits(struct BITSTREAM *bs, unsigned int bits, unsigned int nbits)
{

	/*
		The cache is left-aligned.
		It is always less than 8 bits full (bbFull<8)
		Therefore as long as nbits <=25, bits << bbFull does not overflow
		(provided an int has 32 bits)
	 */
	bs->cache |= (bits << (32-nbits-bs->bbFull));
	bs->bbFull += nbits;

	while (bs->bbFull >= 8) {
		*(bs->bp) = bs->cache >> 24 ;
		++(bs->bp);
		bs->cache <<= 8;
		bs->bbFull -= 8;
		if (bs->bp - bs->bitstream == MAXBYTES) {fprintf(stderr,"bitbuffer full\n"); exit(10);}
	}
}

void writeBits(struct BITSTREAM *bs, unsigned int bits, unsigned int nbits)
{
	int i,j;
	if (dbg) for (i=1; i <= nbits; i++) {putchar('0'+((bits>>(nbits-i))&1));}
	
	_writeBits(bs, bits,nbits);
}

void writeManyBits(struct BITSTREAM *bs, unsigned int bits, unsigned int nbits)
{
	/*
		main routine can only handle up to 24 bits. If we have more,
		tile the write.
	*/
	for (; nbits >= 24; nbits -= 24 ) {
		int w = bits >> (nbits-24);
		writeBits(bs, bits >> (nbits-24), 24);
		bits ^= w << (nbits-24);
	}
	writeBits(bs, bits, nbits);
}
