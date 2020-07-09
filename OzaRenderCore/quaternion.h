#pragma once
#include <istream>
#include <ostream>
#include "svector.h"
class quaternion
{
public:

	quaternion() : w(1.0), v(0.0, 0.0, 0.0) { }
	quaternion(double W, double X = 0.0, double Y = 0.0, double Z = 0.0) : w(W), v(X, Y, Z) { }
	quaternion(double W, svector V) : w(W), v(V) { }

	double length() const;
	void normalize();

	quaternion conj(void) const;
	quaternion inverse(void) const;

	double scale(void) const;

	double angleR(void) const;
	double angleD(void) const;
	svector unitV(void) const;

	quaternion& operator+=(const quaternion& r);
	quaternion& operator-=(const quaternion& r);
	quaternion& operator*=(const quaternion& r);
	quaternion& operator/=(const quaternion& r);

	svector rotate(svector v) const;

	void QuaternionFromAngleAxisR(double a[4]);
	void QuaternionFromAngleAxisR(double theta, svector V);
	void QuaternionToAngleAxisR(double a[4]) const;
	void QuaternionToAngleAxisR(double& theta, svector& V) const;
	void QuaternionFromEulerAnglesR(double x, double y, double z);
	void QuaternionToEulerAnglesR(double& x, double& y, double& z);

	void QuaternionFromAngleAxisD(double a[4]);
	void QuaternionFromAngleAxisD(double theta, svector V);
	void QuaternionToAngleAxisD(double a[4]) const;
	void QuaternionToAngleAxisD(double& theta, svector& V) const;
	void QuaternionFromEulerAnglesD(double x, double y, double z);
	void QuaternionToEulerAnglesD(double& x, double& y, double& z);

	double w;
	svector v;
};

quaternion operator-(const quaternion& q);
quaternion operator+(const quaternion& q1, const quaternion& q2);
quaternion operator-(const quaternion& q1, const quaternion& q2);
quaternion operator*(const quaternion& q1, const quaternion& q2);
quaternion operator/(const quaternion& q1, const quaternion& q2);
double dot(const quaternion& a, const quaternion& b);
std::ostream& operator<<(std::ostream& output, const quaternion& q);

///////////////////////////////////////////////////////////////////
// The following are only meant for unit quaternionernions!!!!!!	     //
///////////////////////////////////////////////////////////////////


