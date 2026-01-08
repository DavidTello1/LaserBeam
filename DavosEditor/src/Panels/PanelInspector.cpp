#include "PanelInspector.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Davos {

	void PanelInspector::OnImGuiRender(const Ref<Scene>& scene, Entity selectedEntity)
	{
		ImGui::Begin("Inspector");

		if (!selectedEntity.isNull())
			_DrawComponents(scene, selectedEntity);

		ImGui::End();
	}

	// --------------------------------------------------
	template<typename T>
	static bool _DrawComponent(const std::string& name)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
		
		ImGui::Separator();
		bool isOpen = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Copy Component"))
			{
			}
			if (ImGui::MenuItem("Paste Component", nullptr, nullptr, false))
			{
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Reset Component"))
			{
			}

			ImGui::EndPopup();
		}

		return isOpen;
	}

	void PanelInspector::_DrawComponents(const Ref<Scene>& scene, Entity entity)
	{
		C_Tag* tag = scene->TryGetComponent<C_Tag>(entity);
		if (tag)
		{
			std::string& name = tag->name;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), name.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				name = std::string(buffer);
			}
		}

		C_Transform* transform = scene->TryGetComponent<C_Transform>(entity);
		if (transform && _DrawComponent<C_Transform>("Transform"))
		{
			ImGui::InputFloat("Layer", &transform->translation.z, 1.0f, 1.0f, "%.1f");

			float pos[2] = { transform->translation.x, transform->translation.y };
			ImGui::DragFloat2("Position", pos, 0.1f);
			transform->translation = { pos[0], pos[1], transform->translation.z };

			float rotation = glm::degrees(transform->rotation.z);
			ImGui::DragFloat("Rotation", &rotation);
			transform->rotation.z = glm::radians(rotation);

			float scale[2] = { transform->scale.x, transform->scale.y };
			ImGui::DragFloat2("Scale", scale, 0.1f);
			transform->scale = { scale[0], scale[1], transform->scale.z };

			//DrawVec3Control("Translation", component.Translation);
			//glm::vec3 rotation = glm::degrees(component.Rotation);
			//DrawVec3Control("Rotation", rotation);
			//component.Rotation = glm::radians(rotation);
			//DrawVec3Control("Scale", component.Scale, 1.0f);

			ImGui::TreePop();
		}

		C_Camera* camera = scene->TryGetComponent<C_Camera>(entity);
		if (camera && _DrawComponent<C_Camera>("Camera"))
		{
			ImGui::Checkbox("Fixed Aspect Ratio", &camera->isFixedAspectRatio);

			float camSize = camera->camera.GetSize();
			ImGui::DragFloat("Camera Size", &camSize);
			camera->camera.SetSize(camSize);

			float nearClip = camera->camera.GetNearClip();
			ImGui::DragFloat("Near Clip", &nearClip);
			camera->camera.SetNearClip(nearClip);

			float farClip = camera->camera.GetFarClip();
			ImGui::DragFloat("Far Clip", &farClip);
			camera->camera.SetFarClip(farClip);

			ImGui::TreePop();
		}

		C_SpriteRenderer* sprite = scene->TryGetComponent<C_SpriteRenderer>(entity);
		if (sprite && _DrawComponent<C_SpriteRenderer>("Sprite Renderer"))
		{
			ImGui::DragFloat("Tiling Factor", &sprite->tilingFactor, 1.0f, 0.0f, 0.0f, "%.2f");
			ImGui::ColorEdit4("Tint Color", glm::value_ptr(sprite->color));

			//***
			//ImGui::Text("Texture");
			//if (sprite->texture)
			//	ImGui::Image(sprite->texture.get()->GetRenderID(), {60.0f, 60.0f});

			ImGui::TreePop();
		}

		C_LevelMap* gridMap = scene->TryGetComponent<C_LevelMap>(entity);
		if (gridMap && _DrawComponent<C_LevelMap>("Sprite Renderer"))
		{
			int gridSize[2] = { gridMap->gridSize.x, gridMap->gridSize.y };
			ImGui::DragInt2("Grid Size", gridSize, 0.1f);
			{
				gridMap->gridSize = { gridSize[0], gridSize[1] };

				int numCells = gridSize[0] * gridSize[1];
				gridMap->cells.resize(numCells);
				gridMap->walkabilityMap.resize(numCells);
			}

			int cellSize[2] = { gridMap->cellSize.x, gridMap->cellSize.y };
			ImGui::DragInt2("Cell Size", cellSize);
			gridMap->cellSize = { cellSize[0], cellSize[1] };

			ImGui::ColorEdit4("Tint Color", glm::value_ptr(gridMap->tintColor));

			if (ImGui::TreeNode("Tileset"))
			{
				//Tileset& tileset = gridMap.tileset;
				//std::string texturePath = (!tileset->texture) ? "NULL" : tileset.texture.get()->GetPath();
				//ImTextureRef textureID = (!tileset->texture) ? nullptr : tileset.texture.get()->GetRenderID();
				std::string texturePath = "NULL";
				ImTextureRef textureID = nullptr;

				ImGui::Text("Texture: %s", texturePath.c_str());
				ImGui::SameLine();
				if (ImGui::Button("Edit"))
				{
					ImGui::MenuItem("Texture");
					ImGui::MenuItem("Texture2");
					ImGui::MenuItem("Texture3");
					ImGui::MenuItem("Texture4");
				}
				ImGui::Image(textureID, { 120.0f, 120.0f });

				//int tilesetSize[2] = { tileset.size.x, tileset.size.y };
				//ImGui::DragInt2("Tileset Size", tilesetSize, 0.1f);
				//tileset.size = { tilesetSize[0], tilesetSize[1] };

				//int tileSize[2] = { tileset.tileSize.x, tileset.tileSize.y };
				//ImGui::DragInt2("Tile Size", tileSize, 0.1f);
				//tileset.tileSize = { tileSize[0], tileSize[1] };

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Grid"))
			{
				ImGui::Checkbox("Draw Grid", &gridMap->isDrawGrid);
				ImGui::ColorEdit4("Grid Color", glm::value_ptr(gridMap->gridColor));

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

	}

}
