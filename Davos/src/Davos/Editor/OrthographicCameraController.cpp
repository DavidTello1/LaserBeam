#include "dvs_pch.h"
#include "OrthographicCameraController.h"

#include "Davos/Core/TimeStep.h"

#include "Davos/Core/Input.h"
#include "Davos/Core/KeyCodes.h"

#include "Davos/Events/ApplicationEvent.h"
#include "Davos/Events/MouseEvent.h"

namespace Davos {

	static constexpr float s_MinZoom = 0.25f;

	OrthographicCameraController::OrthographicCameraController(float aspectRatio) : m_AspectRatio(aspectRatio),
		m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
	{
	}

	void OrthographicCameraController::OnUpdate(TimeStep dt)
	{
		glm::vec3 initialPos = m_CameraPosition;

		if (Input::IsKeyPressed(Key::A))
			m_CameraPosition.x -= m_MoveSpeed * dt;
		else if (Input::IsKeyPressed(Key::D))
			m_CameraPosition.x += m_MoveSpeed * dt;

		if (Input::IsKeyPressed(Key::W))
			m_CameraPosition.y += m_MoveSpeed * dt;
		else if (Input::IsKeyPressed(Key::S))
			m_CameraPosition.y -= m_MoveSpeed * dt;

		if (m_CameraPosition == initialPos)
			return;

		m_Camera.SetPosition(m_CameraPosition);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>(DVS_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(DVS_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrollEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * m_ZoomSensitivity;
		m_ZoomLevel = std::max(m_ZoomLevel, s_MinZoom); //*** DVS_CLAMP, DVS_MIN, DVS_MAX
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		m_MoveSpeed = m_ZoomLevel; // move speed scales with zoom level

		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}
}
