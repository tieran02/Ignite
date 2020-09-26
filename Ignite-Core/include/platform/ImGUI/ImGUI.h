#pragma once

class ImGUI
{
public:
	virtual ~ImGUI() = default;
	static std::unique_ptr<ImGUI> Create();
	
	virtual void Init(float width, float height) = 0;
	virtual void NewFrame(bool updateFrameGraph) = 0;

	virtual void UpdateBuffers() = 0;
	virtual void DrawFrame() = 0;
protected:
	ImGUI() = default;
};

