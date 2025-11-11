#include "dvs_pch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Davos {

	OrthographicCamera::OrthographicCamera()
	{
		RecalculateProjection();
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearClip, float farClip)
	{
		SetProjection(left, right, bottom, top, nearClip, farClip);
	}

	OrthographicCamera::OrthographicCamera(float size, float nearClip, float farClip)
	{
		SetProjection(size, nearClip, farClip);
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top, float nearClip, float farClip)
	{
		DVS_CORE_ASSERT(right > 0 && top > 0);

		m_Near = nearClip;
		m_Far = farClip;
		m_AspectRatio = right / top;
		m_Size = top * 2.0f;

		m_Projection = glm::ortho(left, right, bottom, top, nearClip, farClip);
	}

	void OrthographicCamera::SetProjection(float size, float nearClip, float farClip)
	{
		m_Size = size;
		m_Near = nearClip;
		m_Far = farClip;

		RecalculateProjection();
	}

	void OrthographicCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		DVS_CORE_ASSERT(width > 0 && height > 0);

		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	// --------------------------------------------------
	void OrthographicCamera::RecalculateProjection()
	{
		float left = -m_Size * m_AspectRatio * 0.5f;
		float right = m_Size * m_AspectRatio * 0.5f;
		float bottom = -m_Size * 0.5f;
		float top = m_Size * 0.5f;

		m_Projection = glm::ortho(left, right, bottom, top, m_Near, m_Far);
	}

}
