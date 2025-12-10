#pragma once
#include <Davos.h>

class Sandbox : public Davos::Layer
{
public:
	Sandbox();
	virtual ~Sandbox() = default;

	void OnInit() override;
	void OnCleanUp() override;

	void OnUpdate(Davos::TimeStep dt) override;

	void OnImGuiRender() override;
	void OnEvent(Davos::Event& e) override;

private:
	//*** Debug
	float m_SpriteRotation = 30.0f;
	float m_RectRotationSpeed = 10.0f;
	//std::string text = "Default Text";

	float m_LineWidth = 1.0f;
	glm::vec4 m_RectBackgroundColor = { 0.8f, 0.2f, 0.3f, 1.0f };
	glm::vec4 m_RectBorderColor = { 0.3f, 0.7f, 0.2f, 0.0f };

	Davos::Ref<Davos::Texture2D> m_Texture;
};
