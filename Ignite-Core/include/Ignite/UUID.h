#pragma once

#include <xhash>

namespace Ignite 
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		uint64_t Value() const;
		operator uint64_t() const;
	private:
		uint64_t m_uuid;
	};
}

namespace std
{
	template <>
	struct hash<Ignite::UUID>
	{
		size_t operator()(const Ignite::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}