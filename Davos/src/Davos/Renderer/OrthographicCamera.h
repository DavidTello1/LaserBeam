#pragma once
#include "Camera.h"

namespace Davos {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera();
		OrthographicCamera(float left, float right, float bottom, float top, float nearClip = -1.0f, float farClip = 1.0f);
		OrthographicCamera(float size, float nearClip, float farClip);

		virtual ~OrthographicCamera() = default;

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetProjection(float left, float right, float bottom, float top, float nearClip = -1.0f, float farClip = 1.0f);
		void SetProjection(float size, float nearClip, float farClip);

		// --- Getters
		float GetSize() const { return m_Size; }
		float GetNearClip() const { return m_Near; }
		float GetFarClip() const { return m_Far; }
		
		// --- Setters
		void SetSize(float size) { m_Size = size; RecalculateProjection(); }
		void SetNearClip(float nearClip) { m_Near = nearClip; RecalculateProjection(); }
		void SetFarClip(float farClip) { m_Far = farClip; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		float m_AspectRatio = 0.0f;

		float m_Size = 10.0f;
		float m_Near = -1.0f;
		float m_Far = 1.0f;
	};

}
