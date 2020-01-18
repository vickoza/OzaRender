#include <algorithm>
#include "shape.h"

std::unordered_map<shaderType, std::unique_ptr<shader>> shaderMap;
shape* shape::objectList[MAX_OBJECTS];

std::stack<matrix> shape::matrixStack;   // Model Matrix Stack
std::stack<matrix> shape::itMatrixStack; // InverseTranspose Stack for normal transforms
shaderType shape::defaultShader = shaderType::gouraudShader;
std::stack<shaderType> shape::curShader;
bool shape::stacksInitialized = false;
renderEngine* shape::renderer = nullptr;
scene* shape::curScene;

shape::shape(std::string newName, std::string newObjName)
{
	if (!stacksInitialized)
	{
		while (!curShader.empty())
		{
			curShader.pop();
		}
		curShader.push(defaultShader);
		stacksInitialized = true;
		matrix identity(4, 4, 1.0);
		matrixStack.push(identity);
		itMatrixStack.push(identity);
	}

	SetShapeName(newName);
	SetObjectName(newObjName);

	shade = shaderType::none;
	InitializeAttributes();
	dynamic = false;
}

void shape::InitializeAttributes(void)
{
	transform = matrix(4, 4, 1.0);
	eulerRot[0] = 0; eulerRot[1] = 0; eulerRot[2] = 0;
	translation[0] = 0; translation[1] = 0; translation[2] = 0;
	scale[0] = 1; scale[1] = 1; scale[2] = 1;
	shininessSet = false;
	ambientSet = false;
	diffuseSet = false;
	specularSet = false;
	alphaDataSet = false;
	transparencySet = false;

	shade = shaderType::none;

	ambient[0] = diffuse[0] = specular[0] = shininess = -1;
}

void shape::PushMatrix()
{
	itMatrixStack.push(this->GetCurrentNTransform());
	matrixStack.push(this->GetCurrentNTransform());
}

void shape::PopMatrix(void)
{
	matrixStack.pop();
	itMatrixStack.pop();
}

void shape::Draw(void)
{
	/*glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
	glPushName((GLuint)this);*/
	auto d = renderer->createShapeDraw();
	PushMatrix();

	ApplyTransforms(*d);
	ApplyMaterial(*d);

	if (shade != shaderType::none)
	{
		curShader.push(shade);
		shaderMap[shade]->BeginShading();
	}
	DrawShape(*d);
	if (shade != shaderType::none)
	{
		shaderMap[shade]->EndShading();
		curShader.pop();
	}

	PopMatrix();
	/*glPopName();
	glPopAttrib();
	glPopMatrix();*/
}


void shape::ApplyTransforms(DrawingObject& dobj)
{
	dobj.ApplyTransforms(eulerRot, translation, scale);
	/*glTranslated(translation[0], translation[1], translation[2]);
	glRotated(eulerRot[2], 0.0, 0.0, 1.0);
	glRotated(eulerRot[1], 0.0, 1.0, 0.0);
	glRotated(eulerRot[0], 1.0, 0.0, 0.0);
	glScaled(scale[0], scale[1], scale[2]);
	glMultMatrixd(transform);*/
}

void shape::ApplyMaterial(DrawingObject& dobj)
{
	dobj.ApplyMaterial(ambient, ambientSet, diffuse, diffuseSet, specular, specularSet, shininess, shininessSet);
	/*GLfloat Black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat White[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	if (diffuseSet)
	{
		glColor3fv(diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	}

	if (ambientSet)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	}

	if (shininessSet)
	{
		if (shininess < 0)
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Black);
		}
		else
		{
			if (specularSet)
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
			else
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, White);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);
		}
	}*/

}
/* //HACK
bool shape::FindShapeInHierarchy(shape* s, LinkedList<shape*>& sList)
{
	if (s == this)
	{
		sList.Append(this);
		return true;
	}
	else
		return false;
}
*/
///////////////////////////////////////////////////////////////////////////////
// The following functions manage the internal hash table of loaded shapes
// They allow for searching for shapes by name, which will retrieve a pointer
// to the shape.
///////////////////////////////////////////////////////////////////////////////

// The following function calculates a position in the object array
// Based on a shapes "object name".  It does so by adding up all the
// ascii codes of the characters in the name and taking the remainder
// of this count by MAX_OBJECTS.  Note that this only works efficiently
// if MAX_OBJECTS is a prime number.  For information on why this is
// true see any introductory text on number theory.
int shape::HashFunction(const char* name)
{
	int i = 0;
	unsigned int count = 0;

	while (name[i] != '\0')
		count = count + name[i++];
	return count % MAX_OBJECTS;
}

void shape::ClearShapes(void)
{
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		shape* s = objectList[i];
		if (s != NULL && s->dynamic == true)
			delete s;
		objectList[i] = NULL;
	}
}

void shape::RemoveShape(void)
{
	int index = ShapeIndex();
	if (index >= 0)
		objectList[index] = NULL;
}

shape* shape::FindShape(const char* objName)
{
	std::string tmpName = objName;

	std::transform(tmpName.begin(), tmpName.end(), tmpName.begin(), std::tolower);
	int index, startIndex = HashFunction(tmpName.c_str());

	index = startIndex;
	while (objectList[index] != NULL)
	{
		if (tmpName == objectList[index]->objectName)
			return objectList[index];
		index++;
		if (index >= MAX_OBJECTS)
			index = 0;
		if (index == startIndex)
			break;
	}
	return NULL;  // Not Found!
}

std::string shape::SetObjectName(std::string newObjName)
{
	std::string Name;

	if (newObjName.size() == 0)
		Name = shapeName;
	else
		Name = newObjName;

	int index, startIndex = HashFunction(Name.c_str());
	index = startIndex;

	while (objectList[index] != NULL)
	{
		index++;
		if (index >= MAX_OBJECTS)
			index = 0;
		/*if (index == startIndex)
			complain("Object List Full:  Too many shapes!");*/
	}

	RemoveShape();

	objectName == Name;
	objectList[index] = this;
	return objectName;
}

int shape::ShapeIndex(void)
{
	int startIndex = HashFunction(objectName.c_str());
	int index = startIndex;

	while (objectList[index] != this)
	{
		index++;
		if (index >= MAX_OBJECTS)
			index = 0;
		if (index == startIndex)
		{
			index = -1; // Not found
			break;
		}
	}
	return index;
}

