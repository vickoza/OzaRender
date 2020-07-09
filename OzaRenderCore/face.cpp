#include "face.h"
#include "Misc.h"

face::face(int n)
{
	vertices.resize(n);
	normals.resize(n);
}

void face::SetNVerts(int n)
{
	vertices.clear();
	normals.clear();
	vertices.resize(n);
	normals.resize(n);
}

void face::RecalculateNormal(void)
{
	point p1, p2, p3;
	svector v1(0, 0, 0), v2(0, 0, 0);
	double length = 0;

	normal[0] = 0.0; normal[1] = 0.0; normal[2] = 0.0; // Initialize it

	int k1 = 0, k2 = 0;
	p1 = vertices[0]->position;
	while ((v1.length() < TD_SMALL) && (k1 < vertices.size() - 1))
	{
		k1++;
		p2 = vertices[k1]->position;
		v1 = p2 - p1;
	}
	k2 = k1;
	while ((length < TD_SMALL) && (k2 < vertices.size() - 1))
	{
		k2++;
		p3 = vertices[k2]->position;
		v2 = p3 - p2;
		normal = cross(v1, v2);

		length = sqrt(sqr(normal[0]) + sqr(normal[1]) + sqr(normal[2]));
	}
	if (length > TD_SMALL)
	{
		normal[0] /= length;
		normal[1] /= length;
		normal[2] /= length;
	}
}

void face::RecalculateVertexNormals(double toleranceAngle)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i] != nullptr && (toleranceAngle <= (dot(vertices[i]->normal, normal) / (vertices[i]->normal.length() * normal.length()))))
		{
			normals[i] = vertices[i]->normal;
		}
		else
			normals[i] = normal;
	}
}