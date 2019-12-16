#pragma once
#include "point.h"
class svector
{
public:
	svector();
	svector(double x, double y = 0.0, double z = 0.0);
	svector(const double* x);

	double& operator[](unsigned int i) { return v[i]; }
	double operator[](unsigned int i) const { return v[i]; }

	double length(void) const;
	double sqrLength(void) const;
	void normalize(void);       // Make the svector a unit svector

	svector& operator+=(const svector& w);
	svector& operator-=(const svector& w);
	svector& operator*=(double d);
	svector& operator/=(double d);

	static inline void add(svector& res, const svector& v, const svector& w) { res[0] = v[0] + w[0]; res[1] = v[1] + w[1]; res[2] = v[2] + w[2]; }
	static inline void subtract(svector& res, const svector& v, const svector& w) { res[0] = v[0] - w[0]; res[1] = v[1] - w[1]; res[2] = v[2] - w[2]; }
	static inline void multiply(svector& res, const svector& v, double d) { res[0] = v[0] * d; res[1] = v[1] * d; res[2] = v[2] * d; }
	static inline void divide(svector& res, const svector& v, double d) { res[0] = v[0] / d; res[1] = v[1] / d; res[2] = v[2] / d; }
	static inline void subtract(svector& res, const point& p, const point& q) { res[0] = p[0] - q[0]; res[1] = p[1] - q[1]; res[2] = p[2] - q[2]; }

private:

	std::array<double,4> v;
};

double dot(const svector& v, const svector& w);
double dot(const point& p, const svector& v);
svector cross(const svector& v, const svector& w);
void cross(svector& res, const svector& v, const svector& w);

// Calculates the projection of v onto w
svector Proj(const svector& v, const svector& w);

inline void average(svector& res, const svector& p1, const svector& p2, double w = .5)
{
	double w2 = 1.0 - w; res[0] = w2 * p1[0] + w * p2[0]; res[1] = w2 * p1[1] + w * p2[1]; res[2] = w2 * p1[2] + w * p2[2];
}

inline svector average(const svector& p1, const svector& p2, double w = .5)
{
	double w2 = 1.0 - w;  return {w2 * p1[0] + w * p2[0], w2 * p1[1] + w * p2[1], w2 * p1[2] + w * p2[2]} ;
}

// Adding and Subtracting
svector operator+(const svector& v, const svector& w);
svector operator-(const svector& v, const svector& w);

// Multiplying and dividing by scalars
svector operator*(double d, const svector& v); // We need both of these, because we
svector operator*(const svector& v, double d); // want to say both d*v and v*d!
svector operator/(const svector& v, double d); // Note that (d / v) doesn't make sense.

// Negating a svector
svector operator-(const svector& v);

// Arithmetic operations involving points
point operator+(const point& p, const svector& v);
point operator+(const svector& v, const point& p);
point& operator+=(point& p, const svector& v);

point operator-(const point& p, const svector& v);
point operator-(const svector& v, const point& p);
point& operator-=(point& p, const svector& v);

svector operator-(const point& p, const point q);

std::istream& operator>>(std::istream& input, svector& v);
std::ostream& operator<<(std::ostream& output, const svector& v);

