#include "PanelViewport.h"

#include <Davos/Math/Math.h>
#include <Davos/Editor/EditorCamera.h>
#include <Davos/Events/KeyEvent.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGuizmo/ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>

namespace Davos {

	void PanelViewport::OnInit()
	{
		// --- Framebuffer
		FB_Specs fbSpec;
		fbSpec.attachments = {
			{ FB_TextureFormat::RGBA8,		 FB_TextureFilterMin::LINEAR, FB_TextureFilterMag::LINEAR, FB_TextureWrap::CLAMP_TO_EDGE },
			{ FB_TextureFormat::RED_INTEGER, FB_TextureFilterMin::LINEAR, FB_TextureFilterMag::LINEAR, FB_TextureWrap::CLAMP_TO_EDGE },
			{ FB_TextureFormat::DEPTH,		 FB_TextureFilterMin::LINEAR, FB_TextureFilterMag::LINEAR, FB_TextureWrap::CLAMP_TO_EDGE }
		};
		fbSpec.width = 1280;
		fbSpec.height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		// --- Editor Camera
		m_EditorCamera = EditorCamera(glm::vec3(0.0f), 0.0f, 10.0f, 1.778f, -1.0f, 1000.0f);
	}

	void PanelViewport::OnUpdate(TimeStep dt)
	{
		m_EditorCamera.OnUpdate(dt);
		_RenderGrid();
	}

	void PanelViewport::OnImGuiRender(const Ref<Scene>& scene, Entity selectedEntity)
	{
		 // --- Viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		bool isActive = ImGui::Begin("Viewport");

		ImVec2 viewportPos = ImGui::GetCursorScreenPos();
		m_PanelPosition = { viewportPos.x, viewportPos.y };

		m_IsFocused = ImGui::IsWindowFocused();
		m_IsHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_IsHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_PanelSize = { viewportSize.x, viewportSize.y };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(m_PanelSize.x, m_PanelSize.y), ImVec2(0, 1), ImVec2(1, 0));

		// Gizmos
		_RenderGizmos(scene, selectedEntity);

		ImGui::End();
		ImGui::PopStyleVar();

		// --- Toolbar
		if (isActive)
			_DrawToolbar();

		m_EditorCamera.SetViewportOffset(m_PanelPosition);
	}

	void PanelViewport::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);
	}

	void PanelViewport::OnShortcuts(KeyCode key, bool isControl, bool isShift)
	{
		switch (key)
		{
			// --- Gizmos
			case Key::Q:
			{
				if (!ImGuizmo::IsUsing()) { m_GizmoType = -1; }
				break;
			}
			case Key::W:
			{
				if (!ImGuizmo::IsUsing()) { m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; }
				break;
			}
			case Key::E:
			{
				if (!ImGuizmo::IsUsing()) { m_GizmoType = ImGuizmo::OPERATION::ROTATE; }
				break;
			}
			case Key::R:
			{
				if (!ImGuizmo::IsUsing()) { m_GizmoType = ImGuizmo::OPERATION::SCALE; }
				break;
			}
		}
	}

	bool PanelViewport::OnResize()
	{
		if (FB_Specs spec = m_Framebuffer->GetSpecification();
			m_PanelSize.x > 0.0f && m_PanelSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != m_PanelSize.x || spec.height != m_PanelSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_PanelSize.x, (uint32_t)m_PanelSize.y);
			m_EditorCamera.SetViewportSize(m_PanelSize.x, m_PanelSize.y);

			return true;
		}

		return false;
	}

	void PanelViewport::BindFramebuffer() const
	{
		m_Framebuffer->Bind();
	}

	void PanelViewport::UnbindFramebuffer() const
	{
		m_Framebuffer->Unbind();
	}

	void PanelViewport::ClearFramebufferAttachment(uint32_t index, int value)
	{
		m_Framebuffer->ClearAttachment(index, value);
	}

	int PanelViewport::ReadPixel(uint32_t attachmentID, int x, int y)
	{
		return m_Framebuffer->ReadPixel(attachmentID, x, y);
	}

	// --------------------------------------------------
	bool PanelViewport::IsOverGizmo() const
	{
		return ImGuizmo::IsOver();
	}

	// --------------------------------------------------
	void PanelViewport::_DrawToolbar()
	{
		static const ImVec2 margin = ImVec2(10, 10);
		static const ImVec2 buttonSize = ImVec2(25, 25);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

		ImGui::SetNextWindowPos(ImVec2(m_PanelPosition.x + margin.x, m_PanelPosition.y + margin.y));
		//ImGui::SetNextWindowSize(ImVec2(m_PanelSize.x - margin.x * 2, 0));

		ImGui::Begin("Gizmo Toolbar", nullptr, flags);

		if (ImGui::Button("Sel", buttonSize)) { m_GizmoType = -1; }
		ImGui::SameLine();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		if (ImGui::Button("T", buttonSize)) { m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; }
		ImGui::SameLine();

		if (ImGui::Button("R", buttonSize)) { m_GizmoType = ImGuizmo::OPERATION::ROTATE; }
		ImGui::SameLine();

		if (ImGui::Button("S", buttonSize)) { m_GizmoType = ImGuizmo::OPERATION::SCALE; }
		ImGui::SameLine();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		if (ImGui::Button("G", buttonSize)) { m_IsDrawGrid = !m_IsDrawGrid; }
		ImGui::SameLine();

		if (ImGui::Button("A", buttonSize)) { m_IsDrawAxis = !m_IsDrawAxis; }
		ImGui::SameLine();

		ImGui::End();

		ImGui::PopStyleVar();
	}

	// --------------------------------------------------
	//*** TIENE LAG AL MOVER MUY RAPIDO LOS GUIZMOS (PROFILE & OPTIMIZE)
	void PanelViewport::_RenderGizmos(const Ref<Scene>& scene, Entity selectedEntity)
	{
		if (selectedEntity.isNull() || m_GizmoType == -1)
			return;

		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_PanelPosition.x, m_PanelPosition.y, m_PanelSize.x, m_PanelSize.y);

		// Editor camera
		const glm::mat4& cameraProjection = m_EditorCamera.GetProjectionMatrix();
		glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

		// Apply epsilon to cameraView if equal to IdentityMatrix (fix error with ImGuizmo::Manipulate returning NaNs)
		if (cameraView == glm::mat4(1.0f))
			cameraView[3][2] = -0.0001f;

		// Entity transform
		DVS_CORE_ASSERT(scene->HasComponent<C_Transform>(selectedEntity), "Entity doesn not have Transform Component");
		C_Transform& tc = scene->GetComponent<C_Transform>(selectedEntity);
		glm::mat4 transform = tc.GetTransform();

		// Snapping
		bool isSnap = Input::IsKeyPressed(Key::LEFT_CONTROL) || Input::IsKeyPressed(Key::RIGHT_CONTROL);
		float snapValue = (m_GizmoType == ImGuizmo::OPERATION::ROTATE) ? m_SnapDegrees : m_GridSize;

		float snapValues[3] = { snapValue, snapValue, snapValue };

		ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
			(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, isSnap ? snapValues : nullptr);

		if (!ImGuizmo::IsUsing())
			return;

		glm::vec3 translation, rotation, scale;
		Math::DecomposeTransform(transform, translation, rotation, scale);

		glm::vec3 deltaRotation = rotation - tc.rotation;
		tc.translation = translation;
		tc.rotation += deltaRotation;
		tc.scale = scale;
	}

	void PanelViewport::_RenderGrid()
	{
		const glm::vec4& viewportBounds = m_EditorCamera.GetViewportBounds();
		const float top = viewportBounds.x;
		const float left = viewportBounds.y;
		const float bottom = viewportBounds.z;
		const float right = viewportBounds.w;
		const float layer = -m_EditorCamera.GetFarClip(); //*** inverted (error in EditorCamera?)

		Renderer::BeginScene(m_EditorCamera);

		// --- Axis ---
		if (m_IsDrawAxis)
		{
			static const glm::vec4 xColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			static const glm::vec4 yColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

			// X Axis
			if (0.0f >= bottom && 0.0f <= top)
			{
				glm::vec3 start = glm::vec3(left, 0.0f, layer);
				glm::vec3 end = glm::vec3(right, 0.0f, layer);
				Renderer::DrawLine(start, end, xColor);
			}

			// Y Axis
			if (0.0f >= left && 0.0f <= right)
			{
				glm::vec3 start = glm::vec3(0.0f, bottom, layer);
				glm::vec3 end = glm::vec3(0.0f, top, layer);
				Renderer::DrawLine(start, end, yColor);
			}
		}

		// --- Grid ---
		if (m_IsDrawGrid && m_GridSize >= 1.0f)
		{
			float firstVerticalLine = std::floor(left / m_GridSize) * m_GridSize + 1;
			float firstHorizontalLine = std::floor(bottom / m_GridSize) * m_GridSize + 1;

			int numVerticalLines = static_cast<int>((right - firstVerticalLine) / m_GridSize) + 1;
			int numHorizontalLines = static_cast<int>((top - firstHorizontalLine) / m_GridSize) + 1;

			for (int i = 0; i < numVerticalLines; ++i)
			{
				float xPos = firstVerticalLine + i * m_GridSize;

				glm::vec3 start = glm::vec3(xPos, top, layer);
				glm::vec3 end = glm::vec3(xPos, bottom, layer);
				Renderer::DrawLine(start, end, m_GridColor);
			}

			for (int i = 0; i < numHorizontalLines; ++i)
			{
				float yPos = firstHorizontalLine + i * m_GridSize;

				glm::vec3 start = glm::vec3(left, yPos, layer);
				glm::vec3 end = glm::vec3(right, yPos, layer);
				Renderer::DrawLine(start, end, m_GridColor);
			}
		}

		Renderer::EndScene();
	}

}
