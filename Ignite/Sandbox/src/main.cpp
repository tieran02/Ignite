#include "Ignite/Renderer.h"

int main()
{
    auto renderer = Ignite::Renderer::Create();
	auto& context = renderer->GetGraphicsContext();
    return 0;
}