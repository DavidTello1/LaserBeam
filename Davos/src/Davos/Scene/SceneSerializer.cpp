#include "dvs_pch.h"
#include "SceneSerializer.h"

#include "Davos/Scene/Components.h"
#include "Davos/Utils/SerializerUtils.h"

#include <fstream>

namespace Davos {

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, const Ref<Scene>& scene, Entity entity)
	{
		DVS_CORE_ASSERT(scene->HasComponent<C_UUID>(entity));

		out << YAML::BeginMap;

		// --- UUID
		out << YAML::Key << "Entity";
		out << YAML::Value << scene->GetComponent<C_UUID>(entity).id;

		// --- Tag
		auto* tagComponent = scene->TryGetComponent<C_Tag>(entity);
		if (tagComponent)
		{
			out << YAML::Key << "C_Tag";

			out << YAML::BeginMap;
			out << YAML::Key << "Tag" << YAML::Value << tagComponent->name;
			out << YAML::EndMap;
		}

		// --- Hierarchy
		auto* hierarchyComponent = scene->TryGetComponent<C_Hierarchy>(entity);
		if (hierarchyComponent)
		{
			out << YAML::Key << "C_Hierarchy";

			out << YAML::BeginMap;
			out << YAML::Key << "Parent" << YAML::Value << hierarchyComponent->parent;
			out << YAML::Key << "NumChilds" << YAML::Value << hierarchyComponent->numChilds;
			out << YAML::Key << "FirstChild" << YAML::Value << hierarchyComponent->firstChild;
			out << YAML::Key << "LastChild" << YAML::Value << hierarchyComponent->lastChild;
			out << YAML::Key << "NextSibling" << YAML::Value << hierarchyComponent->nextSibling;
			out << YAML::Key << "PrevSibling" << YAML::Value << hierarchyComponent->prevSibling;
			out << YAML::Key << "TotalChilds" << YAML::Value << hierarchyComponent->totalChilds;
			out << YAML::Key << "NumParents" << YAML::Value << hierarchyComponent->numParents;
			out << YAML::EndMap;
		}

		// --- Transform
		auto* transformComponent = scene->TryGetComponent<C_Transform>(entity);
		if (transformComponent)
		{
			out << YAML::Key << "C_Transform";

			out << YAML::BeginMap;
			out << YAML::Key << "Translation" << YAML::Value << transformComponent->translation;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent->rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent->scale;
			out << YAML::EndMap;
		}

		// --- Camera
		auto* cameraComponent = scene->TryGetComponent<C_Camera>(entity);
		if (cameraComponent)
		{
			out << YAML::Key << "C_Camera";

			out << YAML::BeginMap;
			out << YAML::Key << "IsFixedAspectRatio" << YAML::Value << cameraComponent->isFixedAspectRatio;

			auto& camera = cameraComponent->camera;
			out << YAML::Key << "Camera" << YAML::Value;

			out << YAML::BeginMap;
			out << YAML::Key << "Size" << YAML::Value << camera.GetSize();
			out << YAML::Key << "NearClip" << YAML::Value << camera.GetNearClip();
			out << YAML::Key << "FarClip" << YAML::Value << camera.GetFarClip();
			out << YAML::EndMap;

			out << YAML::EndMap;
		}

		// --- Sprite Renderer
		auto* spriteRendererComponent = scene->TryGetComponent<C_SpriteRenderer>(entity);
		if (spriteRendererComponent)
		{
			out << YAML::Key << "C_SpriteRenderer";

			out << YAML::BeginMap;
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent->color;
			out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent->tilingFactor;

			//if (spriteRendererComponent->texture)
			//	out << YAML::Key << "TexturePath" << YAML::Value << spriteRendererComponent->texture->GetPath();
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	// --------------------------------------------------
	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled_Scene";

		out << YAML::Key << "RootNode" << YAML::Value << m_Scene->GetComponent<C_UUID>(m_Scene->GetRootEntity()).id;
		out << YAML::Key << "MainCamera" << YAML::Value << m_Scene->GetComponent<C_UUID>(m_Scene->GetMainCamera()).id;

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		const auto view = m_Scene->GetAllEntitiesWith<C_UUID>();
		for (auto [entity, uid] : view)
			SerializeEntity(out, m_Scene, Entity(entity));

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			DVS_CORE_ERROR("Failed to load .davos file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		DVS_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		uint64_t rootNodeID = data["RootNode"].as<uint64_t>();
		uint64_t mainCameraID = data["MainCamera"].as<uint64_t>();

		auto entities = data["Entities"];
		if (!entities)
			return true;

		for (auto entity : entities)
		{
			Entity deserializedEntity = m_Scene->CreateEntity();

			// --- UUID
			uint64_t uuid = entity["Entity"].as<uint64_t>();
			m_Scene->AddComponent<C_UUID>(deserializedEntity, uuid);

			if (uuid == rootNodeID)
				m_Scene->SetRootEntity(deserializedEntity);

			DVS_CORE_TRACE("Deserialized entity with ID = {0}", uuid);

			// --- Tag
			auto tagComponent = entity["C_Tag"];
			if (tagComponent)
			{
				auto& tag = m_Scene->AddComponent<C_Tag>(deserializedEntity);

				tag.name = tagComponent["Tag"].as<std::string>();
				DVS_CORE_TRACE("Tag: {0}", tag.name.c_str());
			}

			// --- Hierarchy
			auto hierarchyComponent = entity["C_Hierarchy"];
			if (hierarchyComponent)
			{
				auto& hierarchy = m_Scene->AddComponent<C_Hierarchy>(deserializedEntity);

				hierarchy.parent = (Entity)hierarchyComponent["Parent"].as<uint32_t>();
				hierarchy.numChilds = hierarchyComponent["NumChilds"].as<uint32_t>();
				hierarchy.firstChild = (Entity)hierarchyComponent["FirstChild"].as<uint32_t>();
				hierarchy.lastChild = (Entity)hierarchyComponent["LastChild"].as<uint32_t>();
				hierarchy.nextSibling = (Entity)hierarchyComponent["NextSibling"].as<uint32_t>();
				hierarchy.prevSibling = (Entity)hierarchyComponent["PrevSibling"].as<uint32_t>();
				hierarchy.totalChilds = hierarchyComponent["TotalChilds"].as<uint32_t>();
				hierarchy.numParents = hierarchyComponent["NumParents"].as<uint32_t>();
			}

			// --- Transform
			auto transformComponent = entity["C_Transform"];
			if (transformComponent)
			{
				auto& transform = m_Scene->AddComponent<C_Transform>(deserializedEntity);

				transform.translation = transformComponent["Translation"].as<glm::vec3>();
				transform.rotation = transformComponent["Rotation"].as<glm::vec3>();
				transform.scale = transformComponent["Scale"].as<glm::vec3>();
			}

			// --- Camera
			auto cameraComponent = entity["C_Camera"];
			if (cameraComponent)
			{
				auto& camera = m_Scene->AddComponent<C_Camera>(deserializedEntity);

				camera.isFixedAspectRatio = cameraComponent["IsFixedAspectRatio"].as<bool>();

				auto& cameraProps = cameraComponent["Camera"];
				camera.camera.SetSize(cameraProps["Size"].as<float>());
				camera.camera.SetNearClip(cameraProps["NearClip"].as<float>());
				camera.camera.SetFarClip(cameraProps["FarClip"].as<float>());

				if (uuid == mainCameraID)
					m_Scene->SetMainCamera(deserializedEntity);
			}

			// --- Sprite Renderer
			auto spriteRendererComponent = entity["C_SpriteRenderer"];
			if (spriteRendererComponent)
			{
				auto& sprite = m_Scene->AddComponent<C_SpriteRenderer>(deserializedEntity);

				sprite.color = spriteRendererComponent["Color"].as<glm::vec4>();
				sprite.tilingFactor = spriteRendererComponent["TilingFactor"].as<float>();

				//std::string texturePath = spriteRendererComponent["TexturePath"].as<std::string>();
				//if (texturePath != "")
				//{
				//	auto path = Project::GetAssetFileSystemPath(texturePath);
				//	sprite.texture = Texture2D::Create(path.string());
				//}
			}
		}

		return true;
	}

}
