#include "PanelViewport.h"

#include <imgui/imgui.h>

namespace Davos {

	void PanelViewport::OnInit()
	{
		FB_Specs fbSpec;
		fbSpec.attachments = {
			{ FB_TextureFormat::RGBA8,		 FB_TextureFilterMin::LINEAR, FB_TextureFilterMag::LINEAR, FB_TextureWrap::CLAMP_TO_EDGE },
			{ FB_TextureFormat::RED_INTEGER, FB_TextureFilterMin::LINEAR, FB_TextureFilterMag::LINEAR, FB_TextureWrap::CLAMP_TO_EDGE },
			{ FB_TextureFormat::DEPTH,		 FB_TextureFilterMin::LINEAR, FB_TextureFilterMag::LINEAR, FB_TextureWrap::CLAMP_TO_EDGE }
		};
		fbSpec.width = 1280;
		fbSpec.height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);
	}

	void PanelViewport::OnRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void PanelViewport::OnResize()
	{
		if (FB_Specs spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != m_ViewportSize.x || spec.height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			//m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			//m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}
	}

	void PanelViewport::BindFramebuffer() const
	{
		m_Framebuffer->Bind();
	}

	void PanelViewport::UnbindFramebuffer() const
	{
		m_Framebuffer->Unbind();
	}

}
