#include "grcode.h"

#include <assert.h>
#include <stdio.h>

/*! decode a signed value from a Golomb-Rice code.
 * @in Ml2: Golomb-rice coding parameter log2(M)
 */
signed int grsDecode(struct BITSTREAM *bs, int Ml2)
{
  unsigned int n = grDecode(bs, Ml2);
  /*
  printf("n=%d, -n=%d\n",n,-(signed)n);
  assert((-(signed)n) < (signed)n);
  printf("grsDecode: decoded n=%d\n",n);
  */
  /* bit #0 is the sign bit */
  return ((n&1) ? (1-(signed int)n)/2 : n/2);
}

unsigned int grDecode(struct BITSTREAM *bs, int Ml2)
{
  unsigned int q,r,c ;
  /* read q parameter: q = "the number of 1 bits" */
  for (q=0; readBits(bs, 1) == 1; q++);
  /* read mantissa */
  r = readBits(bs, Ml2);
  /* calculate output */
  c = r + (q<<Ml2);

  /* if c == grCodeLimit, this triggers the escape mechanism */
  if (c < grCodeLimit) return c ;
  else {
    assert(c == grCodeLimit);

    return readBits(bs, 32);
  }
}

static void _grCode(struct BITSTREAM *bs, unsigned int N, int Ml2);

/* unsigned golomb-rice coding */
void grCode(struct BITSTREAM *bs, unsigned int N, int Ml2)
{
  if (N < grCodeLimit) {
    _grCode(bs, N,Ml2);
  } else {
    _grCode(bs, grCodeLimit,Ml2); // serves as escape code
    writeManyBits(bs, N,32);
  }
}

/* unsigned golomb-rice coding */
void _grCode(struct BITSTREAM *bs, unsigned int N, int Ml2)
{
  unsigned int q = N>>Ml2;
  unsigned int r = N - (q<<Ml2);
  int i;

  /* unary code q, slightly optimized */
  while (q >= 16) {writeBits(bs, (1<<16)-1, 16); q-=16;}
  while (q >= 8)  {writeBits(bs,  (1<<8)-1, 8);  q-=8;}
  while (q >= 4)  {writeBits(bs,  (1<<4)-1, 4);  q-=4;}
       
  for (i=0; i < q; i++) writeBits(bs, 1,1);

  writeBits(bs, 0,1);

  /* code the remainder */
  writeBits(bs, r, Ml2);
}

/* signed gr coding. Code the sign bit as bit 0 */
void grsCode(struct BITSTREAM *bs, signed int N, int Ml2)
{
  int N2 = 2*N;
  assert(N2/2 == N);
  grCode(bs, N >= 0 ? N2 : -N2+1, Ml2);
}
