#pragma once
#include "Gameplay/Collection/StickCollectionModel.h"
#include "Gameplay/Collection/StickCollectionController.h"

namespace Gameplay
{
	using namespace Collection;
	class GameplayController;

	class GameplayService
	{
	private:
		GameplayController* gameplay_controller;
		StickCollectionController* collection_controller;

	public:
		GameplayService();
		~GameplayService();

		void initialize();
		void update();
		void render();

		void reset();
		void sortElement(Collection::SortType search_type);

		Collection::SortType getSortType();
		int getNumberOfComparisons();
		int getNumberOfArrayAccess();
		int getNumberOfSticks();
		int getDelayMilliseconds();
		sf::String getTimeComplexity();
	};
}

