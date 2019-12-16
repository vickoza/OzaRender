#include <cmath>
#include "svector.h"

using namespace std;

svector::svector() : svector(0.0)
{
}


svector::svector(double x, double y, double z) : v{x, y, z, 0.0}
{
}

svector::svector(const double* x) : v{x[0], x[1], x[2], 0.0}
{
}

double svector::length(void) const
{
	return sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
}

double svector::sqrLength(void) const
{
	return (sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
}

void svector::normalize(void)
{
	double len = length();
	if (len > 1e-9)
	{
		v[0] /= len; v[1] /= len; v[2] /= len;
	}
}

svector& svector::operator+=(const svector& w)
{
	v[0] += w[0]; v[1] += w[1]; v[2] += w[2];
	return *this;
}

svector& svector::operator-=(const svector& w)
{
	v[0] -= w[0]; v[1] -= w[1]; v[2] -= w[2];
	return *this;
}

svector& svector::operator*=(double d)
{
	v[0] *= d; v[1] *= d; v[2] *= d;
	return *this;
}

svector& svector::operator/=(double d)
{
	v[0] /= d; v[1] /= d; v[2] /= d;
	return *this;
}

double dot(const svector& v, const svector& w)
{
	return v[0] * w[0] + v[1] * w[1] + v[2] * w[2];
}

double dot(const point& p, const svector& v)
{
	return p[0] * v[0] + p[1] * v[1] + p[2] * v[2];
}

svector cross(const svector& v, const svector& w)
{
	return { v[1] * w[2] - v[2] * w[1],
		v[2] * w[0] - v[0] * w[2],
		v[0] * w[1] - v[1] * w[0] };
}

void cross(svector& res, const svector& v, const svector& w)
{
	res[0] = v[1] * w[2] - v[2] * w[1];
	res[1] = v[2] * w[0] - v[0] * w[2];
	res[2] = v[0] * w[1] - v[1] * w[0];
}

// Calculates the projection of v onto w
svector Proj(const svector& v, const svector& w)
{
	return dot(v, w) / sqr(w.length()) * w;
}

svector operator+(const svector& v, const svector& w)
{
	return svector(v[0] + w[0], v[1] + w[1], v[2] + w[2]);
}

svector operator-(const svector& v, const svector& w)
{
	return svector(v[0] - w[0], v[1] - w[1], v[2] - w[2]);
}

svector operator*(double d, const svector& v)
{
	return svector(d * v[0], d * v[1], d * v[2]);
}

svector operator*(const svector& v, double d)
{
	return d * v;
}

// Note that (d / v) doesn't make sense
svector operator/(const svector& v, double d)
{
	return svector(v[0] / d, v[1] / d, v[2] / d);
}

point operator+(const point& p, const svector& v)
{
	return point(p[0] + v[0], p[1] + v[1], p[2] + v[2]);
}

point operator+(const svector& v, const point& p)
{
	return p + v;
}

point& operator+=(point& p, const svector& v)
{
	p[0] += v[0]; p[1] += v[1]; p[2] += v[2];
	return p;
}

point operator-(const point& p, const svector& v)
{
	return point(p[0] - v[0], p[1] - v[1], p[2] - v[2]);
}

point operator-(const svector& v, const point& p)
{
	return point(p[0] - v[0], p[1] - v[1], p[2] - v[2]);
}

point& operator-=(point& p, const svector& v)
{
	p[0] -= v[0]; p[1] -= v[1]; p[2] -= v[2];
	return p;
}

svector operator-(const point& p, const point q)
{
	return svector(p[0] - q[0], p[1] - q[1], p[2] - q[2]);
}

svector operator-(const svector& v)
{
	return svector(-v[0], -v[1], -v[2]);
}

istream& operator>>(istream& input, svector& v)
{
	input >> v[0] >> v[1] >> v[2];
	return input;
}

ostream& operator<<(ostream& output, const svector& v)
{
	output << v[0] << " " << v[1] << " " << v[2] << " ";
	return output;
}
