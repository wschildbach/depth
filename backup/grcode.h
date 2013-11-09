#ifndef _GRCODE_H_
#define _GRCODE_H_

#include "bitstream.h"

#define grCodeLimit (8<<Ml2)

extern int grsDecode(struct BITSTREAM *bs, int Ml2);
extern int grDecode(struct BITSTREAM *bs, int Ml2);
extern void grCode(struct BITSTREAM *bs, int N, int Ml2);
extern void grsCode(struct BITSTREAM *bs, int N, int Ml2);

#endif
