#pragma once
#include <vector>
#include <algorithm>
#include "quaternion.h"

struct QuaternionKey
{
	quaternion q;
	double t;
};

const auto MAX_KEYS = 1000;

class QuaternionInterplotator
{
public:

	QuaternionInterplotator() { keys.reserve(MAX_KEYS); }

	void SetKeyValue(int index, const quaternion& q) { keys[index].q = q; }
	void SetKeyTime(int index, double time) { keys[index].t = time; SortKeys(); }

	int FindInterval(double t);
	virtual quaternion Evaluate(double t) = 0;
	int NKeys(void) { return nKeys; }

	virtual void Load(Loader& input);
	static QuaternionInterplotator* LoadQuaternionInterpolator(Loader& input);

protected:

	void SortKeys(void)
	{
		// To Do
		//
		// Run a bubble sort (or a faster sort if you wish)
		// To sort the keys by their time field.  If you want
		// the bubble sort to go faster, then since we will often
		// be inserting on the end, reverse the bubble sort so 
		// that it goes from the end up to the front of the array.
		std::sort(keys.begin(), keys.end(), [](const auto& a, const auto& b) {return a.t < b.t; })
	}


	std::vector<QuaternionKey> keys;
};

class slerp : public QuaternionInterp
{
public:

	virtual quaternion Evaluate(double t);

};

class bezierQuaternion : public QuaternionInterp
{
	//used to help find a quaternion
private:
	quaternion slerpM(quaternion q1, quaternion q2, double t);
	quaternion squad(quaternion s1, quaternion s2, quaternion a, quaternion b, double t);
	quaternion getQuaternion(quaternion s1, quaternion s2, quaternion s3);
public:
	virtual quaternion Evaluate(double t);

};

