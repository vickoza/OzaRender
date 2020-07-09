#pragma once
#include "vert.h"
class face
{
public:

	face(int n = 3);
	~face() { }

	void SetNVerts(int n);
	void RecalculateNormal(void);
	void RecalculateVertexNormals(double toleranceAngle = 90);

	std::vector<vert*> vertices;
	std::vector<svector> normals;
	svector normal;
};

