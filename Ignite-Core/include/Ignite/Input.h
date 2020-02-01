#pragma once
#include "Ignite/KeyCodes.h"
#include <utility>

namespace Ignite
{
	class Input
	{
	public:
		inline static bool IsKeyPressed(int keyCode) { return s_instance->IsKeyPressedImpl(keyCode); }

		inline static bool IsMousePressed(int keyCode) { return s_instance->IsMousePressedImpl(keyCode); }
		inline static float GetMouseX() { return s_instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_instance->GetMouseYImpl(); }
		inline static std::pair<float, float> GetMousePosition() { return s_instance->GetMousePositionImpl(); }

	protected:
		//implementation is platform depended and needs to be implemented per platform
		virtual bool IsKeyPressedImpl(int keyCode) = 0;
		virtual bool IsMousePressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
	private:
		static Input* s_instance;
	};
}