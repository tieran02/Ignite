#pragma once
#include "IGraphicsContext.h"

namespace Ignite
{
	class ITexture2D
	{
	protected:
		ITexture2D(const std::string& name, const std::string& path);
		virtual void Init(const std::string& path) = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~ITexture2D() = default;
		const IGraphicsContext* m_context;

		virtual void Free() = 0;
		static std::shared_ptr<ITexture2D> Create(const std::string& name, const std::string& path);
	protected:
		std::string m_name;
		uint32_t m_width, m_height;
		bool m_deleted;
	};
}

