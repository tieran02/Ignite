#pragma once
#include "GraphicsContext.h"
#include "Ignite/CreateProperties.h"

namespace Ignite
{
	enum class TextureType
	{
		eUNDEFINED,
		eDIFFUSE,
		eSPECULAR,
		eNORMAL,
		eALPHAMASK,
		COUNT
	};

	struct Texture2DCreateInfo : CreateProperties
	{
		friend Texture2D;
		
		Texture2DCreateInfo(const std::string& m_name, const std::string& path, TextureType m_type)
			: m_name(m_name),
			  m_path(path),
			  m_width(0),
			  m_height(0),
			  m_type(m_type),
			  CreateProperties(CreatePropertyType::Texture2D)
		{
		}

		const std::string& GetName() const { return m_name; }
		const std::string& GetPath() const { return m_path; }
		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }
		uint32_t& Width() { return m_width; }
		uint32_t& Height() { return m_height; }
		TextureType GetType() const { return m_type; }
	private:
		const std::string m_name;
		const std::string m_path;
		uint32_t m_width, m_height;
		const TextureType m_type;
	};
	
	class Texture2D
	{
	protected:
		Texture2D(const Texture2DCreateInfo& info);
		virtual void Init() = 0;
		virtual void Cleanup() = 0;
	public:
		virtual ~Texture2D() = default;
		const GraphicsContext* m_context;

		virtual void Free() = 0;
		static std::unique_ptr<Texture2D> Create(const Texture2DCreateInfo& info);
	protected:
		Texture2DCreateInfo m_Texture2DInfo;
		bool m_deleted;
	};
}

