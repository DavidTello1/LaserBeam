#pragma once
#include "KeyCodes.h"
#include "MouseCodes.h"

#include <glm/glm.hpp>

namespace Davos {

	class Input
	{
	public:
		static bool IsKeyPressed(const KeyCode key);

		static bool IsMouseButtonPressed(const MouseCode button);
		static bool IsMouseButtonReleased(const MouseCode button);

		// Window Coords (local)
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
		
		// Screen Coords (global)
		static glm::vec2 GetMouseScreenPos();
		static float GetMouseScreenX();
		static float GetMouseScreenY();
	};

}