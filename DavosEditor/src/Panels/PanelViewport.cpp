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

		ImVec2 viewportPos = ImGui::GetCursorScreenPos();
		m_PanelPosition = { viewportPos.x, viewportPos.y };

		m_IsFocused = ImGui::IsWindowFocused();
		m_IsHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->SetBlockEvents(!m_IsHovered);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_PanelSize = { viewportSize.x, viewportSize.y };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(m_PanelSize.x, m_PanelSize.y), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();
	}

	bool PanelViewport::OnResize()
	{
		if (FB_Specs spec = m_Framebuffer->GetSpecification();
			m_PanelSize.x > 0.0f && m_PanelSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != m_PanelSize.x || spec.height != m_PanelSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_PanelSize.x, (uint32_t)m_PanelSize.y);
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

}
