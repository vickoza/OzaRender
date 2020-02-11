#include "pch.h"
#include "vulkanRenderEngine.h"
#include "vulkanDrawingObject.h"

std::unique_ptr<DrawingObject> vulkanRenderEngine::createShapeDraw()
{
	return std::make_unique<vulkanDrawingObject>();
}

void vulkanRenderEngine::loadShaders(std::unordered_map<shaderType, std::unique_ptr<shader>>& shaderMap)
{
}
