#include "Gameplay/Collection/StickCollectionController.h"
#include "Gameplay/Collection/StickCollectionView.h"
#include "Gameplay/Collection/StickCollectionModel.h"
#include "Gameplay/GameplayService.h"
#include "Global/ServiceLocator.h"
#include "Gameplay/Collection/Stick.h"
#include <random>
#include"../../include/Sound/SoundService.h"

namespace Gameplay
{
	namespace Collection
	{
		using namespace UI::UIElement;
		using namespace Global;
		using namespace Graphics;
		using namespace Sound;

		StickCollectionController::StickCollectionController()
		{
			collection_view = new StickCollectionView();
			collection_model = new StickCollectionModel();

			for (int i = 0; i < collection_model->number_of_elements; i++) sticks.push_back(new Stick(i));
		}

		StickCollectionController::~StickCollectionController()
		{
			destroy();
		}

		void StickCollectionController::initialize()
		{
			sort_State = SortingState::NotSorting;
			collection_view->initialize(this);
		
			initializeSticks();
			reset();
		}

		void StickCollectionController::initializeSticks()
		{
			float rectangle_width = calculateStickWidth();


			for (int i = 0; i < collection_model->number_of_elements; i++)
			{
				float rectangle_height = calculateStickHeight(i); //calc height

				sf::Vector2f rectangle_size = sf::Vector2f(rectangle_width, rectangle_height);

				sticks[i]->stick_view->initialize(rectangle_size, sf::Vector2f(0, 0), 0, collection_model->element_color);
			}
		}

		void StickCollectionController::update()
		{
			processSortThreadState();
			collection_view->update();
			for (int i = 0; i < sticks.size(); i++) sticks[i]->stick_view->update();
		}

		void StickCollectionController::render()
		{
			collection_view->render();
			for (int i = 0; i < sticks.size(); i++) sticks[i]->stick_view->render();
		}

		float StickCollectionController::calculateStickWidth()
		{
			float total_space = static_cast<float>(ServiceLocator::getInstance()->getGraphicService()->getGameWindow()->getSize().x);

			// Calculate total spacing as 10% of the total space
			float total_spacing = collection_model->space_percentage * total_space;

			// Calculate the space between each stick
			float space_between = total_spacing / (collection_model->number_of_elements - 1);
			collection_model->setElementSpacing(space_between);

			// Calculate the remaining space for the rectangles
			float remaining_space = total_space - total_spacing;

			// Calculate the width of each rectangle
			float rectangle_width = remaining_space / collection_model->number_of_elements;

			return rectangle_width;
		}

		float StickCollectionController::calculateStickHeight(int array_pos)
		{
			return (static_cast<float>(array_pos + 1) / collection_model->number_of_elements) * collection_model->max_element_height;
		}

		void StickCollectionController::updateStickPosition()
		{
			for (int i = 0; i < sticks.size(); i++)
			{
				float x_position = (i * sticks[i]->stick_view->getSize().x) + ((i + 1) * collection_model->elements_spacing);
				float y_position = collection_model->element_y_position - sticks[i]->stick_view->getSize().y;

				sticks[i]->stick_view->setPosition(sf::Vector2f(x_position, y_position));
			}
		}

		void StickCollectionController::shuffleSticks()
		{
			std::random_device device;
			std::mt19937 random_engine(device());

			std::shuffle(sticks.begin(), sticks.end(), random_engine);
			updateStickPosition();
		}

		bool StickCollectionController::compareSticksByData(const Stick* a, const Stick* b) const
		{
			return a->data < b->data;
		}

		void StickCollectionController::processSortThreadState()
		{
			if (sort_thread.joinable() && isCollectionSorted()) {
				sort_thread.join();
				sort_State = SortingState::NotSorting;
			}
		}


		void StickCollectionController::resetSticksColor()
		{
			for (int i = 0; i < sticks.size(); i++) sticks[i]->stick_view->setFillColor(collection_model->element_color);
		}

		void StickCollectionController::resetVariables()
		{
			number_of_comparisons = 0;
			number_of_array_access = 0;
		}

		void StickCollectionController::reset()
		{
			current_operation_delay = 0;
			sort_State = SortingState::NotSorting;
			color_delay = 0;
			if (sort_thread.joinable()) sort_thread.join();

			shuffleSticks();
			resetSticksColor();
			resetVariables();
		}

		void StickCollectionController::sortElements(SortType sort_type)
		{
			current_operation_delay = collection_model->operation_delay;
			this->sort_type = sort_type;
			color_delay = collection_model->colorDelay;
			sort_State = SortingState::Sorting;
			switch (sort_type)
			{
			case Gameplay::Collection::SortType::BUBBLE_SORT:
				time_complexity = "O(n^2)";
				sort_thread = std::thread(&StickCollectionController::processBubbleSort, this);
				break;

			case Gameplay::Collection::SortType::INSERTION_SORT:

				sort_thread = std::thread(&StickCollectionController::processInsertionSort, this);
				break;

			case Gameplay::Collection::SortType::SELECTION_SORT:
				sort_thread = std::thread(&StickCollectionController::processSelectionSort, this);
					break;

			case Gameplay::Collection::SortType::MERGE_SORT:
				sort_thread = std::thread(&StickCollectionController::processMergeSort, this);
				break;

			case Gameplay::Collection::SortType::QUICK_SORT:
				sort_thread = std::thread(&StickCollectionController::processQuickSort, this); // Placeholder for quick sort implementation
			}
			

		
		}

		bool StickCollectionController::isCollectionSorted()
		{
			for (int i = 1; i < sticks.size(); i++) if (sticks[i] < sticks[i - 1]) return false;
			return true;
		}

		void StickCollectionController::destroy()
		{
			current_operation_delay = 0;
			if (sort_thread.joinable()) sort_thread.join();

			for (int i = 0; i < sticks.size(); i++) delete(sticks[i]);
			sticks.clear();

			delete (collection_view);
			delete (collection_model);
		}

		SortType StickCollectionController::getSortType() { return sort_type; }

		int StickCollectionController::getNumberOfComparisons() { return number_of_comparisons; }

		int StickCollectionController::getNumberOfArrayAccess() { return number_of_array_access; }

		int StickCollectionController::getNumberOfSticks() { return collection_model->number_of_elements; }

		int StickCollectionController::getDelayMilliseconds() { return current_operation_delay; }

		sf::String StickCollectionController::getTimeComplexity() { return time_complexity; }
		void StickCollectionController::processBubbleSort()
		{

	     SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();

			for (int j = 0; j < sticks.size(); j++) {   // Loop through the sticks array
				if (sort_State == SortingState::NotSorting) { break; }   // Check if sorting has stopped or been interrupted

				bool swapped = false;  // To track if a swap was made

				for (int i = 1; i < sticks.size() - j; i++) {    // Loop through the array, reducing the range each pass
					if (sort_State == SortingState::NotSorting) { break; }      // Check if sorting has stopped or been interrupted

					// Increment the number of array accesses and comparisons
					number_of_array_access += 2;
					number_of_comparisons++;

					sound->playSound(SoundType::COMPARE_SFX);      // Play the compare sound effect

					// Set the current sticks to the processing color
					sticks[i - 1]->stick_view->setFillColor(collection_model->processing_element_color);
					sticks[i]->stick_view->setFillColor(collection_model->processing_element_color);

					if (sticks[i - 1]->data > sticks[i]->data) {      // Check if the current stick is greater than the next stick
						// Swap the sticks if necessary
						std::swap(sticks[i - 1], sticks[i]);
						swapped = true;  // Set swapped to true if there was a swap
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

					// Reset the stick colors
					sticks[i - 1]->stick_view->setFillColor(collection_model->element_color);
					sticks[i]->stick_view->setFillColor(collection_model->element_color);

					updateStickPosition();
				}

				if (sticks.size() - j - 1 >= 0) {    // Set the last sorted stick to the placement position color
					sticks[sticks.size() - j - 1]->stick_view->setFillColor(collection_model->placement_position_element_color);
				}

				if (!swapped) { break; }    // If no swaps were made, the array is already sorted
			}

			completedColour();
		}
		void StickCollectionController::processInsertionSort()
		{
			/*ServiceLocator::getInstance()->getSoundService()->playSound(Sound::SoundType::COMPARE_SFX);*/

			ServiceLocator::getInstance()->getSoundService()->playSound(Sound::SoundType::COMPARE_SFX);

			for (int i = 1; i < sticks.size(); i++) {
				if (sort_State == SortingState::NotSorting) { break; } // Check if sorting has stopped or been interrupted

				int j = i - 1;
				Stick* key = sticks[i];
				number_of_array_access++;
				sticks[i]->stick_view->setFillColor(collection_model->processing_element_color); // Set the key stick to processing color
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

				while (j >= 0 && sticks[j]->data > key->data) {

					if (sort_State == SortingState::NotSorting) { break; }
					number_of_array_access++;
					number_of_comparisons++;
					sticks[j + 1] = sticks[j];
					sticks[j + 1]->stick_view->setFillColor(collection_model->processing_element_color); // Set the current stick to processing color
					j--;
					updateStickPosition();
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));


				}

				sticks[j + 1] = key; // Insert the key stick at the correct position
				number_of_array_access++;
				sticks[j + 1]->stick_view->setFillColor(collection_model->processing_element_color); // Set the inserted stick to placement position color
				updateStickPosition();
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
				sticks[j + 1]->stick_view->setFillColor(collection_model->selected_element_color); // Set the inserted stick to placement position color
			}
			completedColour();
		}
		void StickCollectionController::processSelectionSort() 
			{
				for (int i = 0; i < sticks.size() - 1; i++) {

					if (sort_State == SortingState::NotSorting) { break; } // Check if sorting has stopped or been interrupted

					int min = i;
					sticks[i]->stick_view->setFillColor(collection_model->selected_element_color);
					number_of_array_access++;

					for (int j = i; j < sticks.size(); j++) {
						if (sort_State == SortingState::NotSorting) { break; }
						number_of_array_access++;
						number_of_comparisons++;
						sticks[j]->stick_view->setFillColor(collection_model->processing_element_color);
						std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
						if (sticks[j]->data < sticks[min]->data) {
							sticks[min]->stick_view->setFillColor(collection_model->element_color);
							min = j;
							sticks[min]->stick_view->setFillColor(collection_model->processing_element_color);

						}
						else {
							sticks[j]->stick_view->setFillColor(collection_model->element_color);
						}

						number_of_array_access += 3;
						

						sticks[i]->stick_view->setFillColor(collection_model->placement_position_element_color);  // Mark as sorted
						updateStickPosition();

					}

					number_of_array_access += 3;
					std::swap(sticks[min], sticks[i]);  // Place the found minimum at its final position

					sticks[i]->stick_view->setFillColor(collection_model->placement_position_element_color);  // Mark as sorted
					updateStickPosition();

				}
			
		
				// Ensure the last stick is also marked as sorted
				sticks[sticks.size() - 1]->stick_view->setFillColor(collection_model->placement_position_element_color);
				completedColour();
			
		}

		void StickCollectionController::processMergeSort()
		{
			MergeSort(0, sticks.size() - 1);
			completedColour();

		}
		
		void StickCollectionController::InPlaceMerge(int left, int mid, int right)
		{
			int i = left;
			int j = mid + 1;

			while (i <= mid && j <= right) {
				number_of_comparisons++;
				number_of_array_access += 2;

				

				if (sticks[i]->data <= sticks[j]->data) {
					
					i++;
				}
				else {
					// Save the smaller value
					Stick* value = sticks[j];
					int index = j;

					// Shift elements rightward
					while (index!=i) {
						sticks[index] = sticks[index - 1];
						index--;
						number_of_array_access += 2;
					}

					// Put value at correct place
					sticks[i] = value;
					number_of_array_access += 2;

					i++;
					mid++;
					j++;
					updateStickPosition();
				}

				sticks[i- 1]->stick_view->setFillColor(collection_model->processing_element_color);
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
				sticks[i - 1]->stick_view->setFillColor(collection_model->element_color);
			}
		}

		void StickCollectionController::MergeSort(int left, int right)
		{
			if (left < right) {
				int mid = left + (right - left) / 2;

				
				MergeSort(left, mid);
				MergeSort(mid + 1, right);

				
				InPlaceMerge(left, mid, right);
			}
		}

		void StickCollectionController::processQuickSort()
		{
			quickSort(0, sticks.size() - 1);
			completedColour();
		}

		int StickCollectionController::partition(int low, int high)
		{
			sticks[high]->stick_view->setFillColor(collection_model->selected_element_color);
			int i = low- 1;

			for (int j = low; j < high; j++) {
				sticks[j]->stick_view->setFillColor(collection_model->processing_element_color);

				if (sticks[j]->data < sticks[high]->data) {
					i++;
					number_of_array_access += 3;
					number_of_comparisons++;
					std::swap(sticks[i], sticks[j]);
					updateStickPosition();
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

				}
				sticks[j]->stick_view->setFillColor(collection_model->element_color); 
			}
			std::swap(sticks[i + 1], sticks[high]);
			number_of_array_access += 3;
			return i + 1;
		}

		void StickCollectionController::quickSort(int low, int high)
		{
			if (low < high) {
				int location = partition(low, high);
				quickSort(low, location - 1);
				quickSort(location + 1, high);
			}
		}

		
		
		void StickCollectionController::completedColour()
		{

			for (int i = 0; i < sticks.size(); i++)
			{
				if (sort_State == SortingState::NotSorting) {
					break;
				}
				 
					sticks[i]->stick_view->setFillColor(collection_model->element_color); // Reset to default color if not completed
				

				
				for (int j = 0; j < sticks.size(); j++) {
				
					if (sort_State == SortingState::NotSorting) {

						break;
					}
				
					/*ServiceLocator::getInstance()->getSoundService()->playSound(Sound::SoundType::COMPARE_SFX);*/
					sticks[j]->stick_view->setFillColor(collection_model->placement_position_element_color);
					std::this_thread::sleep_for(std::chrono::milliseconds(color_delay));
				
				}
				
			}
		}
	}
}


