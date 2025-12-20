#pragma once
#include "Scene.h"

namespace Davos {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;

		template <typename T>
		T& GetComponent() {
			return m_Scene->GetComponent<T>(m_Handle);
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(TimeStep dt) {}

	private:
		Entity m_Handle = Entity::null;
		Scene* m_Scene = nullptr;

		friend class Scene;
	};

}
