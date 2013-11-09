#include "grcode.h"

#include <assert.h>
#include <stdio.h>

int grsDecode(struct BITSTREAM *bs, int Ml2)
{
	int n = grDecode(bs, Ml2);
//	printf("grsDecode: decoded n=%d\n",n);
	return ((n&1) ? (1-n)/2 : n/2);
}

int grDecode(struct BITSTREAM *bs, int Ml2)
{
	int q,r,c ;
	for (q=0; readBits(bs, 1) == 1; q++);
	r = readBits(bs, Ml2);
	c = r + (q<<Ml2);
	printf("q=%d, r=%d, c=%d\n",q,r,c);

	if (c < grCodeLimit) return c ;
	else {
		assert(c == grCodeLimit);
		printf("escape\n");//dbg=1;
		return readBits(bs, 32);
	}
}

static void _grCode(struct BITSTREAM *bs, int N, int Ml2);

/* unsigned golomb-rice coding */
void grCode(struct BITSTREAM *bs, int N, int Ml2)
{
	if (N < grCodeLimit) {
		_grCode(bs, N,Ml2);
	} else {
		_grCode(bs, grCodeLimit,Ml2); // serves as escape code
//		printf("escape, write %d\n",N);//dbg=1;
		writeManyBits(bs, N,32);
	}
}

/* unsigned golomb-rice coding */
void _grCode(struct BITSTREAM *bs, int N, int Ml2)
{
	int q = N>>Ml2;
	int r = N - (q<<Ml2);
	int i;

	/* unary code q */
	for (i=0; i < q; i++) writeBits(bs, 1,1);
	writeBits(bs, 0,1);

	/* code the remainder */
	writeBits(bs, r, Ml2);
//	printf("\n");
}

/* signed gr coding */
void grsCode(struct BITSTREAM *bs, int N, int Ml2)
{
	grCode(bs, N >= 0 ? 2*N : -2*N+1, Ml2);
}
