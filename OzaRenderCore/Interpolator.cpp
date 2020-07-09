#include <algorithm>
#include <iostream>
#include "Interpolator.h"
#include "LinearInterp.h"
#include "HermiteInterp.h"
#include "CatmullRomInterp.h"
#include "TCBInterp.h"

Interpolator* Interpolator::LoadInterpolator(Loader& input)
{
	char type[256];
	Interpolator* result = NULL;

	input.ReadToken(type);
	if (strcmp(type, "linear") == 0)
		result = new LinearInterp;
	else if (strcmp(type, "hermite") == 0)
		result = new HermiteInterp;
	else if (strncmp(type, "catmull", 7) == 0)
		result = new CatmullRomInterp;
	else if (strncmp(type, "tcb", 3) == 0)
		result = new TCBInterp;
	else
		input.Error(type, "Unknown interpolator type");

	result->Load(input);
	return result;
}

void Interpolator::Load(Loader& input)
{
	char token[256];
	int nKeys;

	if (!input.ReadInt(nKeys))
		input.Error("Interpolator object must begin with a key count");

	input.ReadToken(token);
	for (int i = 0; i < nKeys; i++)
	{
		if (strcmp(token, "key") != 0)
			input.Error("Each key must begin with the keyword 'key'");

		if (!input.ReadDouble(keys[i].t))
			input.Error("Key %d must begin with a time", i);

		keys[i].v0 = keys[i].v1 = svector(0, 0, 0);
		keys[i].tcb = svector(0, 0, 0);
		keys[i].constant = false;
		keys[i].easeIn = 0.0;
		keys[i].easeOut = 0.0;

		do
		{
			input.ReadToken(token);

			if (strncmp(token, "val", 3) == 0 || strncmp(token, "pos", 3) == 0)
			{
				for (int j = 0; j < 3; j++)
					if (!input.ReadDouble(keys[i].p[j]))
						input.Error("Not enough coordinates for key %d", i);

				// See if there is a homogeneous coordinate, if not, then 1.0
				if (!input.ReadDouble(keys[i].p[3]))
					keys[i].p[3] = 1.0;

				keys[i].p.normalize(); // Make its homogeneous coordinate 1.0
			}
			else if (strcmp(token, "constant") == 0)
				keys[i].constant = true;
			else if (strcmp(token, "easein") == 0)
			{
				if (!input.ReadDouble(keys[i].easeIn))
					input.Error("The value for a key's ease in must be a floating point value (preferrably between -1 and 1");
			}
			else if (strcmp(token, "easeout") == 0)
			{
				if (!input.ReadDouble(keys[i].easeOut))
					input.Error("The value for a key's ease out must be a floating point value (preferrably between -1 and 1");
			}
			else if (strcmp(token, "key") == 0)
			{
				continue;
			}
			else
			{
				// Try to see if the derived class recognizes the 
				// key's attribute.
				if (!ReadKeyAttribute(input, i, token))
				{
					if (i == nKeys - 1)
						break; // Go on to process attributes
					else
						input.Error(token, "Unknown token '%s' for Interpolator key %d", token, i);
				}
			}
		} while (strcmp(token, "key") != 0 && strcmp(token, "end") != 0);

		if (strcmp(token, "end") == 0 && i != nKeys - 1)
			input.Error("End reached before all keys were found");
	}

	if (strcmp(token, "end") != 0)
	{
		do
		{
			if (!ReadAttribute(input, token))
				input.Error(token, "Invalid attribute '%s' for interpolator object", token);
			input.ReadToken(token);
		} while (strcmp(token, "end") != 0);
	}

	RebuildParamDistanceTable();
}

point Interpolator::Evaluate(double t)   // This evaluation method lets us step in and do constant velocity movement if we want to
{
	int i = FindInterval(t);

	if (!keys[i].constant || (i == (keys.size() - 1)))
		return evaluate(t);  // If the motion is not constant velocity then just evaluate the interpolator at time t
	else
	{
		double s = 0, d;

		// If it is constant then we need to move along the curve at a constant velocity ... but what 
		// velocity.  The total time between this key and the next key is the difference between their
		// time values, and the distance traveled is the difference between their "dist" values (which 
		// are calculated in the "RebuildParamDistanceTable" method.  Therefore the constant velocity 
		// used on the segment must be the total distance divided by the total time.
		// 
		// Calculate a value for s between keys[i].t and keys[i+1].t
		if (FindKey(t) > -1)
			s = t;
		else
		{
			int a = i * 100, b = (i + 1) * 100;
			// finds the distant the object such travel at time t
			d = keys[i].dist + ((keys[i + 1].dist - keys[i].dist) * (t - keys[i].t) / (keys[i + 1].t - keys[i].t));
			while ((b - a) > 1) //find the nearest entery on the distTable
			{
				if (d > distTable[(a + b) / 2].d)
					a = (a + b) / 2;
				else
					b = (a + b) / 2;
			}
			// computes s using the distTable and find the right s value for the d value
			s = distTable[a].s + ((distTable[a + 1].s - distTable[a].s) * (d - distTable[a].d) / (distTable[a + 1].d - distTable[a].d));
		}

		return (evaluate(s));
	}
}

svector Interpolator::Velocity(double t, double h)
{
	// Calculate the velocity of the object at t using a stepsize of h
	// if it is calculated numerically

	return .5 * (evaluate(t + h) - evaluate(t - h));
}

svector Interpolator::Up(double t, double h)
{
	// Calculate the Up vector of the object at t using a stepsize of h

	// t is before the first computate h then it sets the up vector to 1 in z 
	if (t < (keys[0].t + h))
		return svector(0.0, 0.0, 1.0);
	// t is after the last computable t then it look for the up vector for the last computable up vector
	else if (t > (keys.back().t - h))
		return Up((keys.back().t - h), h);
	double test;
	svector v1 = evaluate(t + h) - evaluate(t);
	svector v2 = evaluate(t) - evaluate(t - h);
	v1.normalize();
	v2.normalize();
	test = dot(v1, v2);
	if ((test < 1) && (test > -1))
		return cross(v1, v2);// up vector to the point  
	else
		return Up(t - h, h);// find previous computable up vector if points are linear
}

svector Interpolator::Acceleration(double t, double h)
{
	// Calclulate the acceleration at t using a stepsize of h
	// if it is calculated numerically

	return .5 * (Velocity(t + h, h) - Velocity(t - h, h));
}

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
