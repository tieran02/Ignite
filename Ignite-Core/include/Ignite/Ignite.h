#pragma once
#include "Ignite/Application.h"

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
