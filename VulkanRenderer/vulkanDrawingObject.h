#pragma once
#include "renderEngine.h"
class vulkanDrawingObject : public DrawingObject
{
	virtual void ApplyTransforms(const std::array<double, 4>& eulerRot, const std::array<double, 4>& translation, const std::array<double, 4>& scale) override;
	virtual void ApplyTranslation(const point& pnt) override;
	virtual void ApplyRotationAxis(double rot, const svector& pnt) override;
	virtual void ApplyScaling(const point& pnt) override;
	virtual void ApplyMaterial(const std::array<float, 4>& ambient, const bool& ambientSet, const std::array<float, 4>& diffuse, const bool& diffuseSet, const std::array<float, 4>& specular, const bool& specularSet, const float& shininess, const bool& shininessSet) override;
	virtual void DrawMesh(mesh& m, shader& sh) override;
	virtual void DrawSphere(sphere& sphr, shader& sh) override;
};

