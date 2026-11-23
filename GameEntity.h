#pragma once
#include <memory>
#include <string>
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

#include "Inspectable.h"

class GameEntity : public Inspectable
{
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
public:
	GameEntity(std::string _name, std::shared_ptr<Mesh> startMesh, std::shared_ptr<Material> startMaterial);
	std::shared_ptr <Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();
	std::shared_ptr<Mesh> GetMesh();

	void SetMaterial(std::shared_ptr<Material> newMaterial);

	void Draw();

	void Inspector() override;
};
