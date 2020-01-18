#include "bezierQuaternion.h"

quaternion bezierQuaternion::slerpM(quaternion q1, quaternion q2, double t)
{
	double costheta = dot(q1, q2);
	double theta = acos(costheta);
	double sinTheta = sqrt(1.0 - costheta * costheta);
	if (abs(theta) < 0.01)
		return q1;
	if (abs(sinTheta) < 0.01)
		return .5 * (q1 + q2);
	double ratioA = sin((1 - t) * theta) / sinTheta;
	double ratioB = sin(t * theta) / sinTheta;
	return ((ratioA * q1) + (ratioB * q2));
}
quaternion bezierQuaternion::Evaluate(double t)
{
	if (t < keys[0].t)
		return keys[0].q;
	else if (t > keys.back().t)
		return keys.back().q;
	int i = FindInterval(t);
	double p = (t - keys[i].t) / (keys[i + 1].t - keys[i].t);
	if (keys.size() == 2)
		return slerpM(keys[0].q, keys[1].q, p);
	if (keys.size() == 3)
	{
		quaternion a = getQuaternion(keys[0].q, keys[1].q, keys[2].q);
		return squad(keys[i].q, keys[i + 1].q, a, a, p);
	}
	if (i == 0)
	{
		quaternion a = getQuaternion(keys[0].q, keys[0].q, keys[1].q);
		quaternion b = getQuaternion(keys[0].q, keys[1].q, keys[2].q);
		return squad(keys[i].q, keys[i + 1].q, a, b, p);
	}
	if (i == (keys.size() - 2))
	{
		quaternion a = getQuaternion(keys[keys.size() - 3].q, keys[keys.size() - 2].q, keys.back().q);
		quaternion b = getQuaternion(keys[keys.size() - 2].q, keys.back().q, keys.back().q);
		return squad(keys[i].q, keys[i + 1].q, a, b, p);
	}
	quaternion a = getQuaternion(keys[i - 1].q, keys[i].q, keys[i + 1].q);
	quaternion b = getQuaternion(keys[i].q, keys[i + 1].q, keys[i + 2].q);
	return squad(keys[i].q, keys[i + 1].q, a, b, p);

}

quaternion bezierQuaternion::squad(quaternion s1, quaternion s2, quaternion b1, quaternion b2, double t)
{
	quaternion temp1, temp2;
	temp1 = slerpM(b1, b2, t);
	temp2 = slerpM(s1, s2, t);
	return slerpM(temp1, temp2, t);
}

quaternion bezierQuaternion::getQuaternion(quaternion s1, quaternion s2, quaternion s3)
{
	double th0, th1, th2, a;
	th0 = s1.angleR();
	th1 = s2.angleR();
	th2 = s3.angleR();
	a = (th0 - 6 * th1 + th2) / 4;
	quaternion q(0.0, -1.0);
	return sin(a) * q;
}
