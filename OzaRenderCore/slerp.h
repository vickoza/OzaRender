#pragma once
#include "QuaternionInterplotator.h"

class slerp : public QuaternionInterplotator
{
public:

	virtual quaternion Evaluate(double t) override;
};
