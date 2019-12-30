#include <algorithm>
#include <iostream>
#include "Interpolator.h"
int Interpolator::FindInterval(double t)
{
	if (keys.size() < 2 )
		return 0;
	else if (t < keys[0].t)	// Off the beginning
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

int Interpolator::FindKey(double T, double tolerance)
{
	int i = FindInterval(T);

	// If there is a key then it is either i or i+1 because T is on the interval between keys i and i+1
	if (fabs(keys[i].t - T) < tolerance)
		return i;
	else if (fabs(keys[i + 1].t - T) < tolerance)
		return i + 1;
	else
		return -1;
}

// Simple reverse bubble sort for sorting keys
// Reverse it to make it more efficient for inserting
// keys on the end.
void Interpolator::SortKeys(void)
{
	/*bool found;

	do
	{
		found = false;
		for (int i = nKeys - 1; i > 0; i--)
		{
			if (keys[i - 1].t > keys[i].t)
			{
				InterpKey tmp = keys[i];
				keys[i] = keys[i - 1];
				keys[i - 1] = tmp;
				found = true;
			}
		}
	} while (found == true);*/
	std::sort(keys.begin(), keys.end(), [](const auto& a, const auto& b) { return a.t < b.t; });
}

void Interpolator::SetValue(int index, const point& p)
{
	if (index >= keys.size())
		std::cout << "Tried to set key position above nKeys = " << keys.size() << "; Command ignored.\n";

	keys[index].p = p;

	RebuildParamDistanceTable();
}

void Interpolator::SetTime(int index, double t)
{
	if (index >= keys.size())
		std::cout << "Tried to set key position above nKeys = " << keys.size() << "; Command ignored.\n";

	keys[index].t = t;

	SortKeys();
	RebuildParamDistanceTable();
}

void Interpolator::SetKey(int index, double t, const point& p)
{
	if (index >= keys.size())
		std::cout << "Tried to set key position above nKeys = " << keys.size() << "; Command ignored.\n";

	keys[index].t = t;
	keys[index].p = p;

	SortKeys();
	RebuildParamDistanceTable();
}

int Interpolator::InsertKey(double t, const point& p)
{
	/*if (nKeys == MAX_KEYS)
		std::cout << "Cannot add more than " << MAX_KEYS << " to interpolator; Command ignored.\n";*/

	keys.emplace_back(t, p);
	/*keys[nKeys].t = t;
	keys[nKeys].p = p;
	nKeys++;*/

	SortKeys();
	RebuildParamDistanceTable();

	return FindKey(t);
}

// Deletes the keys it encounters whose time is within
// "tolerance" of t.  If "all" = false then it will only
// delete the first it finds.
void Interpolator::DeleteKey(double t, double tolerance, bool all)
{
	/*for (int i = 0; i < nKeys; i++)
	{
		if (fabs(keys[i].t - t) < tolerance)
		{
			for (int j = i + 1; j < nKeys; j++)
				keys[j - 1] = keys[j];
			nKeys--;
			if (all == false)
				break;
		}
	}*/
	if (all == true)
		keys.erase(std::remove_if(keys.begin(), keys.end(), [t, tolerance](const auto& k) { return fabs(k.t - t) < tolerance; }), keys.end());
	else
		keys.erase(std::find_if(keys.begin(), keys.end(), [t, tolerance](const auto& k) { return fabs(k.t - t) < tolerance; }));
	RebuildParamDistanceTable();
}

void Interpolator::DeleteKey(int index)
{
	if (index < keys.size())
	{
		keys.erase(keys.begin() + index);
	}
	RebuildParamDistanceTable();
}

void Interpolator::RebuildParamDistanceTable(void)
{
	if (keys.size() == 0)
		return;

	if (distTable != nullptr)
		delete[] distTable;

	distTable = new ParamDistEntry[100 * (keys.size() - 1) + 1];

	distTable[0].d = 0;
	distTable[0].s = 0;
	if ((keys.size() - 1) > 0)
	{
		point prev, next;
		prev = keys[0].p;
		double step, d, cur;
		int pos = 1, i = 0;
		step = (keys[i + 1].t - keys[i].t) / 100;
		cur = step;
		while (pos < (100 * (keys.size() - 1) + 1))
		{
			if ((pos % 100) == 0) // to recompute the step if needed
			{
				i++;
				step = (keys[i + 1].t - keys[i].t) / 100;
			}
			next = evaluate(cur);
			d = dist(next, prev);
			distTable[pos].d = distTable[pos - 1].d + d;
			distTable[pos].s = cur;
			pos++;
			cur += step;
			prev = next;
		}
	}
	for (int i = 0; i < keys.size(); i++)
		keys[i].dist = distTable[i * 100].d;

}
