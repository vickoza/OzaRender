#include <iostream>
#include "TCBInterp.h"

void TCBInterp::RecalculateVectors(void)
{
	double t, c, b;
	svector v1, v2;
	/*
		add tension, continuity, and bias to the first and last keys
	*/
	v1 = 3 * (keys[1].p - keys[0].p); // outgoing vector for the frist key
	v2 = -1 * (keys[2].p - keys[1].p); // incoming vector for the frist key
	t = GetTension(0);
	c = GetContinuity(0);
	b = GetBias(0);
	keys[0].v0 = (1 - t) * .5 * (((1 - c) * (1 + b) * v1) + ((1 + c) * (1 - b) * v2)); // sets the curve vector for the first key
	v1 = 3 * (keys.back().p - keys[keys.size() - 2].p); // incoming vector for the last key
	v2 = -1 * (keys[keys.size() - 2].p - keys[keys.size() - 3].p); // outgoing vector for the last key
	t = GetTension(keys.size() - 1);
	c = GetContinuity(keys.size() - 1);
	b = GetBias(keys.size() - 1);
	keys.back().v1 = (1 - t) * .5 * (((1 + c) * (1 + b) * v1) + ((1 - c) * (1 - b) * v2)); // sets the curve vector for the last key
	/*
		add tension, continuity, and bias to the middle keys
	*/
	for (int i = 1; i < keys.size() - 1; i++)
	{
		t = GetTension(i);
		c = GetContinuity(i);
		b = GetBias(i);
		v1 = keys[i].p - keys[i - 1].p; // incoming vector to the key i
		v2 = keys[i + 1].p - keys[i].p; // outgoing vector to the key i
		keys[i].v0 = (1 - t) * .5 * (((1 - c) * (1 + b) * v1) + ((1 + c) * (1 - b) * v2)); // outgoing velocity vector
		keys[i].v1 = (1 - t) * .5 * (((1 + c) * (1 + b) * v1) + ((1 - c) * (1 - b) * v2)); // incoming velocity vector
	}

	RebuildParamDistanceTable();

	RebuildParamDistanceTable();
}

void TCBInterp::SetValue(int index, const point& p)
{
	HermiteInterp::SetValue(index, p);
	RecalculateVectors();
}

void TCBInterp::SetTime(int index, double t)
{
	HermiteInterp::SetTime(index, t);
	RecalculateVectors();
}

void TCBInterp::SetKey(int index, double t, const point& p)
{
	HermiteInterp::SetKey(index, t, p);
	RecalculateVectors();
}

int TCBInterp::InsertKey(double t, const point& p)
{
	int keyIndex = HermiteInterp::InsertKey(t, p);
	RecalculateVectors();

	return keyIndex;
}

void TCBInterp::DeleteKey(double t, double tolerance, bool all)
{
	HermiteInterp::DeleteKey(t, tolerance, all);
	RecalculateVectors();
}

void TCBInterp::DeleteKey(int index)
{
	HermiteInterp::DeleteKey(index);
	RecalculateVectors();
}

void TCBInterp::SetTension(int index, double val)
{
	if (index < 0)
	{
		std::cout << "Tried to set key tension below nKeys = 0 ; Command ignored.\n";
		return;
	}
	if (index >= keys.size())
	{
		std::cout << "Tried to set key tension above nKeys = " << keys.size() << "; Command ignored.\n";
		return;
	}

	keys[index].tcb[0] = val;
	RecalculateVectors();
}

void TCBInterp::SetBias(int index, double val)
{
	if (index < 0)
	{
		std::cout << "Tried to set key tension below nKeys = 0 ; Command ignored.\n";
		return;
	}
	if (index >= keys.size())
	{
		std::cout << "Tried to set key bias above nKeys = " << keys.size() << "; Command ignored.\n";
		return;
	}

	keys[index].tcb[2] = val;
	RecalculateVectors();
}

void TCBInterp::SetContinuity(int index, double val)
{
	if (index < 0)
	{
		std::cout << "Tried to set key tension below nKeys = 0 ; Command ignored.\n";
		return;
	}
	if (index >= keys.size())
	{
		std::cout << "Tried to set key continuity above nKeys = " << keys.size() << "; Command ignored.\n";
		return;
	}

	keys[index].tcb[1] = val;
	RecalculateVectors();
}

void TCBInterp::Load(Loader& input)
{
	Interpolator::Load(input);
	RecalculateVectors();
	RebuildParamDistanceTable();
}

bool TCBInterp::ReadKeyAttribute(Loader& input, int nKey, char* attribute)
{
	if (strcmp(attribute, "tension") == 0)
	{
		if (!input.ReadDouble(keys[nKey].tcb[0]))
			input.Error("Tension command in TCB spline must be followed by a value.");
		return true;
	}
	else if (strcmp(attribute, "bias") == 0)
	{
		if (!input.ReadDouble(keys[nKey].tcb[1]))
			input.Error("Bias command in TCB spline must be followed by a value.");
		return true;
	}
	else if (strcmp(attribute, "continuity") == 0)
	{
		if (!input.ReadDouble(keys[nKey].tcb[2]))
			input.Error("Continuity command in TCB spline must be followed by a value.");
		return true;
	}
	else if (strcmp(attribute, "tcb") == 0)
	{
		for (int i = 0; i < 3; i++)
			if (!input.ReadDouble(keys[nKey].tcb[i]))
				input.Error("TCB command in TCB spline must be followed by three values.\nOnly %d values found.", i);
		return true;
	}
	return false;
}