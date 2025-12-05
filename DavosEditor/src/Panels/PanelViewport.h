#pragma once
#include <Davos.h>
#include <glm/glm.hpp>

namespace Davos {

	class PanelViewport
	{
	public:
		PanelViewport() = default;
		~PanelViewport() = default;

		void OnInit();
		void OnRender();

		bool OnResize();

		void BindFramebuffer() const;
		void UnbindFramebuffer() const;

		bool IsFocused() const { return m_ViewportFocused; }
		bool IsHovered() const { return m_ViewportHovered; }

		const glm::vec2& GetViewportSize() const { return m_ViewportSize; }
		const glm::vec2& GetPanelPosition() const { return m_PanelPosition; }

		// --- Grid
		bool IsDrawGrid() const { return m_IsDrawGrid; }

		const glm::vec4& GetGridColor() const { return m_GridColor; }
		void SetGridColor(const glm::vec4& color) { m_GridColor = color; }

		float GetGridSize() const { return m_GridSize; }
		void SetGridSize(float size) { if (size >= 1.0f) m_GridSize = size; }

	private:
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec2 m_PanelPosition = glm::vec2(0.0f);

		Ref<Framebuffer> m_Framebuffer;

		bool m_IsDrawGrid = true;
		float m_GridSize = 1.0f;
		glm::vec4 m_GridColor = glm::vec4(0.4f);
	};

}
