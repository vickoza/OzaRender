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
