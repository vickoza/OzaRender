#include "shape.h"

shape* shape::objectList[MAX_OBJECTS];

Stack<matrix> shape::matrixStack;   // Model Matrix Stack
Stack<matrix> shape::itMatrixStack; // InverseTranspose Stack for normal transforms
gouraudShader shape::defaultShader;
Stack<shader*> shape::curShader;
bool shape::stacksInitialized = false;

scene* shape::curScene;

shape::shape(const char* newName, const char* newObjName)
{
	if (!stacksInitialized)
	{
		curShader.Clear();
		curShader.Push(&defaultShader);
		stacksInitialized = true;
		matrix identity(4, 4, 1.0);
		matrixStack.Push(identity);
		itMatrixStack.Push(identity);
	}

	SetShapeName(newName);
	SetObjectName(newObjName);

	shade = NULL;
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

	if (!shade)
	{
		delete shade; // we assume that all shaders are created dynamically!!!!
		shade = NULL;
	}

	ambient[0] = diffuse[0] = specular[0] = shininess = -1;
}

void shape::PushMatrix()
{
	itMatrixStack.Push(this->GetCurrentNTransform());
	matrixStack.Push(this->GetCurrentNTransform());
}

void shape::PopMatrix(void)
{
	matrixStack.Pop();
	itMatrixStack.Pop();
}

void shape::Draw(void)
{
	/*glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
	glPushName((GLuint)this);*/
	auto d = renderer->createShapeDraw();
	PushMatrix();

	ApplyTransforms();
	ApplyMaterial();

	if (shade)
	{
		curShader.Push(shade);
		shade->BeginShading();
	}
	DrawShape();
	if (shade)
	{
		shade->EndShading();
		curShader.Pop();
	}

	PopMatrix();
	glPopName();
	glPopAttrib();
	glPopMatrix();
}


void shape::ApplyTransforms(void)
{
	glTranslated(translation[0], translation[1], translation[2]);
	glRotated(eulerRot[2], 0.0, 0.0, 1.0);
	glRotated(eulerRot[1], 0.0, 1.0, 0.0);
	glRotated(eulerRot[0], 1.0, 0.0, 0.0);
	glScaled(scale[0], scale[1], scale[2]);
	glMultMatrixd(transform);
}

void shape::ApplyMaterial(void)
{

	GLfloat Black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
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
	}

}

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
	uint count = 0;

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
	char tmpName[MAX_INPUT_LENGTH];

	strcpy(tmpName, objName);

	LCase(tmpName);
	int index, startIndex = HashFunction(tmpName);

	index = startIndex;
	while (objectList[index] != NULL)
	{
		if (strcmp(tmpName, objectList[index]->objectName) == 0)
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

	int index, startIndex = HashFunction(Name);
	index = startIndex;

	while (objectList[index] != NULL)
	{
		index++;
		if (index >= MAX_OBJECTS)
			index = 0;
		if (index == startIndex)
			complain("Object List Full:  Too many shapes!");
	}

	RemoveShape();

	strcpy(objectName, Name);
	objectList[index] = this;
	return objectName;
}

int shape::ShapeIndex(void)
{
	int startIndex = HashFunction(objectName);
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

