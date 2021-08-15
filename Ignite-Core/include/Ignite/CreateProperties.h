#pragma once
#include <memory>

enum class CreatePropertyType
{
	Window,
	Buffer,
	Pipeline,
	Material,
	Texture2D,
	Model,
	Mesh,
	NumberOfTypes
};

struct CreateProperties
{
public:
	CreateProperties(CreatePropertyType type) : m_type(type) {}
	CreatePropertyType GetType() const { return m_type; }
private:
	CreatePropertyType m_type;
};
