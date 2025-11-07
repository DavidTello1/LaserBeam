#include <Davos.h>
#include <Davos/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Davos {

	class DavosEditor : public Application
	{
	public:
		DavosEditor(const AppSpecs& specs) : Application(specs)
		{
			PushLayer(new EditorLayer());
		}
	};

	// -----------------------------------------------
	Application* CreateApplication(AppCommandLineArgs args)
	{
		AppSpecs specs;
		specs.name = "Davos Editor";
		specs.commandLineArgs = args;

		return new DavosEditor(specs);
	}

}
