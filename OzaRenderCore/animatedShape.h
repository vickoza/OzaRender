#pragma once
#include "shape.h"
#include "Interpolator.h"
#include "QuaternionInterplotator.h"
#include "matrixRotationInterp.h"

class animatedShape : public shape
{
public:

	animatedShape();

	void SetTime(double time);

	void SetShape(shape* S) { s = S; }
	void SetPositionInterp(Interpolator* interp) { iPosition = interp; }
	void SetScaleInterp(Interpolator* interp) { iScale = interp; }
	void SetRotationInterp(Interpolator* interp) { iRotation = interp; }
	void SetRotationInterpQ(QuaternionInterplotator* interp) { iRotationQ = interp; }

	Interpolator* GetPositionInterp(void) { return iPosition; }
	Interpolator* GetScaleInterp(void) { return iScale; }
	Interpolator* GetRotationInterp(void) { return iRotation; }
	QuaternionInterplotator* GetRotationInterpQ(void) { return iRotationQ; }

	virtual void Read(Loader& input);
	virtual void ApplyTransforms(DrawingObject& dobj) override;
	virtual void DrawShape(DrawingObject& dobj) override;

	virtual point Center(void) { return s->Center(); }
	virtual point Size(void) { return s->Size(); }
	virtual void MinMax(point& Min, point& Max) { s->MinMax(Min, Max); }

private:

	shape* s;

	Interpolator* iPosition;
	Interpolator* iScale;
	Interpolator* iRotation;  // Interpolator for Euler Angle Rotation
	QuaternionInterplotator* iRotationQ;  // Interpolator for Quateran Rotation
	matrixRotationInterp* iMatrix;

	double time;
};

