#pragma once
#include <Davos.h>
#include <glm/glm.hpp>

namespace Davos {

	class Framebuffer;

	class PanelViewport
	{
	public:
		PanelViewport() = default;
		~PanelViewport() = default;

		void OnInit();
		void OnRender();

		void OnResize();

		void BindFramebuffer() const;
		void UnbindFramebuffer() const;

		bool IsFocused() const { return m_ViewportFocused; }
		bool IsHovered() const { return m_ViewportHovered; }

	private:
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		Ref<Framebuffer> m_Framebuffer;
	};

}
