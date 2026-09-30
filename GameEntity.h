#pragma once
#include <memory>
#include "Transform.h"
#include "Mesh.h"

class GameEntity
{
private:
	std::unique_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
public:
	GameEntity(std::shared_ptr<Mesh> m);
	Transform& GetTransform();
	std::shared_ptr<Mesh> GetMesh();
	void Draw();
};
