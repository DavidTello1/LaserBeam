#pragma once

#ifdef DVS_PLATFORM_WINDOWS

extern Davos::Application* Davos::CreateApplication();

int main(int argc, char** argv)
{
	Davos::Log::Init();

	Davos::Application* app = Davos::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif
