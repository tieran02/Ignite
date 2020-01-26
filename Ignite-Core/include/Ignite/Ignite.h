#pragma once
#include "Ignite/IPipeline.h"

namespace Ignite
{
	class Ignite
	{
	public:
		Ignite();
		~Ignite();

		void Init();
		void Close();
	private:
		bool m_running;
	};
}
