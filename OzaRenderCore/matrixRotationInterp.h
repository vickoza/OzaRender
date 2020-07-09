#pragma once
#include "Interpolator.h"

class matrixRotationInterp
{
public:
	virtual void Load(Loader& input);
	void Evaluate(double t, double mat[16]);
protected:

	Interpolator* columns[3];
};

