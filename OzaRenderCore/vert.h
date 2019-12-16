#pragma once
#include <vector>
#include "svector.h"
class face;

class vert
{
public:

	void RecalculateNormal(void);
	void SetPosition(double x, double y, double z);
	void SetPosition(double* x);

	double& operator[](int i) { return position[i]; }

	void AppendFace(face* f);
	face* GetCurrent(void) const { return (*current); }
	void MoveFirst(void) { current = facelist.begin(); }
	void MoveNext(void) { if (!AtEnd()) ++current; }
	bool AtEnd(void) const { return current == facelist.end(); }
	void ClearFaceList(void);

	point position;
	svector normal;
	std::vector<face*> facelist;
	svector uv;
	decltype(facelist.begin()) current;
};

