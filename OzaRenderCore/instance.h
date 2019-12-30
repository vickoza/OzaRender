#pragma once
#include "shape.h"
class instance : public shape
{
public:

	instance(const char* masterName = "");
	void SetMaster(const char* masterName);
	virtual point Center(void) { return master->Center(); }
	virtual void MinMax(point& Min, point& Max) { master->MinMax(Min, Max); }
	virtual point Size(void) { return master->Size(); }
	void DrawShape(DrawingObject& dobj) override;
	void ReadObject(Loader& input) override;
	virtual void SetTime(double time) { master->SetTime(time); }

	shape* master;
};

