#include "bitstream.h"
#include "grcode.h"

#include <stdio.h>
#include <stdlib.h>

struct BITSTREAM bs ;

int main(int ac, char *av[])
{
  long t;
  FILE *f = fopen(av[1],"r");
  int Ml2 = atoi(av[2]);

  if (!f) {fprintf(stderr,"could not open %s for reading\n",av[1]);exit(10);}

  resetBits(&bs);
  while (!feof(f) && fscanf(f, "%ld\n",&t)) {
    printf("coding %ld\n",t);
    grsCode(&bs, t,Ml2);
  }
  fclose(f);

  fprintf(stderr,"done writing, now reading\n");

  flushBits(&bs);
  resetBits(&bs);

  f = fopen(av[1],"r");
  if (!f) {fprintf(stderr,"could not open %s for reading\n",av[1]);exit(10);}

  while (!feof(f) && fscanf(f, "%ld\n",&t)) {
    long r;
    r = grsDecode(&bs, Ml2);
    printf("decoding %ld\n",r);
    if (r != t) {fprintf(stderr,"error at %d\n",r);exit(20);}
  }
  fclose(f);
  return 0;
}
