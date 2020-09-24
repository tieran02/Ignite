#include "Ignite/Ignite.h"
#include "SponzaTestLayer.h"
#include "TestUILayer.h"


int main()
{
	Ignite::Ignite ignite;
	ignite.App().PushLayer(new TestUILayer);
	ignite.App().PushLayer(new SponzaTestLayer);

	ignite.App().Start(1920, 1080);

	return 0;
}
