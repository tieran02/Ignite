#include "Ignite/Ignite.h"
#include "SponzaTestLayer.h"


int main()
{
	Ignite::Ignite ignite;
	ignite.App().PushLayer(new SponzaTestLayer);

	ignite.App().Start(1920, 1080);

	return 0;
}
