#include "mesh.h"
const double TD_SMALL = 1e-6;

mesh::mesh(std::string name) : shape("mesh")
{
	SetObjectName(name);
	vertices.reserve(10);
	faces.reserve(10);
	breakAngle = 180;
	glListNumber = 0;
	resize = true;
	normalsSpecifiedInFile = false;
}

mesh::mesh(const mesh& m) : shape("mesh")
{
	SetObjectName(m.objectName);

	vertices.reserve(10);
	faces.reserve(10);
	breakAngle = m.breakAngle;
	normalsSpecifiedInFile = m.normalsSpecifiedInFile;

	for (auto face : m.faces)
		AddFace(face);

	resize = true;

	glListNumber = 0;

	RecalculateNormals();
}

mesh& mesh::operator=(const mesh& m)
{
	SetObjectName(m.objectName);
	Erase(m.vertices.size(), m.faces.size());
	breakAngle = m.breakAngle;

	for (auto face : m.faces)
		AddFace(face);

	resize = true;
	RecalculateNormals();
	glListNumber = 0;

	return *this;
}

mesh::~mesh()
{
}

void mesh::Erase(int newVertexSize, int newFaceSize)
{
	vertices.clear();
	vertices.reserve(newVertexSize);

	faces.clear();
	faces.reserve(newFaceSize);
	resize = true;
}

void mesh::AddFace(face& source)
{
	ResizeFaceList();

	vert* v;
	faces.emplace_back(source.nVerts);
	for (int i = 0; i < source.nVerts; i++)
	{
		v = AddVertex(source.vertices[i]->position.c.data());
		faces.back().vertices[i] = v;
		v->AppendFace(&faces[i]);
	}
	resize = true;
}

void mesh::AddFace(double* Vertices[], int n)
{
	ResizeFaceList();

	vert* v;
	faces.emplace_back(n);
	for (int i = 0; i < n; i++)
	{
		v = AddVertex(Vertices[i]);
		faces.back().vertices[i] = v;
		v->AppendFace(&faces[i]);
	}
	resize = true;
}

void mesh::AddFace(point* Vertices, int n)
{
	ResizeFaceList();

	vert* v;
	faces.emplace_back(n);
	for (int i = 0; i < n; i++)
	{
		v = AddVertex(Vertices[i].c.data());
		faces.back().vertices[i] = v;
		v->AppendFace(&faces[i]);
	}
	resize = true;
}

void mesh::AddFace(face* f)
{
	ResizeFaceList();
	faces.emplace_back(*f);
	resize = true;
}

void mesh::ResizeFaceList(void)
{
	/*if (nFaces == faceSize)
	{
		cout << "Error cannot resize face list.  Allocated " << nFaces << " for this shape " << endl;
		system("Pause");
		exit(1);
	}*/
}

vert* mesh::AddVertex(const double* Coord)
{
	for (auto& vertice : vertices)
	{
		double* tmp = vertice.position.c.data();
		if ((fabs(tmp[0] - Coord[0]) < TD_SMALL) &&
			(fabs(tmp[1] - Coord[1]) < TD_SMALL) &&
			(fabs(tmp[2] - Coord[2]) < TD_SMALL))
		{
			return &vertice;
		}
	}

	/*if (vertices[i].ca == vertices.size())
	{
		std::cout << "Error cannot resize vertex list.  Current size = " << vertSize << "\n";
		system("Pause");
		exit(1);
	}*/

	resize = true;
	vertices.emplace_back();
	for (int i = 0; i < 3; i++)
		vertices.back().position[i] = Coord[i];
	vertices.back().ClearFaceList();
	return &vertices.back();
}

void mesh::SetVertex(int k, double Coord[3], bool recalculateNormals)
{
	if (k < vertices.size())
	{
		vertices[k].position = Coord;
		if (recalculateNormals)
			RecalculateNormals();
		resize = true;
	}
}

void mesh::DrawShape(DrawingObject& dobj)
{
	dobj.DrawMesh(*this, *shaderMap[curShader.top()]);
	/*
	// Have a limit of 1000 vertices per triangle
	point  p[1000];
	vector uv[1000];
	vector vn[1000], fn;
	vert* v;

	glEnable(GL_CULL_FACE);

	if (glListNumber == 0)
	{
		glListNumber = glGenLists(1);
		glNewList(glListNumber, GL_COMPILE_AND_EXECUTE);

		glDisable(GL_CULL_FACE);

		for (int i = 0; i < nFaces; i++)
		{
			fn = faces[i].normal;
			for (int j = 0; j < faces[i].nVerts; j++)
			{
				v = faces[i].vertices[j];
				p[j] = v->position;
				if (normalsSpecifiedInFile)
					vn[j] = v->normal;
				else
					vn[j] = faces[i].normals[j];	// Use this one if you are using the break angle
				uv[j] = v->uv;
			}
			curShader.Peek()->DrawPolygon(faces[i].nVerts, p, vn, fn, uv);
		}
		glEndList();
	}
	else
		glCallList(glListNumber);
	*/
}

void mesh::RecalculateNormals(void)
{
	for (auto& face : faces)
		face.RecalculateNormal();
	if (!normalsSpecifiedInFile)
	{
		for (auto& vertice : vertices)
			vertice.RecalculateNormal();
		for (auto& face : faces)
			face.RecalculateVertexNormals(breakAngle);
	}
}

point mesh::Center(void)
{
	MinMax(mMin, mMax);
	return (mMin + mMax) * .5;
}

point mesh::Size(void)
{
	MinMax(mMin, mMax);
	return point(mMax[0] - mMin[0], mMax[1] - mMin[1], mMax[2] - mMin[2]);
}

void mesh::MinMax(point& Min, point& Max)
{
	if (resize)
	{
		if (vertices.size() < 1)
		{
			Min = Max = point(0, 0, 0);
		}
		else
		{
			mMin[0] = mMax[0] = vertices[0].position[0];
			mMin[1] = mMax[1] = vertices[0].position[1];
			mMin[2] = mMax[2] = vertices[0].position[2];
			for (const auto& v: vertices)
			{
				mMin[0] = std::min(mMin[0], v.position[0]);
				mMin[1] = std::min(mMin[1], v.position[1]);
				mMin[2] = std::min(mMin[2], v.position[2]);

				mMax[0] = std::max(mMax[0], v.position[0]);
				mMax[1] = std::max(mMax[1], v.position[1]);
				mMax[2] = std::max(mMax[2], v.position[2]);
			}
		}
		resize = false;
	}
	Min = mMin; Max = mMax;
}

point mesh::GetVertex(int k)
{
	if (k < vertices.size())
	{
		vert* v = &vertices[k];
		return point(v->position[0], v->position[1], v->position[2]);
	}
	else
		return point(0, 0, 0);
}

int mesh::NVertices(void)
{
	return vertices.size();
}

int mesh::NFaces(void)
{
	return vertices.size();
}
