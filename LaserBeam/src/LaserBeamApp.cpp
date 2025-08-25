#include <Davos.h>

class LaserBeam : public Davos::Application
{
public:
	LaserBeam() {}
	~LaserBeam() {}
};

Davos::Application* Davos::CreateApplication()
{
	return new LaserBeam();
}
