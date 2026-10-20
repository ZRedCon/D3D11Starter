#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> m)
{
	mesh = m;
	transform = std::make_shared<Transform>();
}

std::shared_ptr<Transform> GameEntity::GetTransform() { return transform; }
std::shared_ptr<Mesh> GameEntity::GetMesh() { return mesh; }
void GameEntity::Draw() { mesh->Draw(); }