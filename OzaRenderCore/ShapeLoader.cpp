#include "shape.h"
#include "mesh.h"
#include "component.h"
#include "sphere.h"
#include "animatedShape.h"
#include "Loader.h"


bool shape::ReadSegment(Loader& input, char* token)
{
	if (!strncmp(token, "col", 3))
	{
		int i;
		for (i = 0; i < 3; i++)
			if (!input.ReadFloat(ambient[i]))
				input.Error("Incomplete Color Data");

		if (!input.ReadFloat(ambient[i]))
			ambient[3] = 1.0;

		diffuse[0] = ambient[0];
		diffuse[1] = ambient[1];
		diffuse[2] = ambient[2];
		diffuse[3] = ambient[3];

		ambientSet = diffuseSet = true;

		return true;
	}
	else if (!strncmp(token, "transl", 6))
	{
		for (int i = 0; i < 3; i++)
			if (!input.ReadDouble(translation[i]))
				input.Error("Incomplete Translation Data");
		return true;
	}
	else if (!strncmp(token, "sc", 2))
	{
		for (int i = 0; i < 3; i++)
			if (!input.ReadDouble(scale[i]))
				input.Error("Incomplete Scale Data");
		return true;
	}
	else if (!strncmp(token, "rot", 3))
	{
		for (int i = 0; i < 3; i++)
			if (!input.ReadDouble(eulerRot[i]))
				input.Error("Incomplete Rotation Data");
		return true;
	}
	else if (!strncmp(token, "mat", 3)) // 3x3 matrix
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				if (!input.ReadDouble(transform(i, j)))
					input.Error("Incomplete Matrix Information");
		return true;
	}
	else if (!strncmp(token, "homog", 5)) // homogeneous 4x4 matrix
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (!input.ReadDouble(transform(i, j)))
					input.Error("Incomplete Homogeneous Matrix Information");
		return true;
	}
	else if (!strcmp(token, "speccolor") || !strcmp(token, "specularcolor"))
	{
		for (int i = 0; i < 3; i++)
			if (!input.ReadFloat(specular[i]))
				input.Error("Incomplete Specular Information");

		if (!input.ReadFloat(specular[3]))
			specular[3] = 1.0;
		specularSet = true;
		return true;
	}
	else if (!strcmp(token, "specular"))
	{
		if (!input.ReadFloat(shininess))
			input.Error("Incomplete specular information");
		shininessSet = true;
		return true;
	}
	else if (!strncmp(token, "am", 2))
	{
		for (int i = 0; i < 3; i++)
			if (!input.ReadFloat(ambient[i]))
				input.Error("Incomplete ambient information");

		if (!input.ReadFloat(ambient[3]))
			ambient[3] = 1.0;
		ambientSet = true;
		return true;
	}
	else if (!strncmp(token, "dif", 3))
	{
		for (int i = 0; i < 3; i++)
			if (!input.ReadFloat(diffuse[i]))
				input.Error("Incomplete diffuse information");

		if (!input.ReadFloat(diffuse[3]))
			diffuse[3] = 1.0;
		diffuseSet = true;
		return true;
	}
	else if (!strcmp(token, "alphatest"))
	{
		char tmp[MAX_INPUT_LENGTH];

		input.ReadToken(tmp);
		/*if (strcmp(tmp, "never") == 0)
			alphaFunc = GL_NEVER;
		else if (strcmp(tmp, "less") == 0 || strcmp(tmp, "<") == 0)
			alphaFunc = GL_LESS;
		else if (strcmp(tmp, "equal") == 0 || strcmp(tmp, "==") == 0)
			alphaFunc = GL_EQUAL;
		else if (strcmp(tmp, "lequal") == 0 || strcmp(tmp, "<=") == 0)
			alphaFunc = GL_LEQUAL;
		else if (strcmp(tmp, "greater") == 0 || strcmp(tmp, ">") == 0)
			alphaFunc = GL_GREATER;
		else if (strcmp(tmp, "notequal") == 0 || strcmp(tmp, "!=") == 0)
			alphaFunc = GL_NOTEQUAL;
		else if (strcmp(tmp, "gequal") || strcmp(tmp, ">=") == 0)
			alphaFunc = GL_GEQUAL;
		else if (strcmp(tmp, "always") == 0)
			alphaFunc = GL_ALWAYS;
		else
			input.Error(tmp, "Unrecognized alpha test function: %s", tmp);

		if (!input.ReadFloat(alphaValue))
			input.Error(tmp, "alpha test function must be followed by a test value");*/

		alphaDataSet = true;
		return true;
	}
	else if (!strcmp(token, "transparency"))
	{
		char tmp[MAX_INPUT_LENGTH];

		input.ReadToken(tmp);
		if (strcmp(tmp, "on") == 0)
			alphaTransparency = true;
		else if (strcmp(tmp, "off") == 0)
			alphaTransparency = false;
		else
			input.Error(tmp, "Transparency must be followed by either 'on' or 'off'");

		transparencySet = true;
		return true;
	}
	else if (!strncmp(token, "shad", 4))
	{
		shade = shader::LoadShader(input);
		if (shade == shaderType::none)
		{
			shaderMap[shade]->SetShape(this);
			shaderMap[shade]->SetScene(curScene);
			return true;
		}
		else
		{
			input.Error(token, "Couldn't Process Shader, Unknown Shader?");
			return false;
		}
	}
	else
		return false;
}

void shape::ReadObject(Loader& input)
{
	char token[MAX_INPUT_LENGTH];
	std::string lCaseName;

	InitializeAttributes();
	input.ReadToken(token);

	lCaseName =  shapeName;

	std::transform(lCaseName.cbegin(), lCaseName.cend(), lCaseName.begin(), [](auto c) { return std::tolower(c); });
	if ((strcmp(token, lCaseName.c_str()) == 0) ||
		(strcmp(token, "define") == 0))	// The first token should be the shape name
		input.ReadToken(token);			// Then next token will be the object's name
										// define is the one exception it can be either
										// a component or a mesh
	SetObjectName(token);

	Read(input);					// Read the object's data
	ReadAttributes(input);			// Read the object's attributes which must follow

	input.ReadToken(token);			// Get the final end
	if (strcmp(token, "end") != 0)	// Make sure it is an end
		input.Error(token, "Object %s does not end with 'end': %s", objectName, token);
}

void shape::ReadAttributes(Loader& input)
{
	char token[MAX_INPUT_LENGTH];
	int oldPos = 0;
	do
	{
		oldPos = input.CurPos();			// Remember the old position.  The last token will
		input.ReadToken(token);				// Be an end, so we have to move back so that input function
	} while (ReadSegment(input, token));	// can read that 'end' command
	input.MoveTo(oldPos);
}

////////////////////////////////////////////////////////////////////////////
// The next function is a general method for loading new shapes.  
// Any new shapes added to the hierarchy should be included here
////////////////////////////////////////////////////////////////////////////

// Checks the first word in the shape definition to see what kind of 
// Shape it is.  Then it creates a shape of the appropriate type 
// And loads the shape from the file, and returns a pointer to that shape
//  
// If it doesn't recognize the Shape, it returns NULL;
shape* shape::LoadShape(Loader& input, scene* CurScene)
{
	char shapeType[MAX_INPUT_LENGTH];
	shape* res;

	input.ReadToken(shapeType);
	if (strncmp(shapeType, "comp", 4) == 0)
		res = new component;
	else if (strcmp(shapeType, "mesh") == 0)
		res = new mesh;
	else if (strcmp(shapeType, "sphere") == 0)
		res = new sphere;
	else if (strcmp(shapeType, "animatedshape") == 0)
		res = new animatedShape;
	else
		return nullptr;

	res->SetCurrentScene(CurScene);
	res->ReadObject(input);
	res->dynamic = true;

	return res;
}
