#include "point.h"
using namespace std;
point point::origin;

point::point() : point(0, 0, 0)
{
}

point::point(double x, double y, double z, double w) :c{x, y, z, w}
{
}

point::point(const double* x) : c{x[0], x[1], x[2], 1.0}
{
}
double dist(const point& p, const point& q)
{
	return sqrt(sqr(p[0] - q[0]) + sqr(p[1] - q[1]) + sqr(p[2] - q[2]));
}

double distanceSqr(const point& p, const point& q)
{
	return sqr(p.c[0] - q.c[0]) + sqr(p.c[1] - q.c[1]) + sqr(p.c[2] - q.c[2]);
}

point operator+(const point& p, const point& q)
{
	return point(p[0] + q[0], p[1] + q[1], p[2] + q[2]);
}

point& point::operator+=(const point& q)
{
	c[0] += q[0]; c[1] += q[1]; c[2] += q[2];
	return *this;
}

point operator*(double d, const point& p)
{
	return point(d * p[0], d * p[1], d * p[2], p[3]);
}

point operator*(const point& p, double d)
{
	return point(d * p[0], d * p[1], d * p[2], p[3]);
}

point& operator*=(point& p, double d)
{
	p.c[0] *= d;
	p.c[1] *= d;
	p.c[2] *= d;

	return p;
}

point& operator/=(point& p, double d)
{
	p.c[0] /= d;
	p.c[1] /= d;
	p.c[2] /= d;

	return p;
}

point operator/(const point& p, double d)
{
	return point{ p[0] / d, p[1] / d, p[2] / d, p[3] };
}

istream& operator>>(istream& input, point& p)
{
	input >> p[0] >> p[1] >> p[2];
	return input;
}

ostream& operator<<(ostream& output, const point& p)
{
	output << p[0] << " " << p[1] << " " << p[2] << " " << p[3];
	return output;
}

