#pragma once
#include "dvs_pch.h"
#include "Davos/Core.h"

namespace Davos {

	// Events are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then and there.
	//For the future, a better strategy might be to buffer events in an event bus
	// and process them during the "event" part of the update stage.

	enum class EventType
	{
		None = 0,

		// Window Events
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,

		// App Events
		AppTick,
		AppUpdate,
		AppRender,

		// Key Events
		KeyPressed,
		KeyReleased,
		KeyTyped,

		// Mouse Events
		MouseMoved,
		MouseScrolled,
		MouseButtonPressed,
		MouseButtonReleased,
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

#define EVENT_CLASS_TYPE(type)\
	static EventType GetStaticType() { return EventType::##type; }\
	virtual EventType GetEventType() const override { return GetStaticType(); }\
	virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)\
	virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual const char* GetName() const = 0; // Debug Purposes (should not be used in Dist versions)
		virtual std::string ToString() const { return GetName(); } // Debug Purposes (should not be used in Dist versions)

		inline bool IsInCategory(EventCategory category) {
			return GetCategoryFlags() & category;
		}

	public:
		bool isHandled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event) : m_Event(event) {};

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.isHandled = func(*(T*)&m_Event);
				return true;
			}

			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::string format_as(const Event& e) 
	{
		return e.ToString();
	}
}