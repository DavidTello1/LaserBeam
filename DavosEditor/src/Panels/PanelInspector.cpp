#include "PanelInspector.h"

#include <ImGui/imgui.h>

namespace Davos {

	void PanelInspector::OnImGuiRender(const Ref<Scene>& scene, Entity selectedEntity)
	{
		ImGui::Begin("Inspector");

		if (!selectedEntity.isNull())
			_DrawComponents(scene, selectedEntity);

		ImGui::End();
	}

	// --------------------------------------------------
	template <typename T>
	static void _DrawComponent(const char* name, T& component, const C_UUID id)
	{
		const ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 5));
		bool isOpen = ImGui::TreeNodeEx(name, treeNodeFlags, name);
		ImGui::PopStyleVar();

		if (isOpen)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

			std::string str_id = std::to_string(id.id);
			_DrawComponentUI(str_id.c_str(), component);
			
			ImGui::PopStyleVar();
		}
	}

	template <typename T>
	static void _DrawComponentUI(const char* id, T& component)
	{
		DVS_ASSERT("Invalid Component Type");
	}

	template <>
	static void _DrawComponentUI<C_Transform>(const char* id, C_Transform& transform)
	{
		InspectorUtils::FieldInt(id, "Layer", (int*)&transform.translation.z, 0);

		InspectorUtils::FieldFloat2(id, "Position", &transform.translation.x, &transform.translation.y, { 0.0f, 0.0f }, 0.1f);

		float rotation = glm::degrees(transform.rotation.z);
		if (InspectorUtils::FieldFloat(id, "Rotation", &rotation, 0.0f, 1.0f, -360.0f, 360.0f, "%.1f"))
			transform.rotation.z = glm::radians(rotation);

		static bool isKeepRatio = true;
		InspectorUtils::FieldRatioFloat2(id, "Scale", &transform.scale.x, &transform.scale.y, { 1.0f, 1.0f }, &isKeepRatio, 0.1f);
	}

	template <>
	static void _DrawComponentUI<C_Camera>(const char* id, C_Camera& camera)
	{
		InspectorUtils::FieldBool(id, "Fixed Aspect Ratio", &camera.isFixedAspectRatio, false);

		float camSize = camera.camera.GetSize();
		if (InspectorUtils::FieldFloat(id, "Camera Size", &camSize, 10.0f, 1.0f, 0.0f, 0.0f, "%.1f"))
			camera.camera.SetSize(camSize);

		float camNearClip = camera.camera.GetNearClip();
		if (InspectorUtils::FieldFloat(id, "Near Clip", &camNearClip, -1.0f, 1.0f, 0.0f, 0.0f, "%.1f"))
			camera.camera.SetNearClip(camNearClip);

		float camFarClip = camera.camera.GetFarClip();
		if (InspectorUtils::FieldFloat(id, "Far Clip", &camFarClip, 1.0f, 1.0f, 0.0f, 0.0f, "%.1f"))
			camera.camera.SetFarClip(camFarClip);
	}

	template <>
	static void _DrawComponentUI<C_SpriteRenderer>(const char* id, C_SpriteRenderer& sprite)
	{
		InspectorUtils::FieldFloat(id, "Tiling Factor", &sprite.tilingFactor, 1.0f, 1.0f, 0.0f, 0.0f, "%.2f");
		InspectorUtils::FieldColor4(id, "Tint Color", &sprite.color, glm::vec4(1.0f));
		InspectorUtils::FieldTexture(id, "Texture", &sprite.texture, nullptr);
	}

	// --------------------------------------------------
	void PanelInspector::_DrawComponents(const Ref<Scene>& scene, Entity entity)
	{
		C_UUID* id = scene->TryGetComponent<C_UUID>(entity);
		if (!id)
		{
			DVS_ERROR("Entity without UUID Component");
			return;
		}
		ImGui::Text("%llu", id->id);

		C_Tag* tag = scene->TryGetComponent<C_Tag>(entity);
		if (tag)
		{
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag->name.c_str(), sizeof(buffer));

			ImGui::SetNextItemWidth(-1);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag->name = std::string(buffer);
		}
		ImGui::Separator();

		C_Transform* transform = scene->TryGetComponent<C_Transform>(entity);
		if (transform)
			_DrawComponent<C_Transform>("Transform", *transform, *id);

		C_Camera* camera = scene->TryGetComponent<C_Camera>(entity);
		if (camera)
			_DrawComponent<C_Camera>("Camera", *camera, *id);

		C_SpriteRenderer* sprite = scene->TryGetComponent<C_SpriteRenderer>(entity);
		if (sprite)
			_DrawComponent<C_SpriteRenderer>("Sprite Renderer", *sprite, *id);
	}

//		//C_LevelMap* gridMap = scene->TryGetComponent<C_LevelMap>(entity);
//		//if (gridMap && _DrawComponent<C_LevelMap>("Sprite Renderer"))
//		//{
//		//	int gridSize[2] = { gridMap->gridSize.x, gridMap->gridSize.y };
//		//	ImGui::DragInt2("Grid Size", gridSize, 0.1f);
//		//	{
//		//		gridMap->gridSize = { gridSize[0], gridSize[1] };
//
//		//		int numCells = gridSize[0] * gridSize[1];
//		//		gridMap->cells.resize(numCells);
//		//		gridMap->walkabilityMap.resize(numCells);
//		//	}
//
//		//	int cellSize[2] = { gridMap->cellSize.x, gridMap->cellSize.y };
//		//	ImGui::DragInt2("Cell Size", cellSize);
//		//	gridMap->cellSize = { cellSize[0], cellSize[1] };
//
//		//	ImGui::ColorEdit4("Tint Color", glm::value_ptr(gridMap->tintColor));
//
//		//	if (ImGui::TreeNode("Tileset"))
//		//	{
//		//		//Tileset& tileset = gridMap.tileset;
//		//		//std::string texturePath = (!tileset->texture) ? "NULL" : tileset.texture.get()->GetPath();
//		//		//ImTextureRef textureID = (!tileset->texture) ? nullptr : tileset.texture.get()->GetRenderID();
//		//		std::string texturePath = "NULL";
//		//		ImTextureRef textureID = nullptr;
//
//		//		ImGui::Text("Texture: %s", texturePath.c_str());
//		//		ImGui::SameLine();
//		//		if (ImGui::Button("Edit"))
//		//		{
//		//			ImGui::MenuItem("Texture");
//		//			ImGui::MenuItem("Texture2");
//		//			ImGui::MenuItem("Texture3");
//		//			ImGui::MenuItem("Texture4");
//		//		}
//		//		ImGui::Image(textureID, { 120.0f, 120.0f });
//
//		//		//int tilesetSize[2] = { tileset.size.x, tileset.size.y };
//		//		//ImGui::DragInt2("Tileset Size", tilesetSize, 0.1f);
//		//		//tileset.size = { tilesetSize[0], tilesetSize[1] };
//
//		//		//int tileSize[2] = { tileset.tileSize.x, tileset.tileSize.y };
//		//		//ImGui::DragInt2("Tile Size", tileSize, 0.1f);
//		//		//tileset.tileSize = { tileSize[0], tileSize[1] };
//
//		//		ImGui::TreePop();
//		//	}
//
//		//	if (ImGui::TreeNode("Grid"))
//		//	{
//		//		ImGui::Checkbox("Draw Grid", &gridMap->isDrawGrid);
//		//		ImGui::ColorEdit4("Grid Color", glm::value_ptr(gridMap->gridColor));
//
//		//		ImGui::TreePop();
//		//	}
//
//		//	ImGui::TreePop();
//		//}
}
