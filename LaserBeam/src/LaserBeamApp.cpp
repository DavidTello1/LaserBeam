#include <Davos.h>
#include <Davos/Core/EntryPoint.h>

#include "Sandbox.h"

class LaserBeam : public Davos::Application
{
public:
	LaserBeam() 
	{
		PushLayer(new Sandbox());
	}

	~LaserBeam() 
	{
	}
};

// ----------------------------------
Davos::Application* Davos::CreateApplication()
{
	return new LaserBeam();
}
