#pragma once

class ImGUI
{
public:
	static std::unique_ptr<ImGUI> Create();
	virtual void Init(float width, float height) = 0;
	
	virtual ~ImGUI() = default;
protected:
	ImGUI() = default;
};

