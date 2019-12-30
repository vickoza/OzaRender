#pragma once
#include <vector>
#include "shape.h"
#include "vert.h"
#include "face.h"

class mesh : public shape
{
public:

	mesh(std::string name = "mesh");
	mesh(const mesh& c);
	virtual ~mesh() override;

	void SetFaceAngle(double angle) { breakAngle = angle; RecalculateNormals(); }

	void AddFace(point* Vertices, int n);
	void AddFace(double* Vertices[], int n);

	void SetVertex(int k, double Coord[3], bool AutoRecalculateNormals = true);
	void SetVertex(int k, point Coord, bool recalculateNormals = true)
	{
		SetVertex(k, Coord.c.data(), recalculateNormals);
	}
	point GetVertex(int k);

	int NVertices(void);
	int NFaces(void);
	int NInstances(void);

	void RecalculateNormals(void);
	void DrawShape(DrawingObject& dobj) override;
	void Erase(int newVertexSize, int newFaceSize);

	void Read(Loader& input);
	void Write(std::ostream& output);

	mesh& operator=(const mesh& c);

	virtual point Center(void);
	virtual point Size(void);
	virtual void MinMax(point& min, point& max);

protected:

	vert* AddVertex(const double* Coord);
	void AddFace(face* f);
	void AddFace(face& source);
	void CopyFace(face* f);
	void ResizeFaceList(void);

	void ReadNew(Loader& input);
	void ReadFace(Loader& input);

	// We could make these simple arrays instead of arrays of pointers
	// if we put a copy method into the face structure so that we could
	// conveniently sort them.  Swapping pointers remains the fastest 
	// method
	std::vector<vert> vertices;
	std::vector<face> faces;
	point mMin, mMax;
	double breakAngle; // angle for faces that show a hard corner
	bool resize;
	bool normalsSpecifiedInFile;
};