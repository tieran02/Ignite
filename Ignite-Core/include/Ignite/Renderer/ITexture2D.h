#pragma once
#include "IGraphicsContext.h"

namespace Ignite
{
	enum class TextureType
	{
		eUNDEFINED,
		eDIFFUSE,
		eSPECULAR,
		eNORMAL,
		eALPHAMASK
	};
	
	class ITexture2D
	{
	protected:
		ITexture2D(const std::string& name, const std::string& path, TextureType textureType);
		virtual void Init(const std::string& path) = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~ITexture2D() = default;
		const IGraphicsContext* m_context;

		virtual void Free() = 0;
		static std::shared_ptr<ITexture2D> Create(const std::string& name, const std::string& path, TextureType textureType);
	protected:
		std::string m_name;
		uint32_t m_width, m_height;
		TextureType m_type;
		bool m_deleted;
	};
}

