#include "Ignite/Ignite.h"
#include <memory>


int main()
{
    Ignite::Ignite ignite;
    ignite.Init();

    std::unique_ptr<Ignite::IPipeline> pipeline = Ignite::IPipeline::Create("resources/shaders/vert.spv", "resources/shaders/frag.spv");

	
    while (ignite.Running())
    {
        //start scene
        Ignite::Renderer::BeginScene();

        Ignite::Renderer::Submit(pipeline.get());

        Ignite::Renderer::EndScene();

        Ignite::Renderer::SwapBuffers();
    }
    return 0;
}
