#include "Ignite/Ignite.h"
#include <memory>

int main()
{
    Ignite::Ignite ignite;
    ignite.Init();

    std::unique_ptr<Ignite::IPipeline> pipeline = Ignite::IPipeline::Create("", "");
    return 0;
}
