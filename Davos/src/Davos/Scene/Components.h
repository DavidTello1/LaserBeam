#pragma once
#include "Davos/Renderer/OrthographicCamera.h"
#include "Davos/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Davos {

	struct C_Tag
	{
	public:
		C_Tag() = default;
		C_Tag(const C_Tag&) = default;
		C_Tag(const char* name) : name(name) {};

	public:
		std::string name;
	};

	//// ---
	//struct C_Hierarchy
	//{
	//public:
	//	C_Hierarchy() = default;
	//	C_Hierarchy(const C_Hierarchy&) = default;
	//	C_Hierarchy(const Entity& parent) : parent(parent) {}

	//public:
	//	Entity parent = MAX_ENTITIES;

	//	size_t numChilds = 0;
	//	Entity firstChild = MAX_ENTITIES;

	//	Entity nextSibling = MAX_ENTITIES;
	//	Entity prevSibling = MAX_ENTITIES;
	//};

	// ---
	struct C_Transform
	{
	public:
		C_Transform() = default;
		C_Transform(const C_Transform&) = default;
		C_Transform(const glm::vec3& translation) : translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::quat q = glm::quat(rotation);
			glm::mat4 rot = glm::toMat4(q);

			return glm::translate(glm::mat4(1.0f), translation) * rot * glm::scale(glm::mat4(1.0f), scale);
		}

		float GetLayer() const { return translation.z; }
		float SetLayer(float layer) { translation.z = layer; }

		const glm::vec2& GetPosition() const { return glm::vec2(translation.x, translation.y); }
		void SetPosition(float x, float y) { translation.x = x; translation.y = y; }
		void SetPosition(const glm::vec2& pos) { translation.x = pos.x; translation.y = pos.y; }

		float GetRotationDegrees() const { return glm::degrees(rotation.z); }
		void SetRotation(float degrees) { rotation.z = glm::radians(degrees); }

		const glm::vec2& GetScale() const { return glm::vec2(scale.x, scale.y); }
		void SetScale(float value) { scale.x = value; scale.y = value; }
		void SetScale(const glm::vec2& value) { scale.x = value.x; scale.y = value.y; }

	public:
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	};

	// ---
	struct C_Camera
	{
	public:
		C_Camera() = default;
		C_Camera(const C_Camera&) = default;

	public:
		bool isFixedAspectRatio = false;
		OrthographicCamera camera;
	};

	// ---
	struct C_SpriteRenderer
	{
	public:
		C_SpriteRenderer() = default;
		C_SpriteRenderer(const C_SpriteRenderer&) = default;
		C_SpriteRenderer(const glm::vec4& color) : color(color) {}

	public:
		Ref<Texture2D> texture;
		glm::vec4 color = glm::vec4(1.0f);
		float tilingFactor = 1.0f;
	};

	// ---
	struct C_LevelMap
	{
		using Tile = uint32_t; //***

	public:
		C_LevelMap() = default;
		C_LevelMap(const C_LevelMap&) = default;
		
		C_LevelMap(const glm::ivec2& gridSize, const glm::ivec2& cellSize)
			: gridSize(gridSize), cellSize(cellSize)
		{
			uint32_t numCells = gridSize.x * gridSize.y;
			cells.reserve(numCells);
			cells.assign(numCells, 0);
			
			walkabilityMap.reserve(numCells);
			walkabilityMap.assign(numCells, false);
		}

	public:
		glm::ivec2 gridSize = glm::ivec2(0);
		glm::ivec2 cellSize = glm::ivec2(0);

		//uint32_t startIndex; // entrance door
		//uint32_t endIndex;   // exit door

		//Ref<Tileset> tileset;

		std::vector<Tile> cells;
		std::vector<bool> walkabilityMap;

		glm::vec4 tintColor = glm::vec4(1.0f);

		bool isDrawGrid = false;
		glm::vec4 gridColor = glm::vec4(1.0f);
	};

	// ---
	class ScriptableEntity; // Forward declaration
	struct C_NativeScript
	{
	public:
		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(C_NativeScript*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](C_NativeScript* nsc) { delete nsc->instance; nsc->instance = nullptr; };
		}

	public:
		ScriptableEntity* instance = nullptr;
	};
	
}
