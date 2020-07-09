#pragma once
#include "QuaternionInterplotator.h"

class bezierQuaternion : public QuaternionInterplotator
{
	//used to help find a quaternion
private:
	quaternion slerpM(quaternion q1, quaternion q2, double t);
	quaternion squad(quaternion s1, quaternion s2, quaternion a, quaternion b, double t);
	quaternion getQuaternion(quaternion s1, quaternion s2, quaternion s3);
public:
	virtual quaternion Evaluate(double t) override;

};
