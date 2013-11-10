#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "bitstream.h"

#include "grcode.h"
#include "deltacode.h"

double *store = 0 ;

int readfile(const char *fn)
{
  int n = 0;
  int nn = 1;
  store = malloc(nn*6*sizeof(*store));

  FILE *f = fopen(fn,"r");
  if (!f) {fprintf(stderr,"no such file %s\n",fn); exit(20);}

  while (!feof(f)) {
    n++; //printf("n = %d\n",n);
    if (n > nn) {
      nn = 2*nn;
      store = realloc(store,nn*6*sizeof(*store));
    }
    if (0 == fscanf(f,"%lf,%lf,%lf,%lf,%lf,%lf\n",store+6*n-6,store+6*n-5,store+6*n-4,store+6*n-3,store+6*n-2,store+6*n-1)) {--n; break;}
  }
  fclose(f);
  return n ;
}

int timeenc(struct BITSTREAM *bs, int n, const double *store)
{
  int i=0;
  int nbits = bufferSize(bs);
  
  do {
    int j;
    signed int time = store[6*i];
    writeManyBits(bs, time,32);
    for (j=i;j < n-1 && store[6*(j+1)]-store[6*j]==2;j++);
    writeManyBits(bs, j-i,32);
    
    i = j+1;
  } while (i<n);
  return bufferSize(bs) - nbits ;
}

static int _colenc(struct BITSTREAM *bs, int n, const double *store, int col, double scale, int Ml2);

/* a wrapper routine to optimize for the best coding parameter */ 
int colenc(struct BITSTREAM *bs, int n, const double *store, int col, double scale)
{
  int minbits = 1<<30;
  int minMl2 = 0;
  int Ml2;
  for (Ml2 = 1; Ml2 <= 8; Ml2++) {
    int nbits;
    struct BITSTREAM bs0;
    resetBits(&bs0);

    _colenc(&bs0,n,store,col,scale,Ml2);
    nbits = bufferSize(&bs0);
    if (nbits < minbits) {
      minbits = nbits;
      minMl2 = Ml2;
    }
  }
  assert(minMl2 > 0);

  printf("storing col %d at Ml2 = %d\n",col,minMl2);
  _colenc(bs,n,store,col,scale,minMl2);

  return 0;
}

int _colenc(struct BITSTREAM *bs, int n, const double *store, int col, double scale, int Ml2)
{
#if 0
  double h[2];
  double emax = 0.0;
#endif
  int i;
  int nbits = bufferSize(bs);
  struct DELTACODE dc;

  writeManyBits(bs, (int)scale, 32);
  writeBits(bs, Ml2, 8);

#if 1
  initDeltaCode(&dc, scale, Ml2);
  for (i=0; i<n; i++) {
    deltaEncode(&dc, bs, store[6*i+col]);
  }
#else
  h[0] = h[1] = 0 ;
  for (i=0; i<n; i++) {
    double p = 2.0*h[0]-h[1];
    double d = floor(0.5+scale*(store[6*i+col]-p));
    double e = fabs(p+d/scale-store[6*i+col]);
    printf("p = %1.16lf, writ d=%1.0lf, Ml2=%d\n",p,d,Ml2);
    grsCode(bs, d, Ml2);
    h[1] = h[0];
    h[0] = p+d/scale;
    if (e>emax) emax = e;
  }
  printf("emax = %le\n",emax);
#endif
  return bufferSize(bs) - nbits;
}

int main(int ac, char *av[])
{
  //  int i;
  int bits ;
  int totbits = 0 ;
  int n = readfile(av[1]);
  //  int Ml2 = atoi(av[3]);
  struct BITSTREAM bs ; 

  printf("%d values\n",n);

  resetBits(&bs);
  // encode header

  // NEMC
  writeBits(&bs, 'N',8);writeBits(&bs, 'M',8);writeBits(&bs, 'E',8);writeBits(&bs, 'C',8);
  writeBits(&bs, '0',8); // version 0

  // number of lines
  writeBits(&bs, n,32);

  // encode time

//	bits = timeenc(&bs, n, store);
  bits = colenc(&bs, n, store, 0, 1); // ,1
  printf("%d bits for time/date encoding (%f per line)\n",bits,(float)bits/n);

  bits =  colenc(&bs, n, store, 1, 1E7); // , Ml2);
  bits += colenc(&bs, n, store, 2, 1E7);//, Ml2);
  bits += colenc(&bs, n, store, 3, 1E7);//, Ml2);
  
  printf("%d bits for position encoding (%f per line)\n",bits,(float)bits/n);
  
  bits = colenc(&bs, n, store, 5, 5);//, Ml2);
  printf("%d bits for depth encoding (%f per line)\n",bits,(float)bits/n);
  
  flushBits(&bs);
  
  totbits = (bs.bp - bs.bitstream)*8;
  printf("real  size: %d bytes (%1.1f bits per line)\n",totbits/8,(float)totbits/n);
  
  {
    FILE *f = fopen(av[2],"wb");
    if (!f) {fprintf(stderr,"could not open file %s\n",av[2]);exit(20);}
    
    fwrite(bs.bitstream,bs.bp - bs.bitstream,1,f);
    fclose(f);
  }
  return 0;
}
