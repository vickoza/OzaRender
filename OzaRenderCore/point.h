#pragma once
#include <array>
constexpr long double PI = (long double)3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460;
class point
{
public:
	point();
	point(double x, double y, double z, double w = 1.0);
	point(const double* x);

	// Note that we do not check array bounds here!  This is for speed!
	double& operator[](unsigned int i) { return c[i]; }

	// Of course we need an accessor for const points too ... one which 
	// doesn't return a reference through which the coordinates could
	// be changed!
	double operator[](unsigned int i) const { return c[i]; }

	void normalize() { c[0] /= c[3]; c[1] /= c[3]; c[2] /= c[3]; c[3] = 1.0; }

	friend double distanceSqr(const point& p, const point& q);

	point& operator+=(const point& q);

	static point origin;

	std::array<double, 4> c;
};

double dist(const point& p, const point& q);
template<typename T>
inline T sqr(const T& val) { return val * val; }
inline double dot(const point& p, const point& q) { return p[0] * q[0] + p[1] * q[1] + p[2] * q[2]; }

inline void average(point& res, const point& p1, const point& p2, double w = .5)
{
	double w2 = 1.0 - w; res[0] = w2 * p1[0] + w * p2[0]; res[1] = w2 * p1[1] + w * p2[1]; res[2] = w2 * p1[2] + w * p2[2];
}

inline point average(const point& p1, const point& p2, double w = .5)
{
	point res; double w2 = 1.0 - w; res[0] = w2 * p1[0] + w * p2[0]; res[1] = w2 * p1[1] + w * p2[1]; res[2] = w2 * p1[2] + w * p2[2]; return res;
}

point operator+(const point& p, const point& q);
point operator*(double d, const point& p);
point operator*(const point& p, double d);
point operator/(const point& p, double d);

point& operator*=(point& p, double d);
point& operator/=(point& p, double d);

std::istream& operator>>(std::istream& input, point& v);
std::ostream& operator<<(std::ostream& output, const point& v);


