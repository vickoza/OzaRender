#include "vert.h"
#include "face.h"

void vert::RecalculateNormal(void)
{
	normal[0] = 0;
	normal[1] = 0;
	normal[2] = 0;
	if (facelist.size() == 0)
		return;
	for (const auto& f : facelist)
	{
		normal += f->normal;
	}
	normal /= ((double)facelist.size());
	normal.normalize();
}

void vert::AppendFace(face* f)
{
	facelist.emplace_back(f);
	current = facelist.begin();
}

void vert::ClearFaceList(void)
{
	facelist.clear();
}

void vert::SetPosition(double x, double y, double z)
{
	position[0] = x; position[1] = y; position[2] = z;
	RecalculateNormal();
}

void vert::SetPosition(double* x)
{
	position[0] = x[0]; position[1] = x[1]; position[2] = x[2];
	RecalculateNormal();
}
