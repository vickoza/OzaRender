#include <iostream>
#include "animatedShape.h"
#include "QuaternionInterplotator.h"

animatedShape::animatedShape() : shape("animatedshape"), iPosition(nullptr),  iScale(nullptr), iRotation(nullptr), iRotationQ(nullptr), time(0.0)
{
}

void animatedShape::SetTime(double t)
{
	time = t;
}

void animatedShape::ApplyTransforms(DrawingObject& dobj)
{
	// In this overloaded ApplyTransforms method
	// we need to look at each of the three interpolation
	// pointers
	// 
	//	iPosition
	//  iRotation
	//  iScale
	//
	// and if there is an interpolator assigned (i.e. the
	// pointer is not nullptr) then we need to evaluate it 
	// at the current time which is stored in the member
	// variable "time", and apply the appropriate transformation
	// in OpenGL.
	point p;
	// set the position base on the position keys
	if (iPosition != nullptr)
	{
		p = iPosition->Evaluate(time);
		dobj.ApplyTranslation(p);
		if ((iRotation != nullptr) && (iRotationQ != nullptr))
		{
			svector up = iPosition->Up(time, .001);
			up.normalize();
			svector in = iPosition->Velocity((time - .001), .001);
			svector out = iPosition->Velocity((time + .001), .001);
			in.normalize();
			out.normalize();
			double r = acos(dot(in, out)) * 180 / PI;
			dobj.ApplyRotationAxis(r, up);
		}
	}
	// applies the rotation based on the rotation keys using eular angle rotation
	if (iRotation != nullptr)
	{
		p = iRotation->Evaluate(time);
		dobj.ApplyRotationAxis(p[0], {1,0,0});
		dobj.ApplyRotationAxis(p[1], {0,1,0});
		dobj.ApplyRotationAxis(p[2], {0,0,1});
	}
	// applies the rotation based on the rotation keys quaterants
	if (iRotationQ != nullptr)
	{
		double th;
		auto q = iRotationQ->Evaluate(time);
		std::cout << "q is " << q << "\n";
		th = acos(q.w) * 360 / PI;
		auto v = q.v;
		v.normalize();
		dobj.ApplyRotationAxis(th, v);
	}

	// set the scales based on the scale key
	if (iScale != nullptr)
	{
		p = iScale->Evaluate(time);
		dobj.ApplyScaling(p);
	}
	PopMatrix();
}

void animatedShape::DrawShape(DrawingObject& dobj)
{
	s->DrawShape(dobj);
}

void animatedShape::Read(Loader& input)
{
	char token[256], name[256];

	s = NULL;
	do
	{
		input.PeekTokens(token, 1);

		if (strcmp(token, "object") == 0)
		{
			input.ReadToken(token);

			if (s != NULL)
				input.Error("Only one shape allowed in an AnimatedShape object");

			input.ReadToken(name);
			s = FindShape(name);
			if (s == NULL)
				input.Error("Couldn't find shape '%s' referenced in AnimatedShape '%s'", name, objectName);
		}
		else if (strncmp(token, "pos", 3) == 0)
		{
			input.ReadToken(token);
			iPosition = Interpolator::LoadInterpolator(input);
			if (iPosition->NKeys() == 0)
			{
				delete iPosition;
				iPosition = NULL;
			}
		}
		else if (strncmp(token, "sc", 2) == 0)
		{
			input.ReadToken(token);
			iScale = Interpolator::LoadInterpolator(input);
			if (iScale->NKeys() == 0)
			{
				delete iScale;
				iScale = NULL;
			}
		}
		else if (strncmp(token, "rot", 3) == 0)
		{
			input.ReadToken(token);
			iRotation = Interpolator::LoadInterpolator(input);
			if (iRotation->NKeys() == 0)
			{
				delete iRotation;
				iRotation = NULL;
			}
		}
		else if (strncmp(token, "quatinterpolator", 16) == 0)
		{
			input.ReadToken(token);
			iRotationQ = QuaternionInterplotator::LoadQuaternionInterpolator(input);
			if (iRotationQ->NKeys() == 0)
			{
				delete iRotationQ;
				iRotationQ = NULL;
			}
		}
		else if (strncmp(token, "mat", 3) == 0)
		{
			input.ReadToken(token);
			iMatrix = new matrixRotationInterp;

			iMatrix->Load(input);
		}
		else if (strcmp(token, "end") == 0)
			break;
		else
			input.Error(token, "Unknown command '%s' in AnimatedShape '%s'", token, objectName);
	} while (true);
}
