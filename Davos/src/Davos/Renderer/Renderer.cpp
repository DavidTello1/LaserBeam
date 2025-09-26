#include "dvs_pch.h"
#include "Renderer.h"

namespace Davos {

	//Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init()
	{
		RenderCommand::Init();
		//Renderer2D::Init();
	}

	void Renderer::CleanUp()
	{
		//Renderer2D::CleanUp();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	//void Renderer::BeginScene(Camera& camera)
	void Renderer::BeginScene()
	{
		//s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		//shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		//shader->SetMat4("u_Model", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}