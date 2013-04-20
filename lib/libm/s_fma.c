#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	double fma(double x, double y, double z)
#else
	double fma(x,y)
	double x;
	double y;
        double z;
#endif
{
  /* Implementation defined. */
  return (x * y) + z;
}
