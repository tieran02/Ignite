#pragma once
#include "Ignite/Core.h"

namespace Ignite
{
	class Ignite
	{
	public:
		Ignite();
		~Ignite();

		Ignite::Application& App() { return *m_application; }
	private:
		Ignite::Application* m_application;
	};
}
