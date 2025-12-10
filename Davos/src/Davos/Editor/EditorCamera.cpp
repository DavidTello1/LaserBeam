#include "dvs_pch.h"
#include "EditorCamera.h"

#include "Davos/Core/TimeStep.h"

#include "Davos/Core/Input.h"
//#include "Davos/Core/KeyCodes.h"

#include "Davos/Events/ApplicationEvent.h"
#include "Davos/Events/MouseEvent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Davos {

	static constexpr float s_MinZoom = 1.0f;
	static constexpr float s_MaxZoom = 20.0f;

	// ---------------------------------
	EditorCamera::EditorCamera(glm::vec3 position, float rotation, float zoom, float aspectRatio, float nearClip, float farClip)
		: m_Position(position), m_Rotation(rotation), m_Zoom(zoom), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		_UpdateProjection();
		_UpdateView();
	}

	void EditorCamera::OnUpdate(TimeStep dt)
	{
		// Panning
		if (Input::IsMouseButtonPressed(Mouse::BUTTON_MIDDLE))
		{
			const glm::vec2& mouse = Input::GetMousePosition();
			glm::vec2 mouseWorld = _ScreenToWorld(mouse);

			if (!m_IsPanning)
			{
				m_InitialMousePosition = mouseWorld;
				m_IsPanning = true;
				return;
			}
			
			glm::vec2 delta = m_InitialMousePosition - mouseWorld;
			if (delta == glm::vec2(0.0f))
				return;

			m_Position.x += delta.x;
			m_Position.y += delta.y;

			_UpdateView();

			m_InitialMousePosition = _ScreenToWorld(mouse);
		}
		else if (Input::IsMouseButtonReleased(Mouse::BUTTON_MIDDLE))
		{
			m_IsPanning = false;
		}
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>(DVS_BIND_EVENT_FN(EditorCamera::_OnMouseScrolled));
		//dispatcher.Dispatch<WindowResizeEvent>(DVS_BIND_EVENT_FN(EditorCamera::_OnWindowResized));
	}

	const glm::vec4& EditorCamera::GetViewportBounds() const
	{
		float left = m_Position.x - m_AspectRatio * m_Zoom;
		float right = m_Position.x + m_AspectRatio * m_Zoom;
		float bottom = m_Position.y - m_Zoom;
		float top = m_Position.y + m_Zoom;

		return { top, left, bottom, right };
	}

	// --------------------------------------------------
	bool EditorCamera::_OnMouseScrolled(MouseScrollEvent& e)
	{
		glm::vec2 mouseScreen = Input::GetMouseScreenPos();
		glm::vec2 mouseViewport = mouseScreen - m_ViewportOffset;

		float ndcX = (2.0f * mouseViewport.x) / m_ViewportWidth - 1.0f;
		float ndcY = 1.0f - (2.0f * mouseViewport.y) / m_ViewportHeight; // inverted Y
		glm::vec4 clipPos(ndcX, ndcY, 0.0f, 1.0f);
		glm::mat4 invViewProj = glm::inverse(m_ProjectionMatrix * m_ViewMatrix);
		glm::vec4 worldPos = invViewProj * clipPos;
		worldPos /= worldPos.w;

		m_Zoom -= e.GetYOffset();
		m_Zoom = glm::clamp(m_Zoom, s_MinZoom, s_MaxZoom);
		_UpdateProjection();

		glm::vec4 offset = glm::inverse(m_ProjectionMatrix) * clipPos;
		offset /= offset.w;

		m_Position.x = worldPos.x - offset.x;
		m_Position.y = worldPos.y - offset.y;

		_UpdateView();

		return false;
	}

	// --------------------------------------------------
	void EditorCamera::_UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_ProjectionMatrix = glm::ortho(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom, m_NearClip, m_FarClip);
	}

	void EditorCamera::_UpdateView()
	{
		glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, m_Rotation));;
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	glm::vec2 EditorCamera::_ScreenToWorld(const glm::vec2& screenPos)
	{
		glm::mat4 inverse = glm::inverse(m_ProjectionMatrix * m_ViewMatrix);

		float x = (2.0f * screenPos.x) / m_ViewportWidth - 1.0f;
		float y = 1.0f - (2.0f * screenPos.y) / m_ViewportHeight; // Inverted Y axis
		glm::vec4 clipCoords(x, y, 0.0f, 1.0f);

		glm::vec4 worldCoords = inverse * clipCoords;
		worldCoords /= worldCoords.w;

		return glm::vec2(worldCoords.x, worldCoords.y);
	}

}
