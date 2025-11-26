#pragma once
#include <Davos/Scene/ScriptableEntity.h>

namespace Davos {

	class CameraController : public ScriptableEntity
	{
	public:
		void OnCreate() override
		{
		}

		void OnDestroy() override
		{
		}

		void OnUpdate(TimeStep dt) override
		{
			auto& transform = GetComponent<C_Transform>();
			float speed = 5.0f;

			if (Input::IsKeyPressed(Key::A))	  { transform.translation.x -= speed * dt; }
			else if (Input::IsKeyPressed(Key::D)) { transform.translation.x += speed * dt; }

			if (Input::IsKeyPressed(Key::W))	  { transform.translation.y += speed * dt; }
			else if (Input::IsKeyPressed(Key::S)) { transform.translation.y -= speed * dt; }
		}
	};

}
