#pragma once
#include "Gameplay/Collection/CollectionModel.h"
#include "Gameplay/Collection/CollectionController.h"

namespace Gameplay
{
	using namespace Collection;
	class GameplayController;

	class GameplayService
	{
	private:
		GameplayController* gameplay_controller;
		CollectionController* collection_controller;

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
	};
}
