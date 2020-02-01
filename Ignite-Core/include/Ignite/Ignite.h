#pragma once
#include "Ignite/IPipeline.h"
#include "Ignite/Renderer.h"
#include "Ignite/RenderCommand.h"

namespace Ignite
{
	class Ignite
	{
	public:
		Ignite();
		~Ignite();

		void Init();
		bool Running();
		void Close();
	private:
		bool m_running;
	};
}
