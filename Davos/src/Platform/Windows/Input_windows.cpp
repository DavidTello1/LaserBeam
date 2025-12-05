#include "dvs_pch.h"
#include "Davos/Core/Input.h"

#include "Davos/Application.h"

#include <GLFW/glfw3.h>

namespace Davos {

	bool Input::IsKeyPressed(const KeyCode key)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		int state = glfwGetKey(window, static_cast<int32_t>(key));
		return (state == GLFW_PRESS);
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return (state == GLFW_PRESS);
	}

	bool Input::IsMouseButtonReleased(const MouseCode button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return (state == GLFW_RELEASE);
	}

	// --- Window Coords ---
	glm::vec2 Input::GetMousePosition()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
	// ---

	// --- Screen Coords ---
	glm::vec2 Input::GetMouseScreenPos()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		int winX, winY;
		glfwGetWindowPos(window, &winX, &winY);

		return { (float)(xpos + winX), (float)(ypos + winY) };
	}

	float Input::GetMouseScreenX()
	{
		return GetMouseScreenPos().x;
	}

	float Input::GetMouseScreenY()
	{
		return GetMouseScreenPos().y;
	}
	// ---

}
