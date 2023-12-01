#include "UI/GameplayUI/GameplayUIController.h"
#include "Global/Config.h"
#include "Global/ServiceLocator.h"
#include "Gameplay/GameplayService.h"
#include "Sound/SoundService.h"
#include "Main/GameService.h"
#include "Gameplay/Collection/CollectionModel.h"

namespace UI
{
    namespace GameplayUI
    {
        using namespace Main;
        using namespace Sound;
        using namespace UIElement;
        using namespace Global;
        using namespace Gameplay;
        using namespace Collection;

        GameplayUIController::GameplayUIController()
        {
            createButton();
            createTexts();
        }

        GameplayUIController::~GameplayUIController()
        {
            destroy();
        }

        void GameplayUIController::initialize()
        {
            initializeButton();
            initializeTexts();
        }

        void GameplayUIController::createButton()
        {
            menu_button = new ButtonView();
        }

        void GameplayUIController::createTexts()
        {
            search_type_text = new TextView();
            comparisons_text = new TextView();
            array_access_text = new TextView();
        }

        void GameplayUIController::initializeButton()
        {
            menu_button->initialize("Menu Button",
                Config::menu_button_large_texture_path,
                button_width, button_height,
                sf::Vector2f(menu_button_x_position, menu_button_y_position));

            registerButtonCallback();
        }

        void GameplayUIController::initializeTexts()
        {
            search_type_text->initialize("Sort Type  :  Bubble Sort", sf::Vector2f(search_type_text_x_position, text_y_position), FontType::BUBBLE_BOBBLE, font_size);
            comparisons_text->initialize("Comparisons  :  0", sf::Vector2f(comparisons_text_x_position, text_y_position), FontType::BUBBLE_BOBBLE, font_size);
            array_access_text->initialize("Array Access  :  0", sf::Vector2f(array_access_text_x_position, text_y_position), FontType::BUBBLE_BOBBLE, font_size);
        }

        void GameplayUIController::update()
        {
            menu_button->update();
            updateSearchTypeText();
            updateComparisonsText();
            updateArrayAccessText();
        }

        void GameplayUIController::render()
        {
            menu_button->render();
            search_type_text->render();
            comparisons_text->render();
            array_access_text->render();
        }

        void GameplayUIController::show()
        {
            menu_button->show();
            search_type_text->show();
            comparisons_text->show();
            array_access_text->show();
        }

        void GameplayUIController::updateSearchTypeText()
        {
            SortType sort_type = ServiceLocator::getInstance()->getGameplayService()->getSortType();

            switch (sort_type)
            {
            case::Gameplay::SortType::BUBBLE_SORT:
                search_type_text->setText("Bubble Sort");
                break;

            case::Gameplay::SortType::INSERTION_SORT:
                search_type_text->setText("Insertion Sort");
                break;

            case::Gameplay::SortType::SELECTION_SORT:
                search_type_text->setText("Selection Sort");
                break;

            case::Gameplay::SortType::MERGE_SORT:
                search_type_text->setText("Merge Sort");
                break;

            case::Gameplay::SortType::QUICK_SORT:
                search_type_text->setText("Quick Sort");
                break;

            case::Gameplay::SortType::RADIX_SORT:
                search_type_text->setText("Radix Sort");
                break;
            }
            search_type_text->update();
        }

        void GameplayUIController::updateComparisonsText()
        {
            int comparisons = ServiceLocator::getInstance()->getGameplayService()->getNumberOfComparisons();;
            sf::String comparisons_string = "Comparisons  :  " + std::to_string(comparisons);

            comparisons_text->setText(comparisons_string);
            comparisons_text->update();
        }

        void GameplayUIController::updateArrayAccessText()
        {
            int array_access = ServiceLocator::getInstance()->getGameplayService()->getNumberOfArrayAccess();;
            sf::String array_access_string = "Array Access  :  " + std::to_string(array_access);

            array_access_text->setText(array_access_string);
            array_access_text->update();
        }

        void GameplayUIController::menuButtonCallback()
        {
            ServiceLocator::getInstance()->getSoundService()->playSound(SoundType::BUTTON_CLICK);
            ServiceLocator::getInstance()->getGameplayService()->reset();
            GameService::setGameState(GameState::MAIN_MENU);
        }

        void GameplayUIController::registerButtonCallback()
        {
            menu_button->registerCallbackFuntion(std::bind(&GameplayUIController::menuButtonCallback, this));
        }

        void GameplayUIController::destroy()
        {
            delete (menu_button);
            delete (search_type_text);
            delete (comparisons_text);
            delete (array_access_text);
        }
    }
}