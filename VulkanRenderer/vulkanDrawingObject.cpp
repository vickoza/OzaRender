#include "pch.h"
#include "vulkanDrawingObject.h"

void vulkanDrawingObject::ApplyTransforms(const std::array<double, 4>& eulerRot, const std::array<double, 4>& translation, const std::array<double, 4>& scale)
{
}

void vulkanDrawingObject::ApplyTranslation(const point& pnt)
{
}

void vulkanDrawingObject::ApplyRotationAxis(double rot, const svector& pnt)
{
}

void vulkanDrawingObject::ApplyScaling(const point& pnt)
{
}

void vulkanDrawingObject::ApplyMaterial(const std::array<float, 4>& ambient, const bool& ambientSet, const std::array<float, 4>& diffuse, const bool& diffuseSet, const std::array<float, 4>& specular, const bool& specularSet, const float& shininess, const bool& shininessSet)
{
}

void vulkanDrawingObject::DrawMesh(mesh& m, shader& sh)
{
}

void vulkanDrawingObject::DrawSphere(sphere& sphr, shader& sh)
{
}
