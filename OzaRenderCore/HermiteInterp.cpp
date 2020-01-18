#include <iostream>
#include "HermiteInterp.h"

void HermiteInterp::SetKeyInVector(int index, svector vin)
{
	if (index >= keys.size())
	{
		std::cout << "Tried to set key in vector above nKeys = " << keys.size() << "; Command ignored.\n";
		return;
	}

	keys[index].v0 = vin;
	RebuildParamDistanceTable();
}

void HermiteInterp::SetKeyOutVector(int index, svector vout)
{
	if (index >= keys.size())
	{
		std::cout << "Tried to set key position above nKeys = " << keys.size() << "; Command ignored.\n";
		return;
	}

	keys[index].v1 = vout;
	RebuildParamDistanceTable();
}

void HermiteInterp::SetKeyVector(int index, svector v)
{
	if (index >= keys.size())
	{
		std::cout << "Tried to set key position above nKeys = " << keys.size() << "; Command ignored.\n";
		return;
	}

	keys[index].v0 = keys[index].v1 = v;
	RebuildParamDistanceTable();
}

void HermiteInterp::RebuildParamDistanceTable(void)
{
	averageDT = 0;
	for (int i = 0; i < keys.size() - 1; i++)
		averageDT += (keys[i + 1].t - keys[i].t);
	averageDT /= keys.size() - 1;

	Interpolator::RebuildParamDistanceTable();
}

point HermiteInterp::evaluate(double t)
{
	if (keys.size() == 0)
		return point(0, 0, 0);
	if (keys.size() == 1)
		return keys[0].p;
	if (t < keys[0].t)
		return keys[0].p;
	if (t > keys.back().t)
		return keys.back().p;

	int i = FindInterval(t);

	point result;
	double s;
	s = (t - keys[i].t) / (keys[i + 1].t - keys[i].t);
	result = (1 - (3 * s * s) + (2 * s * s * s)) * keys[i].p + ((3 * s * s) - (2 * s * s * s)) * keys[i + 1].p + ((s * s * s) - (2 * s * s) + s) * keys[i].v0 + ((s * s * s) - (s * s)) * keys[i + 1].v1;

	return result;
}

bool HermiteInterp::ReadKeyAttribute(Loader& input, int nKey, char* attribute)
{
	int j;

	if (strcmp(attribute, "vin") == 0 || strcmp(attribute, "v0") == 0)
	{
		for (j = 0; j < 3; j++)
			if (!input.ReadDouble(keys[nKey].v0[j]))
				input.Error("Not enough coordinates for v0 in key %d", nKey);
		return true;
	}
	else if (strcmp(attribute, "vout") == 0 || strcmp(attribute, "v1") == 0)
	{
		for (j = 0; j < 3; j++)
			if (!input.ReadDouble(keys[nKey].v1[j]))
				input.Error("Not enough coordinates for v1 in key %d", nKey);
		return true;
	}
	else if (strcmp(attribute, "v") == 0)
	{
		for (j = 0; j < 3; j++)
			if (!input.ReadDouble(keys[nKey].v0[j]))
				input.Error("Not enough coordinates for vector 'v' in key %d", nKey);
		keys[nKey].v1 = keys[nKey].v0;
		return true;
	}
	return false;  // Didn't recognize it
}
/*
void HermiteInterp::RebuildParamDistanceTable()
{
	averageDT = 0;
	for (int i = 0; i < keys.size() - 1; i++)
		averageDT += (keys[i + 1].t - keys[i].t);
	averageDT /= keys.size() - 1;

	Interpolator::RebuildParamDistanceTable();
}
*/