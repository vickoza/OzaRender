#include "mesh.h"
#include "Misc.h"
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
	faces.emplace_back(source);
	for (int i = 0; i < source.vertices.size(); i++)
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

static const int MAX_OLD_MESH_SIZE = 20000;

void mesh::Read(Loader& input)
{
	char token[MAX_INPUT_LENGTH];
	int nmeshs;

	if (!input.ReadInt(nmeshs))
		ReadNew(input);
	else
	{
		Erase(MAX_OLD_MESH_SIZE, nmeshs);		// We have currently hardwired in a limit of MAX_OLD_MESH_SIZE vertices
												// for the old format.  Any larger mesh must be in the "new" format
		for (int i = 0; i < nmeshs; i++)
		{
			input.ReadToken(token);
			if (strcmp(token, "face") == 0)
				ReadFace(input);
			else
				input.Error("Insufficient number of faces or objects for mesh: %s\nEncountered '%s' before all faces were complete", objectName, token);
		}
		for (auto& vertex : vertices)
			vertex.uv = vertex.position - point::origin;
	}

	do
	{
		input.PeekTokens(token, 1);
		if (strcmp(token, "breakangle") == 0)
		{
			input.ReadToken(token); // Read the "breakangle" token
			if (!input.ReadDouble(breakAngle))
				input.Error("Incomplete 'breakangle' command for mesh %s.  Expected a number.", objectName);
		}
		else if (strcmp(token, "end") != 0)
			input.Error("Unknown option for mesh %s", objectName);
	} while (strcmp(token, "end") != 0);

	RecalculateNormals();
}

void mesh::ReadFace(Loader& input)
{
	int n;
	vert* v;
	double coord[3];

	if (!input.ReadInt(n))
		input.Error("Syntax error in face command.  # Vertices not found.");

	ResizeFaceList();
	faces.back().SetNVerts(n);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < 3; j++)
			if (!input.ReadDouble(coord[j]))
				;//input.Error("Syntax error in face command.  Too few coordinates for Vertex #%d.", nFaces + 1); //HACK
		v = AddVertex(coord);
		faces.back().vertices[i] = v;
		v->AppendFace(&faces.back());
	}
}

void mesh::ReadNew(Loader& input)
{
	char token[MAX_INPUT_LENGTH];

	int nVerts;
	int nFaces;

	input.ReadToken(token);
	if (strncmp(token, "vertices", 4) != 0)
		input.Error(token, "An indexed mesh must begin with a Vertices command");

	if (!input.ReadInt(nVerts))
		input.Error("Vertices command must be followed by a vertex count");

	vertices.clear();
	vertices.resize(nVerts);

	input.ReadToken(token);
	if (strncmp(token, "vert", 4) != 0)
		input.Error("A vertex must begin with a 'vert' command");

	for (int i = 0; i < nVerts; i++)
	{
		vertices[i].position = point(0, 0, 0);
		vertices[i].uv = svector(9.9999e25, 0, 0);
		vertices[i].ClearFaceList();

		do
		{
			input.ReadToken(token);
			if (strncmp(token, "coord", 5) == 0)
			{
				int j;
				for (j = 0; j < 3; j++)
					if (!input.ReadDouble(vertices[i].position[j]))
						input.Error("Incomplete coordinate data for vertex %d, must have at least 3", i);

				if (!input.ReadDouble(vertices[i].position[3]))
					vertices[i].position[j] = 1.0;
			}
			else if (strncmp(token, "uv", 3) == 0)
			{
				vertices[i].uv = svector(0, 0, 0);
				for (int j = 0; j < 2; j++)
					if (!input.ReadDouble(vertices[i].uv[j]))
						input.Error("Incomplete uv data for vertex %d, must have 2", i);
			}
			else if (strncmp(token, "norm", 4) == 0)
			{
				vertices[i].normal = svector(0, 0, 0);
				for (int j = 0; j < 3; j++)
					if (!input.ReadDouble(vertices[i].normal[j]))
						input.Error("Incomplete normal data for vertex %d, must have 3", i);
				normalsSpecifiedInFile = true;
			}
			else if (strncmp(token, "endvertices", 6) == 0)
			{
				if (i != nVerts - 1);
					//complain("Vertex list ended before %d vertices were found", nVerts);
				else
					break; // Continue to process the faces
			}
			else if (strncmp(token, "vert", 4) != 0)
				input.Error(token, "Unknown Vertex Command Encountered: %s", token);
		} while (strncmp(token, "vert", 4) != 0);  // If you hit a vert command, go on to next vertex

		if (vertices[i].uv[0] == 9.9999e25)
			vertices[i].uv = vertices[i].position - point::origin;
	}

	input.ReadToken(token);
	if (strncmp(token, "faces", 5) != 0)
		input.Error(token, "Vertex list must be followed by face list");

	if (!input.ReadInt(nFaces))
		input.Error("Faces command must be followed by a face count");

	faces.clear();
	faces.resize(nFaces);

	if (!input.ReadToken(token))
		input.Error("A face must begin with a 'face' command");

	for (int i = 0; i < nFaces; i++)
	{
		int vertCount;
		if (!input.ReadInt(vertCount))
			input.Error("face command must be followed by a vertex count");

		faces[i].SetNVerts(vertCount);
		do
		{
			input.ReadToken(token);
			if (strncmp(token, "index", 5) == 0)
			{
				for (int j = 0; j < vertCount; j++)
				{
					int vertIndex;
					if (!input.ReadInt(vertIndex))
						input.Error("Incomplete vertex data for face %d, must have %d", i, vertCount);
					else if (vertIndex >= nVerts)
						input.Error("Badly formatted data: vertex index %d in mesh %s\n", vertIndex, objectName);
					faces[i].vertices[j] = &vertices[vertIndex];
				}
			}
			else if (strncmp(token, "face", 4) == 0)
			{
				continue;
			}
			else if (strncmp(token, "endfaces", 4) == 0)
			{
				if (i != nFaces - 1)
					input.Error(token, "End reached before %d faces were found", nFaces);
				else
					break; // Continue to process the faces
			}
		} while (strncmp(token, "face", 4) != 0);  // If hit a new face command, then continue on to next face
		faces[i].RecalculateNormal();
		if (faces[i].normal.length() > TD_SMALL)
		{
			for (int k = 0; k < faces[i].vertices.size(); k++)
				faces[i].vertices[k]->AppendFace(&faces[i]);
		}
	}
}

void mesh::Write(std::ostream& output)
{
	output << "\tVertices " << vertices.size() << '\n';
	for (const auto& vertex : vertices)
		output << "\t\tvert\tcoord\t" << vertex.position << '\n';
	output << "\tEndVertices\n";
	output << "\tFaces " << faces.size() << '\n';
	for (const auto& face : faces)
	{
		output << "\t\tface " << face.vertices.size() << "\tindex\t";
		for (const auto&  vert : face.vertices)
		{
			// This can be improved.  All we need to do is to subtract pointers to
			// get the index!
			for (int k = 0; k < vertices.size(); k++)
			{
				if (&vertices[k] == vert)
					output << k << " ";
			}
		}
		output << '\n';
	}
	output << "\tEndFaces\n";
	output << "End" << '\n';
	output.flush();
}

