#pragma once
#include "Camera.h"

namespace Davos {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float Near = -1.0f, float Far = 1.0f);

		void SetProjection(float left, float right, float bottom, float top, float Near = -1.0f, float Far = 1.0f);

		const glm::vec3& GetPosition() const { return m_Position; }
		const float GetRotation() const { return m_Rotation; }

		void SetPosition(const glm::vec3& position) {
			m_Position = position;
			RecalculateViewMatrix();
		}

		void SetRotation(float rotation) {
			m_Rotation = rotation;
			RecalculateViewMatrix();
		}

	private:
		void RecalculateViewMatrix();

	private:
		glm::vec3 m_Position = glm::vec3(0.0f);
		float m_Rotation = 0.0f;
	};

}
