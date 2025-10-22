#pragma once
#include "Davos/Renderer/OrthographicCamera.h"

namespace Davos {

	class TimeStep;
	class Event;
	class MouseScrollEvent;
	class WindowResizeEvent;

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio);
		~OrthographicCameraController() = default;

		void OnUpdate(TimeStep dt);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }
		float GetMoveSpeed() const { return m_MoveSpeed; }
		float GetZoomSensitivity() const { return m_ZoomSensitivity; }

		void SetZoomLevel(float value) { m_ZoomLevel = value; }
		void SetMoveSpeed(float value) { m_MoveSpeed = value; }
		void SetZoomSensitivity(float value) { m_ZoomSensitivity = value; }

	private:
		bool OnMouseScrolled(MouseScrollEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;

		float m_MoveSpeed = 5.0f;
		float m_ZoomSensitivity = 0.25f;

		glm::vec3 m_CameraPosition = glm::vec3(0.0f);

		OrthographicCamera m_Camera;
	};

}
