#pragma once
#include "shape.h"

class sphere : public shape
{
public:
	sphere(point c = point(0, 0, 0), double r = 1.0, int res = 10, const char* name = "sphere") : shape("sphere", name)
	{
		/*glListNumber = 0;*/ SetSphere(c, r, res);
	}
	~sphere();

	void SetSphere(point c, double r = 1.0, int res = 10);

	virtual point Center(void) { return center; }
	virtual point Size(void) { return point(2 * radius, 2 * radius, 2 * radius); }
	virtual void MinMax(point& Min, point& Max)
	{
		svector v(radius, radius, radius); Min = center - v; Max = center + v;
	}
	virtual void DrawShape(DrawingObject& dobj) override;

protected:

	virtual void Read(Loader& input);

	void SetupShape(void);
	void DrawSphere(void);

	point center;
	double radius;
	int resolution;
};

