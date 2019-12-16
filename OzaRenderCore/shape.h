#pragma once
#include <string>
#include <array>
#include "point.h"
#include "svector.h"
#include "renderEngine.h"
#include "Loader.h"

class scene;
class shape
{
	shape(std::string newName, std::string newObjName = "");
	virtual ~shape() { RemoveShape(); }

	std::string SetObjectName(std::string newObjName);
	std::string ObjectName(void) { return objectName; }
	std::string GetShapeName(void) { return shapeName; }

	// A derived class must override the next four methods
	virtual point Center(void) = 0;
	virtual point Size(void) = 0;
	virtual void MinMax(point& min, point& max) = 0;
	virtual void DrawShape(void) = 0;

	virtual void Draw(void);
	virtual void ApplyMaterial(void);
	virtual void ApplyTransforms(void);

	virtual void SetTime(double time) { }
	virtual bool FindShapeInHierarchy(shape* s, LinkedList<shape*>& sList);

	void Translate(double x, double y, double z)
	{
		translation[0] += x; translation[1] += y; translation[2] += z;
	}

	int ShapeIndex(void);
	// The following three static methods are for loading and managing 
	// shapes and connecting shape objects to an OpenGLWin that is 
	// current for a scene
	static shape* FindShape(const char* objName);
	static shape* LoadShape(Loader& input, scene* CurScene = NULL);
	static void ClearShapes(void);
	static void SetCurrentScene(scene* newCur) { curScene = newCur; }
	static scene* GetCurrentScene(void) { return curScene; }
	static const matrix& GetCurrentTransform(void) { return matrixStack.Peek(); }
	static const matrix& GetCurrentNTransform(void) { return itMatrixStack.Peek(); }

	// These constants are important for managing shapes
	// 9973 is the largest prime number < 10000
	// Must use a prime to make hashing efficient
	enum { MAX_OBJECTS = 9973, MAX_MESSAGE_LENGTH = 1024 };

protected:

	void SetShapeName(const char* newName) { strcpy(shapeName, newName); }
	void RemoveShape(void);

	virtual bool ReadSegment(Loader& input, char* token);
	virtual void ReadObject(Loader& input);
	virtual void ReadAttributes(Loader& input);
	virtual void Read(Loader& input) { }
	virtual void InitializeAttributes(void);
	virtual void PushMatrix(void);
	virtual void PopMatrix(void);

	void PlanarMap(double x, double y, double z, double& u, double& v);
	void CylindricalMap(double x, double y, double z, double& u, double& v);
	void SphericalMap(double x, double y, double z, double& u, double& v);
	void BoxMap(double x, double y, double z, double& u, double& v);

	std::string shapeName;
	std::string objectName;

	bool dynamic;

	int			glListNumber;
	std::array<double, 3> eulerRot;
	matrix		transform; // Transformation Matrix
	std::array<double, 3> translation;
	std::array<double, 3>	scale;

	std::array<float, 3> ambient;
	bool		ambientSet;
	std::array<float, 3> diffuse;
	bool		diffuseSet;
	std::array<float,3> specular;
	bool		specularSet;
	float		shininess;
	bool		shininessSet;
	int		alphaFunc;		// Function and value for alpha testing
	float		alphaValue;
	bool		alphaDataSet;
	bool		alphaTransparency;
	bool		transparencySet;

	static renderEngine* renderer;
	/*shader* shade;

	// Static data for keeping track of rendering
	static Stack<matrix> matrixStack;   // Model Matrix Stack
	static Stack<matrix> itMatrixStack; // InverseTranspose Stack for normal transforms

	/*static gouraudShader		defaultShader;
	static Stack<shader*>		curShader;*/
	static bool					stacksInitialized;

	static scene* curScene;	// Current scene for reflection mapping

	// These members keep track of all of the shape objects that have been 
	// loaded by the system so that we can search for them by name. 
	// See the "FindShape" method above.
	static shape* objectList[MAX_OBJECTS];
	static int HashFunction(const char* name);

	friend class scene;
};

