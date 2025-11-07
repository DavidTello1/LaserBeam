#pragma once

#ifdef DVS_PLATFORM_WINDOWS

extern Davos::Application* Davos::CreateApplication(AppCommandLineArgs args);

int main(int argc, char** argv)
{
	Davos::Log::Init();

	Davos::Application* app = Davos::CreateApplication({ argc, argv });

	app->Run();
	
	delete app;
}

#endif
