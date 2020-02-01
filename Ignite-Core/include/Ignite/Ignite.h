#pragma once
#include "Ignite/Renderer/IPipeline.h"
#include "Ignite/Renderer/Renderer.h"
#include "Ignite/Renderer/RenderCommand.h"

namespace Ignite
{
	class Ignite
	{
	public:
		Ignite();
		~Ignite();

		void Init(uint32_t width, uint32_t height);
		bool Running();
		void Close();
	private:
		bool m_running;
	};
}
