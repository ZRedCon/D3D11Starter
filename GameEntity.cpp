#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> startMesh, std::shared_ptr<Material> startMaterial)
{
	mesh = startMesh;
	material = startMaterial;
	transform = std::make_shared<Transform>();
}

std::shared_ptr<Transform> GameEntity::GetTransform() { return transform; }
std::shared_ptr<Material> GameEntity::GetMaterial() { return material; }
std::shared_ptr<Mesh> GameEntity::GetMesh() { return mesh; }
void GameEntity::SetMaterial(std::shared_ptr<Material> newMaterial) { material = newMaterial; }
void GameEntity::Draw() { mesh->Draw(); }

void GameEntity::Inspector(std::string name)
{
	if (ImGui::CollapsingHeader(("Entity " + name + ":").c_str()))
	{
		ImGui::Indent();
		transform->Inspector(name);
		mesh->Inspector(name);
		material->Inspector(name);
		ImGui::Unindent();
	}
}
