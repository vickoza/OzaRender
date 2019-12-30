#pragma once
#include <array>
#include <memory>
#include "shaderInterface.h"

class mesh;
class sphere;

class DrawingObject
{
public:
	virtual ~DrawingObject() {};
	virtual void ApplyTransforms(const std::array<double, 3>& eulerRot, const std::array<double, 3>& translation, const std::array<double, 3>& scale) = 0;
	virtual void ApplyTranslation(const point& pnt) = 0;
	virtual void ApplyRotationAxis(double rot, const svector& pnt) = 0;
	virtual void ApplyScaling(const point& pnt) = 0;
	virtual void ApplyMaterial(const std::array<float, 3>& ambient, const bool& ambientSet, const std::array<float, 3>& diffuse, const bool& diffuseSet, const std::array<float, 3>& specular, const bool& specularSet, const float& shininess, const bool& shininessSet) = 0;
	virtual void DrawMesh(mesh& m, shader& sh) = 0;
	virtual void DrawSphere(sphere& sphr, shader& sh) = 0;
};

class renderEngine
{
public:
	virtual ~renderEngine() {}
	virtual std::unique_ptr<DrawingObject> createShapeDraw() = 0;
	virtual void loadShaders(std::unordered_map<shaderType, std::unique_ptr<shader>>& shaderMap) = 0;
};

