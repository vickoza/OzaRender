#pragma once
#include <vector>
#include <algorithm>
#include "quaternion.h"
#include "Loader.h"

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

	int FindInterval(double t)
	{
		// To Do
		//
		// Again, perform a binary search for the key at the
		// beginning of the interval containing t
		if (keys.size() < 2)
			return 0;
		else if (t < keys.front().t)	// Off the beginning
			return 0;
		else if (t > keys.back().t) // Off the end
			return keys.size() - 2;

		//  Binary Serach
		int a = 0, b = keys.size() - 1;

		while ((b - a) > 1)
		{
			if (t > keys[(a + b) / 2].t)
				a = (a + b) / 2;
			else
				b = (a + b) / 2;
		}
		return a;
	}

	virtual quaternion Evaluate(double t) = 0;
	auto NKeys(void) { return keys.size(); }

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
		std::sort(keys.begin(), keys.end(), [](const auto& a, const auto& b) {return a.t < b.t; });
	}


	std::vector<QuaternionKey> keys;
};
