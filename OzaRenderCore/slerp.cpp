#include "slerp.h"

quaternion slerp::Evaluate(double t)
{

	// Find the interval that t is on
	// 
	// Calculate the percentage that t is along that interval
	// 
	// Use that percentage to calculate the slerp between the 
	// two quaternions.
	//
	// Be careful of division by 0 in the slerp formula!
	if (t < keys[0].t)
		return keys[0].q;
	else if (t > keys.back().t)
		return keys.back().q;
	int i = FindInterval(t);
	double p = (t - keys[i].t) / (keys[i + 1].t - keys[i].t);

	double costheta = dot(keys[i].q, keys[i + 1].q);
	double theta = acos(costheta);
	double sinTheta = sqrt(1.0 - costheta * costheta);
	if (abs(theta) < 0.01)
		return keys[i].q;
	if (abs(sinTheta) < 0.01)
		return .5 * (keys[i].q + keys[i + 1].q);
	double ratioA = sin((1 - p) * theta) / sinTheta;
	double ratioB = sin(p * theta) / sinTheta;
	return ((ratioA * keys[i].q) + (ratioB * keys[i + 1].q));
}
