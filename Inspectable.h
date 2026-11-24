#pragma once
#include <memory>
#include <string>
#include <type_traits>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <vector>


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

class Inspectable
{
public:
	std::string name;
	virtual ~Inspectable() = default;
	virtual void Inspector() = 0;
};

template <typename T>
class Registered : public Inspectable
{
private:
	static inline std::unordered_map<std::string, std::shared_ptr<T>> instances;

public:
	template <typename... Args>
	static std::shared_ptr<T> Create(const std::string& key, Args&&... args)
	{
		auto instance = std::make_shared<T>(key, std::forward<Args>(args)...);
		instances[key] = instance;
		return instance;
	}

	static std::shared_ptr<T> Get(const std::string& key)
	{
		auto it = instances.find(key);
		if (it != instances.end()) return it->second;

		if constexpr (std::is_constructible_v<T, const std::string&>) return Create(key);
		else return nullptr;
	}

	static std::vector<std::shared_ptr<T>> GetInstances()
	{
		std::vector<std::shared_ptr<T>> values;
		values.reserve(instances.size());
		for (const auto& [key, value] : instances)
			values.push_back(value);
		return values;
	}

	static size_t Count() { return instances.size(); }
	static void Clear() { instances.clear(); }

	static void InspectAll()
	{
		T::InspectStatics();
		for (auto& [key, instance] : instances)
			instance->Inspector();
	}

	static void InspectStatics() {}
};