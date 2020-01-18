#pragma once
#include "Interpolator.h"

class HermiteInterp : public Interpolator
{
public:

	virtual point evaluate(double s) override final;

	virtual void SetKeyInVector(int index, svector vin);
	virtual void SetKeyOutVector(int index, svector vout);
	virtual void SetKeyVector(int index, svector v);

	virtual svector GetKeyInVector(int index) { return keys[index].v0; }
	virtual svector GetKeyOutVector(int index) { return keys[index].v1; }
	virtual svector GetKeyVector(int index) { return keys[index].v0; }

	virtual bool ReadKeyAttribute(Loader& input, int nKey, char* attribute);

protected:

	virtual void RebuildParamDistanceTable(void) override;

	double averageDT;
};
