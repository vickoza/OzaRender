#pragma once
#include "vert.h"
class face
{
public:

	face(int n = 3);
	~face() { delete[] vertices; delete[] normals; }

	void SetNVerts(int n);
	void RecalculateNormal(void);
	void RecalculateVertexNormals(double toleranceAngle = 90);

	int nVerts;
	vert** vertices;
	svector* normals;
	svector normal;
};

