#pragma once
#include <Davos.h>

namespace Davos {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void OnInit() override;
		void OnCleanUp() override;

		void OnUpdate(TimeStep dt) override;

		void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		//Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_EditorScene;
		Ref<Scene> m_ActiveScene;

		OrthographicCameraController m_CameraController;
		UUID m_Camera;
		UUID m_Square;

		//*** Debug
		float m_SpriteRotation = 30.0f;
		float m_RectRotationSpeed = 10.0f;
		//std::string text = "Default Text";

		float m_LineWidth = 1.0f;
		glm::vec4 m_RectBackgroundColor = { 0.8f, 0.2f, 0.3f, 1.0f };
		glm::vec4 m_RectBorderColor = { 0.3f, 0.7f, 0.2f, 0.0f };

		Ref<Texture2D> m_Texture;
	};

}
