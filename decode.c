#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bitstream.h"
#include "grcode.h"
#include "deltacode.h"

void timedec(struct BITSTREAM *bs, int n, double *store)
{
  int i=0;
  do {
    int j,len;
    signed long t;
    t = readBits(bs, 32);
    printf("%ld\n",t);
    store[6*i] = t;
    len = readBits(bs, 32);
    printf("len %d\n",len);

    for (j=i; j<i+len; j++) store[6*(j+1)] = store[6*j]+2;
    i = j+1;
  } while (i<n);
}

void coldec(struct BITSTREAM *bs, int n, double *store, int col)
{
  double h[2];
  int i;
  double scale = readBits(bs, 32);
  int Ml2 = readBits(bs, 8);
  struct DELTACODE dc ;
  
  printf("coldec scale=%lf, Ml2=%d\n",scale,Ml2);
	
#if 1
  initDeltaCode(&dc, scale, Ml2);
  for (i=0; i<n; i++) {
    store[6*i+col] = deltaDecode(&dc, bs);
  }
#else
  h[0] = h[1] = 0 ;
  for (i=0; i<n; i++) {
    double p = 2.0*h[0]-h[1];
    double d = grsDecode(bs, Ml2) ;
    
    printf("p = %1.16lf, read d=%1.0lf\n",p,d);
    store[6*i+col] = p + d/scale;
    h[1] = h[0];
    h[0] = p+d/scale;
  }
#endif
}

void writeFile(int n, double *store, const char *fn)
{
	int i;
	FILE *f = fopen(fn,"w");
	if (!f) {fprintf(stderr,"could not open file %s\n",fn);exit(20);}

	for (i=0; i<n; i++) {
		fprintf(f,"%1.14lf,%1.14lf,%1.14lf,%1.14lf,0,%1.14lf\n",store[6*i],store[6*i+1],store[6*i+2],store[6*i+3],store[6*i+5]);
	}
	
	fclose(f);
}

int main(int ac, char *av[])
{
  int c,n ;
  struct BITSTREAM bs ;
  FILE *f = fopen(av[1],"rb");
  if (!f) {fprintf(stderr,"could not open file %s\n",av[1]); exit(20);}
  double *store = 0 ;
	
  resetBits(&bs);

  fread(bs.bitstream,MAXBYTES,1,f);
  fclose(f);

  c = readBits(&bs, 8); printf("%c",c);
  c = readBits(&bs, 8); printf("%c",c);
  c = readBits(&bs, 8); printf("%c",c);
  c = readBits(&bs, 8); printf("%c",c);

  c = readBits(&bs, 8); printf(" version %d\n",c);
  n = readBits(&bs, 32); printf("lines: %ld\n",n);

  store = malloc(6*sizeof(*store)*n);
//	timedec(&bs, n,store);

  coldec(&bs, n, store, 0);

  coldec(&bs, n, store, 1);
  coldec(&bs, n, store, 2);
  coldec(&bs, n, store, 3);

  coldec(&bs, n, store, 5);

  writeFile(n,store, av[2]);

  return 0;
}
