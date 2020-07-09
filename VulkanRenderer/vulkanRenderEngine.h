#pragma once
#include "renderEngine.h"

class vulkanRenderEngine : public renderEngine
{
	virtual std::unique_ptr<DrawingObject> createShapeDraw() override;
	virtual void loadShaders(std::unordered_map<shaderType, std::unique_ptr<shader>>& shaderMap) override;
};

