#include "bitstream.h"
#include "grcode.h"

#include <stdio.h>
#include <stdlib.h>

int main(int ac, char *av[])
{
	long t;
	FILE *f = fopen(av[1],"r");
	int Ml2 = atoi(av[2]);

	if (!f) {fprintf(stderr,"could not open %s for reading\n",av[1]);exit(10);}
	while (!feof(f) && fscanf(f, "%ld\n",&t)) {
		printf("coding %ld\n",t);
		grsCode(t,Ml2);
	}
	fclose(f);
	
	flushBits();
	resetBits();

	f = fopen(av[1],"r");
	if (!f) {fprintf(stderr,"could not open %s for reading\n",av[1]);exit(10);}
	while (!feof(f) && fscanf(f, "%ld\n",&t)) {
		long r;
		r = grsDecode(Ml2);
		printf("decoding %ld\n",r);
		if (r != t) {fprintf(stderr,"error at %d\n",r);exit(20);}
	}
	fclose(f);
	return 0;
}
