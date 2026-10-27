#pragma once
#include <memory>
#include <string>
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

class GameEntity
{
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
public:
	GameEntity(std::shared_ptr<Mesh> startMesh, std::shared_ptr<Material> startMaterial);
	std::shared_ptr <Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();
	std::shared_ptr<Mesh> GetMesh();

	void SetMaterial(std::shared_ptr<Material> newMaterial);

	void Draw();

	void Inspector(std::string name);
};
