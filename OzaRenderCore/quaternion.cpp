#include "quaternion.h"
#include "matrix.h"

double quaternion::length() const
{
	return sqrt(sqr(w) + sqr(v.length()));
}

void quaternion::normalize()
{
	double len = length(); w /= len; v /= len;
}

quaternion quaternion::conj(void) const
{
	return quaternion(w, -v);
}

quaternion quaternion::inverse(void) const
{
	double len = length();
	quaternion inv = conj();
	inv.w /= (len * len);
	inv.v /= (len * len);
	return inv;
}

double quaternion::scale(void) const
{
	return length();
}

double quaternion::angleR(void) const
{
	// To do 
	//
	// Calculate the "angle" from a rotation quaternionernion.
	// I.e. the angle that it would rotate a vector about its
	// axis.
	return acos(w / length()) * 2;
}

double quaternion::angleD(void) const
{
	// To do 
	//
	// Calculate the "angle" from a rotation quaternionernion.
	// I.e. the angle that it would rotate a vector about its
	// axis.
	return acos(w / length()) * 360 / PI;
}

svector quaternion::unitV(void) const
{
	// To do
	//
	// Return the axis of a rotation quaternionernion as a unit vector
	// Be careful of trying to normalize a 0 vector.
	auto temp = v;
	temp.normalize();
	return temp;
}

quaternion& quaternion::operator+=(const quaternion& r)
{
	// To Do
	// 
	// Add The quaternionernion r to this quaternionernion.  Remember
	// that the sum of two quaternionernions is just computed componentwise.
	w += r.w;
	v += r.v;
	return *this;
}

quaternion& quaternion::operator-=(const quaternion& r)
{
	// To Do
	// 
	// The same with the subtraction
	w -= r.w;
	v -= r.v;
	return *this;
}

quaternion& quaternion::operator*=(const quaternion& r)
{
	// To Do 
	//
	// Multiply this quaternionerion by r
	double temp = w;
	w = temp * r.w - (dot(v, r.v));
	v = (temp * r.v) + (r.w * v) + cross(v, r.v);
	return *this;
}

quaternion& quaternion::operator/=(const quaternion& r)
{
	(*this) *= r.inverse();
	return *this;
}

svector quaternion::rotate(svector v) const
{
	// To Do
	// 
	// Rotate the vector v by this quaternionernion and return
	// the rotated vector.
	if (v.length() > 0)
	{
		quaternion temp = ((*this) * quaternion(0, v));
		return temp.v;
	}
	else
		return v;
}

void quaternion::QuaternionFromAngleAxisR(double a[4])
{
	// To Do
	// 
	// If this quaternionernion is a unit quaternionernion then
	// compute the quaternionernion defined by the angle axis
	// the angle is in the first coordinate of the array
	// and the axis in the last three
	svector temp(a[1], a[2], a[3]);
	temp.normalize();
	w = cos(a[0] / 2);
	v = sin(a[0] / 2) * temp;
}

void quaternion::QuaternionFromAngleAxisR(double theta, svector V)
{
	// To Do
	//
	// The same thing but with the angle and the axis defined
	// separately.
	auto temp = V;
	temp.normalize();
	w = cos(theta / 2);
	v = sin(theta / 2) * temp;
}

void quaternion::QuaternionToAngleAxisR(double a[4]) const
{
	// To Do
	//
	// Assuming that this is a unit quaternionernion, convert it
	// to an angle-axis form.  Again, the angle should go 
	// into the first coordinate and the axis into the last
	// three
	a[0] = acos(w / length()) * 2;
	auto temp = v;
	temp.normalize();
	a[1] = temp[0];
	a[2] = temp[1];
	a[3] = temp[2];
}

void quaternion::QuaternionToAngleAxisR(double& theta, svector& V) const
{
	// To Do
	//
	// Assuming that this is a unit quaternionernion, convert it
	// to an angle-axis form.
	if (length() > 0)
	{
		double temp = w / length();
		theta = acos(temp) * 2;
		if ((temp > -1) && (temp < 1))
		{
			auto temp1 = v;
			temp1.normalize();
			V = temp;
		}
		else
			V = svector{ 0, 0, 0 };
	}
}

void quaternion::QuaternionFromEulerAnglesR(double x, double y, double z)
{
	// To Do
	// 
	// Convert the given Euler-angles to a quaternion.
	(*this) = quaternion((x / 2), svector{1.0, 0.0, 0.0}) * quaternion((y / 2), svector{0.0, 1.0, 0.0}) * quaternion((z / 2), svector{0.0, 0.0, 1.0});
}

void quaternion::QuaternionToEulerAnglesR(double& x, double& y, double& z)
{
	// Convert the given Euler-angles to a quaternion.
	matrix m;
	double a = acos(w / length()) * 360 / PI;
	auto ax = v;
	ax.normalize();
	m.MakeRotation(a, ax);
	double temp = m.getElement(2, 0);
	if (abs(temp) < 1)
	{
		x = atan(m.getElement(1, 2) / m.getElement(2, 2));
		y = atan((-m.getElement(0, 2) * cos(x)) / m.getElement(2, 2));
		z = atan(m.getElement(1, 0) / m.getElement(1, 1));
	}
	else
	{
		x = 0;
		y = asin(temp);
		z = atan(-m.getElement(1, 2) / m.getElement(1, 1));
	}
}
void quaternion::QuaternionFromAngleAxisD(double a[4])
{
	// If this quaternionernion is a unit quaternionernion then
	// compute the quaternionernion defined by the angle axis
	// the angle is in the first coordinate of the array
	// and the axis in the last three
	svector temp(a[1], a[2], a[3]);
	temp.normalize();
	w = cos(a[0] * PI / 360);
	v = sin(a[0] * PI / 360) * temp;
}

void quaternion::QuaternionFromAngleAxisD(double theta, svector V)
{
	// The same thing but with the angle and the axis defined
	// separately.
	auto temp = V;
	temp.normalize();
	w = cos(theta * PI / 360);
	v = sin(theta * PI / 360) * temp;
}

void quaternion::QuaternionToAngleAxisD(double a[4]) const
{
	// Assuming that this is a unit quaternionernion, convert it
	// to an angle-axis form.  Again, the angle should go 
	// into the first coordinate and the axis into the last
	// three
	a[0] = acos(w / length()) * 360 / PI;
	auto temp = v;
	temp.normalize();
	a[1] = temp[0];
	a[2] = temp[1];
	a[3] = temp[2];
}

void quaternion::QuaternionToAngleAxisD(double& theta, svector& V) const
{
	// Assuming that this is a unit quaternionernion, convert it
	// to an angle-axis form.
	if (length() > 0)
	{
		double temp = w / length();
		theta = acos(temp) * 360 / PI;
		if ((temp > -1) && (temp < 1))
		{
			auto temp1 = v;
			temp1.normalize();
			V = temp;
		}
		else
			V = svector(0, 0, 0);
	}
}

void quaternion::QuaternionFromEulerAnglesD(double x, double y, double z)
{
	// Convert the given Euler-angles to a quaternion.
	(*this) = quaternion((x * PI / 360), svector{1.0, 0.0, 0.0}) * quaternion((y * PI / 360), svector{0.0, 1.0, 0.0}) * quaternion((z * PI / 360), svector{0.0, 0.0, 1.0});
}

void quaternion::QuaternionToEulerAnglesD(double& x, double& y, double& z)
{
	// Convert the given Euler-angles to a quaternion.
	matrix m;
	double a = acos(w / length()) * 360 / PI;
	svector ax = v;
	ax.normalize();
	m.MakeRotation(a, ax);
	double temp = m.getElement(2, 0);
	if (abs(temp) < 1)
	{
		x = atan(m.getElement(1, 2) / m.getElement(2, 2)) * 180 / PI;
		y = atan((-m.getElement(0, 2) * cos(x)) / m.getElement(2, 2) * 180 / PI);
		z = atan(m.getElement(1, 0) / m.getElement(1, 1)) * 180 / PI;
	}
	else
	{
		x = 0;
		y = asin(temp) * 180 / PI;
		z = atan(-m.getElement(1, 2) / m.getElement(1, 1)) * 180 / PI;
	}
}


quaternion operator-(const quaternion& q)
{
	return quaternion(-q.w, -q.v);
}

quaternion operator+(const quaternion& q1, const quaternion& q2)
{
	return quaternion((q1.w + q2.w), (q1.v + q2.v));
}

quaternion operator-(const quaternion& q1, const quaternion& q2)
{
	return quaternion((q1.w - q2.w), (q1.v - q2.v));
}

quaternion operator*(const quaternion& q1, const quaternion& q2)
{
	return quaternion((q1.w * q2.w - dot(q1.v, q2.v)), ((q2.w * q1.v) + (q1.w * q2.v) + cross(q1.v, q2.v)));
}

quaternion operator/(const quaternion& q1, const quaternion& q2)
{
	quaternion tmp = q2.inverse();
	return quaternion((q1.w * tmp.w - dot(q1.v, tmp.v)), ((tmp.w * q1.v) + (q1.w * tmp.v) + cross(q1.v, tmp.v)));
}

double dot(const quaternion& a, const quaternion& b)
{
	// To Do
	//
	// Calculate the 4-D dot product of the two quaternionernions.
	return (a.w * b.w + dot(a.v, b.v));
}

std::ostream& operator<<(std::ostream& output, const quaternion& q)
{
	output << "(" << q.w << ", <" << q.v[0] << ", " << q.v[1] << ", " << q.v[2] << ">)";
	return output;
}


int QuaternionInterp::FindInterval(double t)
{
	// To Do
	//
	// Again, perform a binary search for the key at the
	// beginning of the interval containing t
	if (nKeys == 0 || nKeys == 1)
		return 0;
	else if (t < keys[0].t)	// Off the beginning
		return 0;
	else if (t > keys[nKeys - 1].t) // Off the end
		return nKeys - 2;

	//  Binary Serach
	int a = 0, b = nKeys - 1;

	while ((b - a) > 1)
	{
		if (t > keys[(a + b) / 2].t)
			a = (a + b) / 2;
		else
			b = (a + b) / 2;
	}
	return a;
}

QuaternionInterp* QuaternionInterp::LoadQuaternionInterpolator(Loader& input)
{
	char type[256];
	QuaternionInterp* result = NULL;

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

void QuaternionInterp::Load(Loader& input)
{
	char token[256], tmp[256];

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
					vector v;

					if (!input.ReadDouble(angle))
						input.Error("An angle-axis value for a quaternionernion key must begin with an angle\n");

					for (int j = 0; j < 3; j++)
						if (!input.ReadDouble(v[j]))
							input.Error("Not enough coordinates for quaternionernion key vector");
					keys[i].q.QuaternionFromAngleAxisD(angle, v);
				}
				else if (strncmp(tmp, "euler", 5) == 0)
				{
					vector v;

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

		if (strcmp(token, "end") == 0 && i != nKeys - 1)
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
	else if (t > keys[nKeys - 1].t)
		return keys[nKeys - 1].q;
	int i = FindInterval(t);
	double p = (t - keys[i].t) / (keys[i + 1].t - keys[i].t);
	if (nKeys == 2)
		return slerpM(keys[0].q, keys[1].q, p);
	if (nKeys == 3)
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
	if (i == (nKeys - 2))
	{
		quaternion a = getQuaternion(keys[nKeys - 3].q, keys[nKeys - 2].q, keys[nKeys - 1].q);
		quaternion b = getQuaternion(keys[nKeys - 2].q, keys[nKeys - 1].q, keys[nKeys - 1].q);
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
	else if (t > keys[nKeys - 1].t)
		return keys[nKeys - 1].q;
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
