#include "LinearInterp.h"

point LinearInterp::evaluate(double t)
{
	double s;
	if (t < keys[0].t)
		return keys[0].p;
	if (t > keys.back().t)
		return keys.back().p;
	int i = FindInterval(t);
	s = (t - keys[i].t) / (keys[i + 1].t - keys[i].t);
	return keys[i].p + s * (keys[i + 1].p - keys[i].p);
}