#include "sphere.h"
#include "Misc.h"

sphere::~sphere()
{
	//glDeleteLists(glListNumber, 1);
}

void sphere::SetSphere(point c, double r, int res)
{
	center = c;
	radius = r;
	resolution = res;
}

void sphere::DrawShape(DrawingObject& dobj)
{
	dobj.DrawSphere(*this, *shaderMap[curShader.top()]);
/*	glEnable(GL_CULL_FACE);

	if (glListNumber == 0)
		SetupShape();

	glCallList(glListNumber);*/
}

void sphere::SetupShape(void)
{
	/*if (glIsList(glListNumber))
		glDeleteLists(glListNumber, 1);

	glListNumber = glGenLists(1);
	glNewList(glListNumber, GL_COMPILE);

	glCullFace(GL_FRONT);
	DrawSphere();
	glCullFace(GL_BACK);
	DrawSphere();

	glEndList();*/
}

void sphere::DrawSphere(void)
{
	point p[4];		// The polygon's vertices (points)
	svector vn[4];	// The polygon vertex normals
	svector uv[4];	// The polygon vertex uv-coordinates
	svector pn;		// The polygon's normal

	// Notice in this version I've simplified things a bit.
	// There is no longer a need to have arrays of pointers.
	// We can just pass in p, vn, uv and pn into DrawPolygon

	double thetaInc = PI / resolution;
	double thetaLimit = 2 * PI - thetaInc / 2.0;
	double phiInc = PI / resolution;
	double phiLimit = PI - phiInc / 2.0;
	double theta, phi;

	for (theta = 0.0; theta < thetaLimit; theta += thetaInc)
	{
		for (phi = 0.0; phi <= phiLimit; phi += phiInc)
		{
			vn[0][0] = cos(theta) * sin(phi);
			vn[0][1] = sin(theta) * sin(phi);
			vn[0][2] = cos(phi);
			uv[0] = svector(theta / (2 * PI), 1 - (phi / PI));

			vn[1][0] = cos(theta) * sin(phi + phiInc);
			vn[1][1] = sin(theta) * sin(phi + phiInc);
			vn[1][2] = cos(phi + phiInc);
			uv[1] = svector(theta / (2 * PI), 1 - ((phi + phiInc) / PI));

			vn[2][0] = cos(theta + thetaInc) * sin(phi + phiInc);
			vn[2][1] = sin(theta + thetaInc) * sin(phi + phiInc);
			vn[2][2] = cos(phi + phiInc);
			uv[2] = svector((theta + thetaInc) / (2 * PI), 1 - ((phi + phiInc) / PI));

			vn[3][0] = cos(theta + thetaInc) * sin(phi);
			vn[3][1] = sin(theta + thetaInc) * sin(phi);
			vn[3][2] = cos(phi);
			uv[3] = svector((theta + thetaInc) / (2 * PI), 1 - (phi / PI));

			for (int i = 0; i < 4; i++)
				p[i] = center + vn[i] * radius;
			pn = (vn[0] + vn[1] + vn[2] + vn[3]); // Take the average to be the face normal
			pn.normalize();

			shaderMap[curShader.top()]->DrawPolygon(4, p, vn, pn, uv); // Use the current shader to draw it
		}
	}
}

void sphere::Read(Loader& input)
{
	char token[MAX_INPUT_LENGTH];

	bool moreTokens;
	do
	{
		// We need to "peek" because if the token isn't giving us one
		// of the sphere properties, then it is a shape attribute like
		// color and if we've already read it out of the file, the shape
		// class would not be able to read the attribute afterwords.
		// So we "peek" at the token which does not remove it from the 
		// file stream.  For more information, see the "PeekTokens" 
		// method defined in the Loader class
		input.PeekTokens(token, 1);

		moreTokens = true;
		if (strcmp(token, "center") == 0)
		{
			input.ReadToken(token);
			for (int i = 0; i < 3; i++)
				if (!input.ReadDouble(center[i]))
					input.Error("Not enough parameters for sphere's center in sphere '%s'", objectName);
		}
		else if (strcmp(token, "radius") == 0)
		{
			input.ReadToken(token);
			if (!input.ReadDouble(radius))
				input.Error("Error reading radius in sphere '%s'", objectName);
		}
		else if (strcmp(token, "resolution") == 0)
		{
			input.ReadToken(token);
			if (!input.ReadInt(resolution))
				input.Error("Error reading resolution in sphere '%s'", objectName);
		}
		else
			moreTokens = false; // If we don't recognize it jump out ... it is an attribute
	} while (moreTokens);
}
