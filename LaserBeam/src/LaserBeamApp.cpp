#include <Davos.h>

#include <memory>

class ExampleLayer : public Davos::Layer
{
public:
	ExampleLayer() : Layer("Example") 
	{
		m_VertexArray = Davos::VertexArray::Create();

		float vertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		Davos::Ref<Davos::VertexBuffer> vertexBuffer;
		vertexBuffer = Davos::VertexBuffer::Create(vertices, sizeof(vertices));
		vertexBuffer->SetLayout({
			{ Davos::ShaderDataType::FLOAT_3, "a_Position" },
			{ Davos::ShaderDataType::FLOAT_2, "a_TexCoords" }
			});

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[6] = { 0, 1, 2, 0, 3, 2};
		Davos::Ref<Davos::IndexBuffer> indexBuffer;
		indexBuffer = Davos::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_Texture = Davos::Texture2D::Create("Assets/Textures/checkerboard.png");
		m_Shader = Davos::Shader::Create("Assets/Shaders/Texture.glsl");
		m_Shader->Bind();
		m_Shader->SetInt("u_Texture", 0);
	}

	void OnUpdate(Davos::TimeStep dt) override 
	{
		//DVS_TRACE("Delta time: {0} s ({1} ms)", dt.GetSeconds(), dt.GetMilliseconds());

		//DVS_INFO("ExampleLayer::Update");
		//if (Davos::Input::IsKeyPressed(DVS_KEY_A))
		//	DVS_TRACE("A Key is pressed!");

		Davos::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Davos::RenderCommand::Clear();

		Davos::Renderer::BeginScene();

		m_Shader->Bind();
		m_Texture->Bind();
		Davos::Renderer::Submit(m_Shader, m_VertexArray);

		Davos::Renderer::EndScene();
	}

	void OnEvent(Davos::Event& event) override 
	{
	}

private:
	Davos::Ref<Davos::Shader> m_Shader;
	Davos::Ref<Davos::VertexArray> m_VertexArray;

	Davos::Ref<Davos::Texture> m_Texture;

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
