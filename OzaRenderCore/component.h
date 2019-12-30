#pragma once
#include <vector>
#include <memory>
#include "shape.h"
#include "instance.h"
class component : public shape
{
public:

	component(std::string name = "component");
	component(const component& c);
	~component();

	component& operator=(const component& c);

	void AddInstance(std::unique_ptr<instance> c);
	int NInstances(void);

	void DrawShape(DrawingObject& dobj);
	void Erase(void);
	void Read(Loader& input);

	//bool FindShapeInHierarchy(shape* s, LinkedList<shape*> sList);

	virtual point Center(void);
	virtual point Size(void);
	virtual void MinMax(point& min, point& max);

	virtual void SetTime(double time) { for (auto& instance : instances) instance->SetTime(time); }

protected:

	std::vector<std::unique_ptr<instance>> instances;
};
