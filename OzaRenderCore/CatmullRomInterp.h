#pragma once
#include "HermiteInterp.h"

enum class endAction
{
	stop, linear, quadratic
};

class CatmullRomInterp : public HermiteInterp
{
public:

	CatmullRomInterp() { begin = endAction::quadratic; end = endAction::quadratic; }
	virtual void SetValue(int index, const point& p);
	virtual void SetTime(int index, double t);
	virtual void SetKey(int index, double t, const point& p);
	virtual int InsertKey(double t, const point& p);
	virtual void DeleteKey(double t, double tolerance, bool all);
	virtual void DeleteKey(int index);

	void SetBeginAction(endAction value) { begin = value; RecalculateVectors(); }
	void SetEndAction(endAction value) { end = value; RecalculateVectors(); }

	virtual void Load(Loader& input);
	virtual bool ReadAttribute(Loader& input, char* attribute);

	//	Don't need an "Evaluate" method since a Catmull-Rom is a Hermite Spline

protected:

	void RecalculateVectors(void);

	endAction begin, end;
};

