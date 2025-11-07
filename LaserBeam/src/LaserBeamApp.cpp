#include <Davos.h>
#include <Davos/Core/EntryPoint.h>

#include "Sandbox.h"

class LaserBeam : public Davos::Application
{
public:
	LaserBeam(const Davos::AppSpecs& specs) : Davos::Application(specs)
	{
		PushLayer(new Sandbox());
	}
};

// -----------------------------------------------
Davos::Application* Davos::CreateApplication(Davos::AppCommandLineArgs args)
{
	AppSpecs specs;
	specs.name = "Sandbox";
	specs.workingDir = "../DavosEditor";
	specs.commandLineArgs = args;

	return new LaserBeam(specs);
}
