#ifndef _deltacode_h_
#define _deltacode_h_

#include "bitstream.h"

struct DELTACODE {
	int alg ; /*< algorithm */
	double h[2] ; /*< history */
	double scale ;
	int Ml2 ;
} ;

void initDeltaCode(struct DELTACODE *c, double scale, int Ml2);
double deltaDecode(struct DELTACODE *c, struct BITSTREAM *bs);
void deltaEncode(struct DELTACODE *c, struct BITSTREAM *bs, double x);
//void finishCode(struct DELTACODE *c);

#endif
