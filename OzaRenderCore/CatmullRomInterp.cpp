#include "CatmullRomInterp.h"

void CatmullRomInterp::RecalculateVectors(void)
{
	double d1, d2;
	svector v1, v2;
	switch (begin)
	{
	case endAction::stop:
		// set begining key ve
		keys[0].v0 = keys[0].v1 = svector(0.0);
		break;
	case endAction::linear:
		keys[0].v0 = keys[0].v1 = keys[1].p - keys[0].p;
		break;
	case endAction::quadratic:
		keys[0].v0 = keys[0].v1 = .5 * ((3 * (keys[1].p - keys[0].p)) - (keys[2].p - keys[1].p));
		break;
	//default:
		//complain("Invalid beginning condition for catmul rom: %d", begin); //HACK
	};

	for (int i = 1; i < keys.size() - 1; i++)
	{
		v1 = keys[i].p - keys[i - 1].p;
		v2 = keys[i + 1].p - keys[i].p;
		d1 = v1.length();
		d2 = v2.length();
		keys[i].v0 = keys[i].v1 = ((d2 / (d1 + d2)) * v1 + (d1 / (d1 + d2)) * v2);
	}

	switch (end)
	{
	case endAction::stop:
		keys.back().v0 = keys.back().v1 = svector(0.0);
		break;
	case endAction::linear:
		keys.back().v0 = keys.back().v1 = keys.back().p - keys[keys.size() - 2].p;
		break;
	case endAction::quadratic:
		keys.back().v0 = keys.back().v1 = .5 * ((3 * (keys.back().p - keys[keys.size() - 2].p)) - (keys[keys.size() - 2].p - keys[keys.size() - 3].p));
		break;
	//default:
		//complain("Invalid beginning condition for catmul rom: %d", begin); //HACK
	};

	RebuildParamDistanceTable();
}

void CatmullRomInterp::SetValue(int index, const point& p)
{
	HermiteInterp::SetValue(index, p);
	RecalculateVectors();
}

void CatmullRomInterp::SetTime(int index, double t)
{
	HermiteInterp::SetTime(index, t);
	RecalculateVectors();
}

void CatmullRomInterp::SetKey(int index, double t, const point& p)
{
	HermiteInterp::SetKey(index, t, p);
	RecalculateVectors();
}

int CatmullRomInterp::InsertKey(double t, const point& p)
{
	int newKey = HermiteInterp::InsertKey(t, p);
	RecalculateVectors();

	return newKey;
}

void CatmullRomInterp::DeleteKey(double t, double tolerance, bool all)
{
	HermiteInterp::DeleteKey(t, tolerance, all);
	RecalculateVectors();
}

void CatmullRomInterp::DeleteKey(int index)
{
	HermiteInterp::DeleteKey(index);
	RecalculateVectors();
}

void CatmullRomInterp::Load(Loader& input)
{
	Interpolator::Load(input);
	RecalculateVectors();
	RebuildParamDistanceTable();
}

bool CatmullRomInterp::ReadAttribute(Loader& input, char* attribute)
{
	char token[256];
	if (strcmp(attribute, "begincondition") == 0 || strcmp(attribute, "startcondition") == 0)
	{
		input.ReadToken(token);
		if (strcmp(token, "stop") == 0)
			begin = endAction::stop;
		else if (strcmp(token, "linear") == 0)
			begin = endAction::linear;
		else if (strcmp(token, "quadratic") == 0)
			begin = endAction::quadratic;
		else
			input.Error(token, "Unrecognized begin condition '%s' for Catmull Rom Interpolator", token);

		return true;
	}
	if (strcmp(attribute, "endcondition") == 0)
	{
		input.ReadToken(token);
		if (strcmp(token, "stop") == 0)
			end = endAction::stop;
		else if (strcmp(token, "linear") == 0)
			end = endAction::linear;
		else if (strcmp(token, "quadratic") == 0)
			end = endAction::quadratic;
		else
			input.Error(token, "Unrecognized end condition '%s' for Catmull Rom Interpolator", token);

		return true;
	}
	return false;
}

