#include "QuaternionInterplotator.h"
#include "slerp.h"
#include "bezierQuaternion.h"

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
