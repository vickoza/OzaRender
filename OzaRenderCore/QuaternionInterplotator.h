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

class slerp : public QuaternionInterplotator
{
public:

	virtual quaternion Evaluate(double t);

};

class bezierQuaternion : public QuaternionInterplotator
{
	//used to help find a quaternion
private:
	quaternion slerpM(quaternion q1, quaternion q2, double t);
	quaternion squad(quaternion s1, quaternion s2, quaternion a, quaternion b, double t);
	quaternion getQuaternion(quaternion s1, quaternion s2, quaternion s3);
public:
	virtual quaternion Evaluate(double t);

};
QuaternionInterplotator* QuaternionInterplotator::LoadQuaternionInterpolator(Loader& input)
{
	char type[256];
	QuaternionInterplotator* result = nullptr;

	input.ReadToken(type);
	if (strcmp(type, "slerp") == 0)
		result = new slerp;
	else if (strcmp(type, "bezier") == 0)
		result = new bezierQuaternion;
	else
		input.Error(type, "Unknown quaternionernion interpolator type");

	result->Load(input);
	return result;
}

void QuaternionInterplotator::Load(Loader& input)
{
	char token[256], tmp[256];
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

		do
		{
			input.ReadToken(token);

			if (strncmp(token, "val", 3) == 0)
			{
				input.ReadToken(tmp);
				if (strncmp(tmp, "quaternion", 4) == 0)
				{
					if (!input.ReadDouble(keys[i].q.w))
						input.Error("A quaternionernion key value must begin with a scalar 'w' value");

					for (int j = 0; j < 3; j++)
						if (!input.ReadDouble(keys[i].q.v[j]))
							input.Error("Not enough coordinates for quaternionernion key vector");

					keys[i].q.normalize(); // Make it a unit quaternionernion
				}
				else if (strncmp(tmp, "angleaxis", 9) == 0)
				{
					double angle;
					svector v;

					if (!input.ReadDouble(angle))
						input.Error("An angle-axis value for a quaternionernion key must begin with an angle\n");

					for (int j = 0; j < 3; j++)
						if (!input.ReadDouble(v[j]))
							input.Error("Not enough coordinates for quaternionernion key vector");
					keys[i].q.QuaternionFromAngleAxisD(angle, v);
				}
				else if (strncmp(tmp, "euler", 5) == 0)
				{
					svector v;

					for (int j = 0; j < 3; j++)
					{
						if (!input.ReadDouble(v[j]))
							v[j] = 0.0;  // If we don't get enough, set the rest to 0.
					}

					keys[i].q.QuaternionFromEulerAnglesD(v[0], v[1], v[2]);
				}
				else input.Error("Unknown quaternionernion key value type: %s\n", tmp);
			}
		} while (strcmp(token, "key") != 0 && strcmp(token, "end") != 0);

		if (strcmp(token, "end") == 0 && i != keys.size() - 1)
			input.Error("End reached before all keys were found");
	}
}
quaternion bezierQuaternion::slerpM(quaternion q1, quaternion q2, double t)
{
	double costheta = dot(q1, q2);
	double theta = acos(costheta);
	double sinTheta = sqrt(1.0 - costheta * costheta);
	if (abs(theta) < 0.01)
		return q1;
	if (abs(sinTheta) < 0.01)
		return .5 * (q1 + q2);
	double ratioA = sin((1 - t) * theta) / sinTheta;
	double ratioB = sin(t * theta) / sinTheta;
	return ((ratioA * q1) + (ratioB * q2));
}
quaternion bezierQuaternion::Evaluate(double t)
{
	if (t < keys[0].t)
		return keys[0].q;
	else if (t > keys.back().t)
		return keys.back().q;
	int i = FindInterval(t);
	double p = (t - keys[i].t) / (keys[i + 1].t - keys[i].t);
	if (keys.size() == 2)
		return slerpM(keys[0].q, keys[1].q, p);
	if (keys.size() == 3)
	{
		quaternion a = getQuaternion(keys[0].q, keys[1].q, keys[2].q);
		return squad(keys[i].q, keys[i + 1].q, a, a, p);
	}
	if (i == 0)
	{
		quaternion a = getQuaternion(keys[0].q, keys[0].q, keys[1].q);
		quaternion b = getQuaternion(keys[0].q, keys[1].q, keys[2].q);
		return squad(keys[i].q, keys[i + 1].q, a, b, p);
	}
	if (i == (keys.size() - 2))
	{
		quaternion a = getQuaternion(keys[keys.size() - 3].q, keys[keys.size() - 2].q, keys.back().q);
		quaternion b = getQuaternion(keys[keys.size() - 2].q, keys.back().q, keys.back().q);
		return squad(keys[i].q, keys[i + 1].q, a, b, p);
	}
	quaternion a = getQuaternion(keys[i - 1].q, keys[i].q, keys[i + 1].q);
	quaternion b = getQuaternion(keys[i].q, keys[i + 1].q, keys[i + 2].q);
	return squad(keys[i].q, keys[i + 1].q, a, b, p);

}
quaternion bezierQuaternion::squad(quaternion s1, quaternion s2, quaternion b1, quaternion b2, double t)
{
	quaternion temp1, temp2;
	temp1 = slerpM(b1, b2, t);
	temp2 = slerpM(s1, s2, t);
	return slerpM(temp1, temp2, t);
}
quaternion slerp::Evaluate(double t)
{

	// Find the interval that t is on
	// 
	// Calculate the percentage that t is along that interval
	// 
	// Use that percentage to calculate the slerp between the 
	// two quaternions.
	//
	// Be careful of division by 0 in the slerp formula!
	if (t < keys[0].t)
		return keys[0].q;
	else if (t > keys.back().t)
		return keys.back().q;
	int i = FindInterval(t);
	double p = (t - keys[i].t) / (keys[i + 1].t - keys[i].t);

	double costheta = dot(keys[i].q, keys[i + 1].q);
	double theta = acos(costheta);
	double sinTheta = sqrt(1.0 - costheta * costheta);
	if (abs(theta) < 0.01)
		return keys[i].q;
	if (abs(sinTheta) < 0.01)
		return .5 * (keys[i].q + keys[i + 1].q);
	double ratioA = sin((1 - p) * theta) / sinTheta;
	double ratioB = sin(p * theta) / sinTheta;
	return ((ratioA * keys[i].q) + (ratioB * keys[i + 1].q));
}
quaternion bezierQuaternion::getQuaternion(quaternion s1, quaternion s2, quaternion s3)
{
	double th0, th1, th2, a;
	th0 = s1.angleR();
	th1 = s2.angleR();
	th2 = s3.angleR();
	a = (th0 - 6 * th1 + th2) / 4;
	quaternion q(0.0, -1.0);
	return sin(a) * q;
}

