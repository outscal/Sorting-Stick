#pragma once
#include "UI/MainMenu/MainMenuUIController.h"
#include "UI/SplashScreen/SplashScreenUIController.h"
#include "UI/GameplayUI/GameplayUIController.h"
#include "UI/Interface/IUIController.h"

namespace UI
{
	class UIService
	{
	private:
		SplashScreen::SplashScreenUIController* splash_screen_controller;
		MainMenu::MainMenuUIController* main_menu_controller;
		GameplayUI::GameplayUIController* gameplay_controller;

		void createControllers();
		void initializeControllers();
		Interface::IUIController* getCurrentUIController();
		void destroy();

	public:
		UIService();
		~UIService();

		void initialize();
		void update();
		void render();
		void showScreen();
	};
}