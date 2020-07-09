#pragma once
#include "Interpolator.h"
class LinearInterp : public Interpolator
{
public:
	virtual point evaluate(double s) override;
};

