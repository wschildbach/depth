#include <math.h>
#include <stdio.h>

#include "deltacode.h"
#include "grcode.h"

void initDeltaCode(struct DELTACODE *c, double scale, int Ml2)
{
//	resetBits(c->bs);
	c->alg = 0;
	c->scale = scale; c->Ml2 = Ml2;
	c->h[0] = c->h[1] = 0 ;
}

double deltaDecode(struct DELTACODE *c, struct BITSTREAM *bs)
{
	double p = 2.0*c->h[0] - c->h[1];
	double d = grsDecode(bs, c->Ml2) ;

	printf("p = %1.16lf, read d=%1.0lf\n",p,d);
	c->h[1] = c->h[0];
	c->h[0] = p+d/c->scale;

	return p + d/c->scale;
}

void deltaEncode(struct DELTACODE *c, struct BITSTREAM *bs, double x)
{
	/* predict new value */
	double p = 2.0*c->h[0]-c->h[1];
	/* delta to be coded is difference between prediction and true value, scaled, quauntized */
	double delta = floor(0.5+c->scale*(x-p));
//	double e = fabs(x-p-delta/scale);
	printf("p = %1.16lf, writ d=%1.0lf\n",p,delta);
	grsCode(bs, delta, c->Ml2);

	/* update filter history */
	c->h[1] = c->h[0];
	c->h[0] = p+delta/c->scale;
//	if (e>emax) emax = e;
}

#if 0
void finishCode(struct DELTACODE *c)
{
	flushBits(c->bs);
}
#endif
