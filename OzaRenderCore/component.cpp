#include <ctype.h>
#include "component.h"
#include "Misc.h"
using namespace std;

component::component(std::string name) : shape("component")
{
	SetObjectName(name);
}

component::component(const component& c) : shape("component")
{
	SetObjectName(c.objectName);

	for (const auto& inst : c.instances)
		AddInstance(std::make_unique<instance>(*inst));
}

component& component::operator=(const component& c)
{
	Erase();

	SetObjectName(c.objectName);

	for (const auto& inst : c.instances)
		AddInstance(std::make_unique<instance>(*inst));
	return *this;
}

component::~component()
{
}

void component::Erase(void)
{
	instances.clear();
}

void component::AddInstance(std::unique_ptr<instance> c)
{
	instances.push_back(std::move(c));
}

void component::DrawShape(DrawingObject& dobj)
{
	for (auto& inst: instances)
		inst->DrawShape(dobj);
}
/*
bool component::FindShapeInHierarchy(shape* s, LinkedList<shape*> sList)
{
	sList.Append(this);
	if (s == this)
		return true;

	for (int i = 0; i < nInst; i++)
		if (instances[i]->FindShapeInHierarchy(s, sList))
			return true;

	sList.RemoveLast();
	return false;
}
*/
point component::Center(void)
{
	point Min, Max;
	point iMin, iMax;

	if (instances.size() < 1)
		return point(0, 0, 0);

	instances[0]->master->MinMax(iMin, iMax);
	Min[0] = iMin[0];
	Min[1] = iMin[1];
	Min[2] = iMin[2];

	Max[0] = iMax[0];
	Max[1] = iMax[1];
	Max[2] = iMax[2];
	for (auto& inst : instances)
	{
		inst->master->MinMax(iMin, iMax);

		Min[0] = min(Min[0], iMin[0]);
		Min[1] = min(Min[1], iMin[1]);
		Min[2] = min(Min[2], iMin[2]);

		Max[0] = max(Max[0], iMax[0]);
		Max[1] = max(Max[1], iMax[1]);
		Max[2] = max(Max[2], iMax[2]);
	}
	return (Min + Max) * .5;
}

point component::Size(void)
{
	point Min, Max;
	point iMin, iMax;

	if (instances.size() < 1)
		return point(0, 0, 0);

	instances[0]->master->MinMax(iMin, iMax);
	Min[0] = iMin[0];
	Min[1] = iMin[1];
	Min[2] = iMin[2];

	Max[0] = iMax[0];
	Max[1] = iMax[1];
	Max[2] = iMax[2];
	for (auto& inst : instances)
	{
		inst->master->MinMax(iMin, iMax);

		Min[0] = min(Min[0], iMin[0]);
		Min[1] = min(Min[1], iMin[1]);
		Min[2] = min(Min[2], iMin[2]);

		Max[0] = max(Max[0], iMax[0]);
		Max[1] = max(Max[1], iMax[1]);
		Max[2] = max(Max[2], iMax[2]);
	}
	return point(Max[0] - Min[0], Max[1] - Min[1], Max[2] - Min[2]);
}

void component::MinMax(point& Min, point& Max)
{
	point iMin, iMax;

	if (instances.size() < 1)
		Min = Max = point(0, 0, 0);

	instances[0]->master->MinMax(iMin, iMax);
	Min[0] = iMin[0];
	Min[1] = iMin[1];
	Min[2] = iMin[2];

	Max[0] = iMax[0];
	Max[1] = iMax[1];
	Max[2] = iMax[2];
	for (auto& instance : instances)
	{
		instance->master->MinMax(iMin, iMax);

		Min[0] = min(Min[0], iMin[0]);
		Min[1] = min(Min[1], iMin[1]);
		Min[2] = min(Min[2], iMin[2]);

		Max[0] = max(Max[0], iMax[0]);
		Max[1] = max(Max[1], iMax[1]);
		Max[2] = max(Max[2], iMax[2]);
	}
}

int component::NInstances(void)
{
	return instances.size();
}


void component::Read(Loader& input)
{
	int nComponents; 
	double count = 0;

	Erase();

	if (!input.ReadInt(nComponents))
		input.Error("Component command must be followed by a component count");

	char type[MAX_INPUT_LENGTH];
	for (int i = 0; i < nComponents; i++)
	{
		int oldPos = input.CurPos();
		input.ReadToken(type);
		if (strcmp(type, "object") == 0)
		{
			auto inst = std::make_unique<instance>();
			inst->ReadObject(input);
			AddInstance(std::move(inst));
		}
		else
		{
			input.MoveTo(oldPos);
			shape* s = shape::LoadShape(input);
			if (s != NULL)
			{
				auto inst = std::make_unique<instance>();
				inst->SetMaster(s->ObjectName().c_str());
				AddInstance(std::move(inst));
			}
			else
				input.Error(type, "Attributes started before all components were found: %s", type);
		}
	}
}

