#pragma once
#include "Ignite/Input.h"

namespace Ignite
{
	class WinInput : public Input
	{
	protected:
		bool IsKeyPressedImpl(int keyCode) override;
		bool IsMousePressedImpl(int button) override;
		float GetMouseXImpl() override;
		float GetMouseYImpl() override;
		std::pair<float, float> GetMousePositionImpl() override;
	};
}
