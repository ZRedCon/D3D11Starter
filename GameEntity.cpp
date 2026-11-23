#include "GameEntity.h"

GameEntity::GameEntity(std::string _name, std::shared_ptr<Mesh> startMesh, std::shared_ptr<Material> startMaterial)
{
	name = _name;
	mesh = startMesh;
	material = startMaterial;
	transform = std::make_shared<Transform>(_name);
}

std::shared_ptr<Transform> GameEntity::GetTransform() { return transform; }
std::shared_ptr<Material> GameEntity::GetMaterial() { return material; }
std::shared_ptr<Mesh> GameEntity::GetMesh() { return mesh; }
void GameEntity::SetMaterial(std::shared_ptr<Material> newMaterial) { material = newMaterial; }
void GameEntity::Draw() { mesh->Draw(); }

void GameEntity::Inspector()
{
	if (ImGui::CollapsingHeader(("Entity " + name + ":").c_str()))
	{
		ImGui::PushID(this);
		ImGui::Indent();
		transform->Inspector();
		mesh->Inspector();
		material->Inspector();
		ImGui::Unindent();
		ImGui::PopID();
	}
}
