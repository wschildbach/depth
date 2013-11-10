#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

#define MAXBYTES 128000

struct BITSTREAM {
	unsigned char bitstream[MAXBYTES];
	unsigned char *bp;
	unsigned int cache;
	unsigned int bbFull;
} ;

void resetBits(struct BITSTREAM *bs);
void flushBits(struct BITSTREAM *bs);
int readBits(struct BITSTREAM *bs, int nBits);
void writeBits(struct BITSTREAM *bs, unsigned int bits, unsigned int nbits);
void writeManyBits(struct BITSTREAM *bs, unsigned int bits, unsigned int nbits);
int bufferSize(struct BITSTREAM *bs);

#endif /* _BITSTREAM_H_ */
