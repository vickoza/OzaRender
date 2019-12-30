#pragma once
#include <vector>
#include "svector.h"
#include "point.h"
#include "Loader.h"
struct InterpKey
{
	InterpKey() : constant( false ), v0(0, 0, 0), v1(0, 0, 0), dist(0), tcb(0, 0, 0), p (0, 0, 0), t(0.0), easeIn(0), easeOut(0) { }
	InterpKey(double time, const point& pt) : constant(false), v0(0, 0, 0), v1(0, 0, 0), dist(0), tcb(0, 0, 0), p(pt), t(time), easeIn(0), easeOut(0) { }
	// Curve Data
	point p;
	svector v0, v1;
	svector tcb;
	double easeIn, easeOut;

	// Timing Data
	double t;
	double dist;
	bool constant; // Should the object move with a constant speed along the segment
};

struct ParamDistEntry
{
	double s;
	double d;
};

class Interpolator
{
public:

	Interpolator() { distTable = nullptr; }

	virtual point Evaluate(double t);
	virtual svector Velocity(double t, double h);
	virtual svector Up(double t, double h);
	virtual svector Acceleration(double t, double h);

	virtual void SetValue(int index, const point& p);
	virtual void SetTime(int index, double t);
	virtual void SetConstant(int index, bool value) { keys[index].constant = value; RebuildParamDistanceTable(); }
	virtual void SetKey(int index, double t, const point& p);
	virtual int  InsertKey(double t, const point& p);
	virtual void DeleteKey(double t, double tolerance, bool all);
	virtual void DeleteKey(int index);
	virtual void SetEaseIn(int index, double in) { keys[index].easeIn = in; }
	virtual void setEaseOut(int index, double out) { keys[index].easeOut = out; }

	virtual double GetTime(int index) { return keys[index].t; }
	virtual point GetValue(int index) { return keys[index].p; }
	virtual bool GetConstant(int index) { return keys[index].constant; }
	int FindInterval(double T);
	int FindKey(double T, double tolerance = .01);
	int NKeys(void) { return keys.size(); }

	virtual void Load(Loader& input);
	virtual bool ReadKeyAttribute(Loader& input, int nKey, char* attribute) { return false; }
	virtual bool ReadAttribute(Loader& input, char* attribute) { return false; }

	static Interpolator* LoadInterpolator(Loader& input);

protected:

	void SortKeys(void);

	virtual void RebuildParamDistanceTable(void);

	virtual point evaluate(double s) = 0;

	std::vector<InterpKey> keys;	

	// DistTable will be 100 * nKeys long.  
	ParamDistEntry* distTable;
};

