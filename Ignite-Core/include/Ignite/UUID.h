#pragma once

#include <xhash>
#include <unordered_map>

namespace Ignite 
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		//UUID(UUID const& other);
		//UUID& operator=(UUID other);
		//UUID(UUID&& other);
		//UUID& operator=(UUID&& other);

		uint64_t Value() const;
		operator uint64_t() const;
	private:
		uint64_t m_uuid;
	};

	template <typename T>
	class IRegister
	{
	public:
		typedef std::unordered_map<UUID, T*> RegisterMapType;

		IRegister(RegisterMapType& registerMap) :
			m_register(&registerMap),
			m_final(true)
		{
		}

		IRegister(IRegister&& o)
		{
			m_register = std::move(o.m_register);
			m_uuid = std::move(o.m_uuid);
			o.m_final = false;
		};

		IRegister& operator=(IRegister&& o)
		{
			if (this != &o)
			{
				m_register = std::move(o.m_register);
				m_uuid = std::move(o.m_uuid);
				o.m_final = false;
			}
			return *this;
		}

		void Register()
		{
			if (!m_final) return;

			auto it = m_register->find(m_uuid);
			if (it == m_register->end())
			{
				printf("Registered object with UUID:%Iu \n", m_uuid.Value());
				m_register->insert(std::make_pair(m_uuid, reinterpret_cast<T*>(this)));
			}
		}

		void Deregister()
		{
			if (!m_final) return;

			auto it = m_register->find(m_uuid);
			if (it != m_register->end())
			{
				printf("Unregistered object with UUID:%Iu \n", it->first.Value());
				m_register->erase(it);
			}
		}

		const UUID& GetUUID() { return m_uuid; }
	private:
		RegisterMapType* m_register;
		UUID m_uuid;
		bool m_final;
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