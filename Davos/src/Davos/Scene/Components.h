#pragma once
#include "Davos/Editor/OrthographicCameraController.h"
#include "Davos/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Davos {

	//struct C_Tag
	//{

	//};

	//struct C_Hierarchy
	//{

	//};

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

	public:
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	};

	//struct C_Camera
	//{
	//public:
	//	C_Camera() = default;
	//	C_Camera(const C_Camera&) = default;

	//public:
	//	bool isFixedAspectRatio = false;
	//	SceneCamera camera;
	//};

	struct C_CameraController
	{
	public:
		C_CameraController() = default;
		C_CameraController(const C_CameraController&) = default;
		C_CameraController(float aspectRatio) : controller(aspectRatio) {}

	public:
		OrthographicCameraController controller;
	};

	struct C_SpriteRenderer
	{
	public:
		C_SpriteRenderer() = default;
		C_SpriteRenderer(const C_SpriteRenderer&) = default;
		C_SpriteRenderer(const glm::vec4& color) : color(color) {}

	public:
		Ref<Texture2D> texture;
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float tilingFactor = 1.0f;
	};

}
