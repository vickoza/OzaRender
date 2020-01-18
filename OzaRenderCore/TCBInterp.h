#pragma once
#include "HermiteInterp.h"

class TCBInterp : public HermiteInterp
{
public:
	TCBInterp() { }
	virtual void SetValue(int index, const point& p);
	virtual void SetTime(int index, double t);
	virtual void SetKey(int index, double t, const point& p);
	virtual int InsertKey(double t, const point& p);
	virtual void DeleteKey(double t, double tolerance, bool all);
	virtual void DeleteKey(int index);

	virtual void SetTension(int index, double val);
	virtual void SetContinuity(int index, double val);
	virtual void SetBias(int index, double val);

	virtual double GetTension(int index) { return keys[index].tcb[0]; }
	virtual double GetContinuity(int index) { return keys[index].tcb[1]; }
	virtual double GetBias(int index) { return keys[index].tcb[2]; }

	virtual void Load(Loader& input);
	virtual bool ReadKeyAttribute(Loader& input, int nKey, char* attribute);

	//	Don't need an "Evaluate" method since a TCB is a Hermite Spline

protected:

	void RecalculateVectors(void);
};

