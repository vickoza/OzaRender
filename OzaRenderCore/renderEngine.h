#pragma once
class DrawingObject
{
public:
	virtual ~DrawingObject() {};
};

class renderEngine
{
public:
	virtual ~renderEngine() {}
	virtual DrawingObject createShapeDraw() = 0;
};

