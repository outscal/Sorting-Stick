#include "Gameplay/Collection/CollectionController.h"
#include "Gameplay/Collection/CollectionView.h"
#include "Gameplay/Collection/CollectionModel.h"
#include "Gameplay/GameplayService.h"
#include "Global/ServiceLocator.h"
#include "Gameplay/Collection/Element.h"
#include <random>

namespace Gameplay
{
	namespace Collection
	{
		using namespace UI::UIElement;
		using namespace Global;
		using namespace Graphics;

		CollectionController::CollectionController()
		{
			collection_view = new CollectionView();
			collection_model = new CollectionModel();

			for (int i = 0; i < collection_model->number_of_elements; i++) elements.push_back(new Element(i));
		}

		CollectionController::~CollectionController()
		{
			destroy();
		}

		void CollectionController::initialize()
		{
			collection_view->initialize(this);
			initializeElements();
			reset();
		}

		void CollectionController::initializeElements()
		{
			float rectangle_width = calculateElementWidth();

			for (int i = 0; i < collection_model->number_of_elements; i++)
			{
				sf::Vector2f rectangle_size = sf::Vector2f(rectangle_width,
					(static_cast<float>(i + 1) / collection_model->number_of_elements) * collection_model->max_element_height);

				elements[i]->element_view->initialize(rectangle_size, sf::Vector2f(0, 0), 0, collection_model->element_color);
			}
		}

		void CollectionController::update()
		{
			processSortThreadState();
			collection_view->update();
			for (int i = 0; i < elements.size(); i++) elements[i]->element_view->update();
		}

		void CollectionController::render()
		{
			collection_view->render();
			for (int i = 0; i < elements.size(); i++) elements[i]->element_view->render();
		}

		float CollectionController::calculateElementWidth()
		{
			float total_spacing = collection_model->elements_spacing * (collection_model->number_of_elements + 1);
			float total_width = ServiceLocator::getInstance()->getGraphicService()->getGameWindow()->getSize().x;

			return (total_width - total_spacing) / collection_model->number_of_elements;
		}

		void CollectionController::updateElementsPosition()
		{
			for (int i = 0; i < elements.size(); i++)
			{
				float x_position = (i * elements[i]->element_view->getSize().x) + ((i + 1) * collection_model->elements_spacing);
				float y_position = collection_model->element_y_position - elements[i]->element_view->getSize().y;

				elements[i]->element_view->setPosition(sf::Vector2f(x_position, y_position));
			}
		}

		void CollectionController::shuffleElements()
		{
			std::random_device device;
			std::mt19937 random_engine(device());

			std::shuffle(elements.begin(), elements.end(), random_engine);
			updateElementsPosition();
		}

		bool CollectionController::compareElementsByData(const Element* a, const Element* b) const
		{
			return a->data < b->data;
		}

		void CollectionController::processSortThreadState()
		{
			if (sort_thread.joinable() && isCollectionSorted()) sort_thread.join();
		}

		void CollectionController::processBubbleSort()
		{
			for (int j = 0; j < elements.size(); j++)
			{
				for (int i = 1; i < elements.size(); i++)
				{
					number_of_array_access += 2;
					number_of_comparisons++;

					elements[i - 1]->element_view->setFillColor(collection_model->processing_element_color);
					elements[i]->element_view->setFillColor(collection_model->processing_element_color);

					if (elements[i - 1]->data > elements[i]->data) std::swap(elements[i - 1], elements[i]);
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

					elements[i - 1]->element_view->setFillColor(collection_model->element_color);
					elements[i]->element_view->setFillColor(collection_model->element_color);
					updateElementsPosition();
				}
			}
		}

		void CollectionController::processInsertionSort()
		{
			for (int i = 1; i < elements.size(); ++i) 
			{
				int j = i - 1;
				number_of_array_access++;

				Element* key = elements[i];
				key->element_view->setFillColor(collection_model->processing_element_color);

				while (j >= 0 && elements[j]->data > key->data)
				{
					number_of_array_access += 4;
					number_of_comparisons++;

					elements[j + 1] = elements[j];
					elements[j] = key;

					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
					updateElementsPosition();
					--j;
				}

				key->element_view->setFillColor(collection_model->element_color);
				elements[j + 1] = key;

				updateElementsPosition();
				number_of_array_access++;
			}
		}

		void CollectionController::resetElementsColor()
		{
			for (int i = 0; i < elements.size(); i++) elements[i]->element_view->setFillColor(collection_model->element_color);
		}

		void CollectionController::resetVariables()
		{
			number_of_comparisons = 0;
			number_of_array_access = 0;
		}

		void CollectionController::reset()
		{
			current_operation_delay = 0;
			if (sort_thread.joinable()) sort_thread.join();

			shuffleElements();
			resetElementsColor();
			resetVariables();
		}

		void CollectionController::sortElements(SortType sort_type)
		{
			current_operation_delay = collection_model->operation_delay;
			this->sort_type = sort_type;

			switch (sort_type)
			{
			case Gameplay::Collection::SortType::BUBBLE_SORT:
				sort_thread = std::thread(&CollectionController::processBubbleSort, this);	
				break;

			case Gameplay::Collection::SortType::INSERTION_SORT:
				sort_thread = std::thread(&CollectionController::processInsertionSort, this);
				break;
			}
		}

		bool CollectionController::isCollectionSorted()
		{
			for (int i = 1; i < elements.size(); i++) if (elements[i] < elements[i - 1]) return false;
			return true;
		}

		void CollectionController::destroy()
		{
			current_operation_delay = 0;
			if (sort_thread.joinable()) sort_thread.join();

			for (int i = 0; i < elements.size(); i++) delete(elements[i]);
			elements.clear();

			delete (collection_view);
			delete (collection_model);
		}

		SortType CollectionController::getSortType() { return sort_type; }

		int CollectionController::getNumberOfComparisons() { return number_of_comparisons; }

		int CollectionController::getNumberOfArrayAccess() { return number_of_array_access; }
	}
}