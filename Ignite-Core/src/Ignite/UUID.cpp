#include "igpch.h"
#include "Ignite/UUID.h"

#include <random>

namespace Ignite
{
	static std::random_device s_randomDevice;
	static std::mt19937_64 s_engine(s_randomDevice());
	static std::uniform_int_distribution<uint64_t> s_uniformDistribution;

	UUID::UUID() : 
		m_uuid(s_uniformDistribution(s_engine))
	{

	}

	UUID::UUID(uint64_t uuid) : 
		m_uuid(uuid)
	{
	}

	uint64_t UUID::Value() const
	{
		return m_uuid;
	}

	UUID::operator uint64_t() const
	{
		return m_uuid;
	}
}