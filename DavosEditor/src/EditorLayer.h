#pragma once
#include "Panels/PanelViewport.h"
#include "Panels/PanelHierarchy.h"
#include "Panels/PanelInspector.h"

namespace Davos {

	class KeyPressEvent;
	class MouseButtonPressEvent;

	enum class EntityType
	{
		EMPTY = 0,
		CAMERA,
		SPRITE
	};

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

		void OnProcessLast() override;

	private:
		void _OnMousePicking();

		// Input
		bool _OnKeyPressed(KeyPressEvent& e);
		bool _OnMouseButtonPressed(MouseButtonPressEvent& e);

		// Entities
		Entity _OnCreateEntity(EntityType type, Entity parent);
		void _OnDestroyEntity(Entity id);
		void _OnSelectEntity(Entity id);
		//void _OnReparentEntity(Entity id, Entity newParent);
		//void _OnReorderEntity(Entity id, uint8_t position);
		void _MarkToDelete(Entity id);
		std::vector<Entity> _GetAllChilds(Entity firstChild);

		// Scene Serialization
		void _NewScene();
		void _OpenScene();
		void _OpenScene(const std::filesystem::path& path);
		void _SaveScene();
		void _SaveSceneAs();
		void _SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		// ImGui
		void _DrawMainMenubar();
		void _DrawToolbar();
		void _Dockspace();

	private:
		Ref<Scene> m_ActiveScene;

		Entity m_SelectedEntity = Entity::null;
		Entity m_HoveredEntity = Entity::null;
		std::vector<Entity> m_DirtyEntities;

		// Panels
		PanelViewport m_PanelViewport;
		PanelHierarchy m_PanelHierarchy;
		PanelInspector m_PanelInspector;

		friend class PanelHierarchy; //*** just for testing
	};

}
