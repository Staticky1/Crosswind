#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"

#include "Core/CrosswindApp.h"
#include "Core.h"


Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{

	Walnut::ApplicationSpecification spec;
	spec.Name = "Crosswind";
	spec.CustomTitlebar = true;
	spec.IconPath = "data/images/icons/redstar.png";

	//create application
	Walnut::Application* app = new CrosswindApp(spec);
	CrosswindApp* crosswindApp = static_cast<CrosswindApp*>(app);

	Core::Instance().SetApp(crosswindApp);

	crosswindApp->StartApp();

	return app;
}

