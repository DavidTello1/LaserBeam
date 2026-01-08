#pragma once
#include "Davos/Editor/EditorCamera.h"

#include <Davos.h>
#include <glm/glm.hpp>

namespace Davos {

	class KeyPressEvent;

	class PanelViewport
	{
	public:
		PanelViewport() = default;
		~PanelViewport() = default;

		void OnInit();
		void OnUpdate(TimeStep dt);

		void OnImGuiRender(const Ref<Scene>& scene, Entity selectedEntity);
		void OnEvent(Event& e);

		void OnShortcuts(KeyCode key, bool isControl, bool isShift);

		bool OnResize();

		EditorCamera GetEditorCamera() const { return m_EditorCamera; }

		// --- Framebuffer
		void BindFramebuffer() const;
		void UnbindFramebuffer() const;
		void ClearFramebufferAttachment(uint32_t index, int value);
		int ReadPixel(uint32_t attachmentID, int x, int y);

		// --- Panel
		bool IsFocused() const { return m_IsFocused; }
		bool IsHovered() const { return m_IsHovered; }

		const glm::vec2& GetPanelSize() const { return m_PanelSize; }
		const glm::vec2& GetPanelPosition() const { return m_PanelPosition; }

		// --- Gizmos
		bool IsOverGizmo() const;

	private:
		void _DrawToolbar();

		void _RenderGizmos(const Ref<Scene>& scene, Entity selectedEntity);
		void _RenderGrid();

	private:
		bool m_IsFocused = false;
		bool m_IsHovered = false;
		glm::vec2 m_PanelSize = glm::vec2(0.0f);
		glm::vec2 m_PanelPosition = glm::vec2(0.0f);

		Ref<Framebuffer> m_Framebuffer;

		EditorCamera m_EditorCamera;

		int m_GizmoType = -1;
		float m_SnapDegrees = 15.0f;

		bool m_IsDrawGrid = true;
		float m_GridSize = 1.0f;
		glm::vec4 m_GridColor = glm::vec4(0.4f);

		bool m_IsDrawAxis = true;
	};

}
