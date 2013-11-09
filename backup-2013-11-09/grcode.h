#ifndef _GRCODE_H_
#define _GRCODE_H_

/*
 * routines for Golomb-Rice coding
 */

#include "bitstream.h"

#define grCodeLimit (8<<Ml2)

/*! decode a signed value from a Golomb-Rice code.
 * @in Ml2: Golomb-rice coding parameter log2(M)
 */
extern signed int grsDecode(struct BITSTREAM *bs, int Ml2);
/*! decode an unsigned value from a Golomb-Rice code.
 * @in Ml2: Golomb-rice coding parameter log2(M)
 */
extern unsigned int grDecode(struct BITSTREAM *bs, int Ml2);
/*! encode a signed value into a Golomb-Rice code.
 * @in N: the value
 * @in Ml2: Golomb-rice coding parameter log2(M)
 */
extern void grCode(struct BITSTREAM *bs, unsigned int N, int Ml2);
/*! encode an unsigned value into a Golomb-Rice code.
 * @in N: the value
 * @in Ml2: Golomb-rice coding parameter log2(M)
 */
extern void grsCode(struct BITSTREAM *bs, signed int N, int Ml2);

#endif
