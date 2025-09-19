#include <Davos.h>

class ExampleLayer : public Davos::Layer
{
public:
	ExampleLayer() : Layer("Example") 
	{
		m_VertexArray.reset(Davos::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.2f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.8f, 0.2f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.0f
		};
		std::shared_ptr<Davos::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Davos::VertexBuffer::Create(vertices, sizeof(vertices)));
		vertexBuffer->SetLayout({
			{ Davos::ShaderDataType::FLOAT_3, "a_Position" },
			{ Davos::ShaderDataType::FLOAT_4, "a_Color" }
			});

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Davos::IndexBuffer> indexBuffer;
		indexBuffer.reset(Davos::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSource = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSource = R"(
			#version 330 core
			
			layout(location = 0) out vec4 o_Color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				o_Color = vec4(v_Position * 0.5 + 0.5, 1.0);
				o_Color = v_Color;
			}
		)";

		m_Shader.reset(new Davos::Shader(vertexSource, fragmentSource));
	}

	void OnUpdate() override 
	{
		//DVS_INFO("ExampleLayer::Update");
		//if (Davos::Input::IsKeyPressed(DVS_KEY_A))
		//	DVS_TRACE("A Key is pressed!");

		Davos::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Davos::RenderCommand::Clear();

		Davos::Renderer::BeginScene();

		m_Shader->Bind();
		Davos::Renderer::Submit(m_VertexArray);

		Davos::Renderer::EndScene();
	}

	void OnEvent(Davos::Event& event) override 
	{
		//DVS_TRACE("{0}", event);
	}

private:
	std::shared_ptr<Davos::Shader> m_Shader;
	std::shared_ptr<Davos::VertexArray> m_VertexArray;

};

// ---
class LaserBeam : public Davos::Application
{
public:
	LaserBeam() 
	{
		PushLayer(new ExampleLayer());
	}

	~LaserBeam() 
	{
	}
};

// ----------------------------------
Davos::Application* Davos::CreateApplication()
{
	return new LaserBeam();
}
