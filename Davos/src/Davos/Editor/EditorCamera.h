#pragma once
#include "Davos/Renderer/OrthographicCamera.h"

namespace Davos {

	class TimeStep;
	class Event;
	class MouseScrollEvent;
	class WindowResizeEvent;

	class EditorCamera
	{
	public:
		EditorCamera() = default;
		EditorCamera(glm::vec3 position, float rotation, float zoom, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(TimeStep dt);
		void OnEvent(Event& e);

		// --- Viewport
		const glm::vec4& GetViewportBounds() const;
		const glm::vec2& GetViewportSize() const { return glm::vec2(m_ViewportWidth, m_ViewportHeight); }
		const glm::vec2& GetViewportOffset() const { return m_ViewportOffset; }
		void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; _UpdateProjection(); }
		void SetViewportOffset(const glm::vec2& offset) { m_ViewportOffset = offset; }

		// --- Matrices
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

		// --- Getters
		const glm::vec3& GetPosition() const { return m_Position; }
		float GetRotation() const { return m_Rotation; }
		float GetRotationDegrees() const { return glm::degrees(m_Rotation); }
		float GetZoom() const { return m_Zoom; }

		// --- Setters
		void SetPosition(glm::vec3 position) { m_Position = position; _UpdateView(); }
		void SetRotation(float rotation) { m_Rotation = rotation; _UpdateView(); }
		void SetRotationDegrees(float rotation_degrees) { m_Rotation = glm::radians(rotation_degrees); _UpdateView(); }
		void SetZoom(float zoom) { m_Zoom = zoom; _UpdateView(); }

	private:
		bool _OnMouseScrolled(MouseScrollEvent& e);

		void _UpdateProjection();
		void _UpdateView();

		// --- Utils
		glm::vec2 _ScreenToWorld(const glm::vec2& screenPos);

	private:
		glm::vec3 m_Position = glm::vec3(0.0f);
		float m_Rotation = 0.0f; // in radians
		float m_Zoom = 0.0f;

		float m_AspectRatio = 1.778f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;

		float m_ViewportWidth = 1280;
		float m_ViewportHeight = 720;

		glm::vec2 m_ViewportOffset = glm::vec2(0.0f);

		bool m_IsPanning = false;
		glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	};

}
