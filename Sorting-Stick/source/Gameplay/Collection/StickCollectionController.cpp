#include "Gameplay/Collection/StickCollectionController.h"
#include "Gameplay/Collection/StickCollectionView.h"
#include "Gameplay/Collection/StickCollectionModel.h"
#include "Gameplay/GameplayService.h"
#include "Global/ServiceLocator.h"
#include "Gameplay/Collection/Stick.h"
#include <random>

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
				float x_position = (i * sticks[i]->stick_view->getSize().x) + ((i) * collection_model->elements_spacing);
				float y_position = collection_model->element_y_position - sticks[i]->stick_view->getSize().y;

				sticks[i]->stick_view->setPosition(sf::Vector2f(x_position, y_position));
			}
		}
		
		void StickCollectionController::processBubbleSort()
		{
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();
			for (int j = 0; j < sticks.size(); j++)
			{
				bool swapped = false;  // To track if a swap was made
				for (int i = 1; i < sticks.size() - j; i++)  // Reduce the range each pass
				{
					number_of_array_access += 2;
					number_of_comparisons++;
					sound->playSound(SoundType::COMPARE_SFX);

					sticks[i - 1]->stick_view->setFillColor(collection_model->processing_element_color);
					sticks[i]->stick_view->setFillColor(collection_model->processing_element_color);

					if (sticks[i - 1]->data > sticks[i]->data) {
						std::swap(sticks[i - 1], sticks[i]);
						swapped = true;  // Set swapped to true if there was a swap
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

					sticks[i - 1]->stick_view->setFillColor(collection_model->element_color);
					sticks[i]->stick_view->setFillColor(collection_model->element_color);
					updateStickPosition();
				}
				// Set the last sorted stick to green
				if (sticks.size() - j - 1 >= 0) {
					sticks[sticks.size() - j - 1]->stick_view->setFillColor(collection_model->placement_position_element_color);
				}
				// If no swaps were made, the array is already sorted
				if (!swapped)
					break;
			}

			setCompletedColor();
		}

		void StickCollectionController::processInsertionSort()
		{
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();

			for (int i = 1; i < sticks.size(); ++i)
			{
				int j = i - 1;
				Stick* key = sticks[i];
				number_of_array_access++; // Access for key stick

				sound->playSound(SoundType::COMPARE_SFX);

				key->stick_view->setFillColor(collection_model->processing_element_color); // Current key is red
				updateStickPosition(); // Update to show the key in red

				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

				while (j >= 0 && sticks[j]->data > key->data)
				{
					number_of_comparisons++;
					number_of_array_access++; // Access for comparing sticks[j]->data with key->data

					sticks[j + 1] = sticks[j];
					number_of_array_access++; // Access for assigning sticks[j] to sticks[j + 1]

					sticks[j + 1]->stick_view->setFillColor(collection_model->selected_element_color); // Mark as being compared
					j--;
					sound->playSound(SoundType::COMPARE_SFX);
					updateStickPosition(); // Visual update
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
				}

				sticks[j + 1] = key;
				number_of_array_access++; // Access for placing the key in sticks[j + 1]
				sticks[j + 1]->stick_view->setFillColor(collection_model->placement_position_element_color); // Placed key is green indicating it's sorted
				updateStickPosition(); // Final visual update for this iteration
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
			}

			setCompletedColor();
		}

		void StickCollectionController::processSelectionSort()
		{
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();
			for (int i = 0; i < sticks.size() - 1; ++i)
			{
				int min_index = i;
				sticks[i]->stick_view->setFillColor(collection_model->processing_element_color);  // Mark the start of processing

				for (int j = i + 1; j < sticks.size(); ++j)
				{
					number_of_array_access += 2;
					number_of_comparisons++;

					sound->playSound(SoundType::COMPARE_SFX);
					sticks[j]->stick_view->setFillColor(collection_model->processing_element_color);
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));

					if (sticks[j]->data < sticks[min_index]->data)
					{
						if (min_index != i) sticks[min_index]->stick_view->setFillColor(collection_model->element_color);  // Reset previous min
						min_index = j;
						sticks[min_index]->stick_view->setFillColor(collection_model->selected_element_color);  // New min found
					}
					else
					{
						sticks[j]->stick_view->setFillColor(collection_model->element_color);  // Not the minimum, reset color
					}
				}

				number_of_array_access += 3;
				std::swap(sticks[min_index], sticks[i]);  // Place the found minimum at its final position

				sticks[i]->stick_view->setFillColor(collection_model->placement_position_element_color);  // Mark as sorted
				updateStickPosition();
			}

			// Ensure the last stick is also marked as sorted
			sticks[sticks.size() - 1]->stick_view->setFillColor(collection_model->placement_position_element_color);

			setCompletedColor();  // Optional if you want to re-mark everything, can be redundant
		}

		void StickCollectionController::merge(int left, int mid, int right)
		{
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();
			std::vector<Stick*> temp(right - left + 1);  // Temporary vector to hold pointers
			int i = left, j = mid + 1, k = 0;

			// Copy pointers from the original sticks array to temp
			for (int index = left; index <= right; ++index) {
				temp[k++] = sticks[index];
				sticks[index]->stick_view->setFillColor(collection_model->temporary_processing_color);  // Temporary color
				number_of_array_access++;
			}

			i = 0; // reset i to start of temp
			j = mid - left + 1; // adjust j to start of second half in temp
			k = left; // reset k to left to start filling the main array

			while (i < mid - left + 1 && j < temp.size()) {
				sound->playSound(SoundType::COMPARE_SFX);  // Play sound at compare
				number_of_comparisons++;
				if (temp[i]->data <= temp[j]->data) {
					sticks[k] = temp[i++];
				}
				else {
					sticks[k] = temp[j++];
					sticks[k]->stick_view->setFillColor(collection_model->selected_element_color);  // Highlight the selected element
					updateStickPosition();
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
				}
				number_of_array_access++; // Access for reassigning from temp
				sticks[k]->stick_view->setFillColor(collection_model->processing_element_color);  // Processing color
				updateStickPosition();
				std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
				k++;
			}

			// Copy remaining elements of left half, if any
			while (i < mid - left + 1) {
				sticks[k] = temp[i++];
				sticks[k]->stick_view->setFillColor(collection_model->selected_element_color);  // Use selected color during merge
				updateStickPosition();
				number_of_array_access++; // Access for reassigning remaining elements
				k++;
			}

			// Copy remaining elements of right half, if any
			while (j < temp.size()) {
				sticks[k] = temp[j++];
				sticks[k]->stick_view->setFillColor(collection_model->selected_element_color);  // Sorted color
				updateStickPosition();
				number_of_array_access++; // Access for reassigning remaining elements
				k++;
			}
		}

		void StickCollectionController::mergeSort(int left, int right)
		{
			if (left >= right) return;
			int mid = left + (right - left) / 2;

			mergeSort(left, mid);
			mergeSort(mid + 1, right);
			merge(left, mid, right);
		}

		void StickCollectionController::processMergeSort()
		{
			mergeSort(0, sticks.size() - 1);

			if (current_operation_delay != 0)
			{
				setCompletedColor(); //set all sticks green (dramatic effect)
			}

		}

		int StickCollectionController::partition(int left, int right)
		{
			
			sticks[right]->stick_view->setFillColor(collection_model->selected_element_color);
			int i = left - 1;
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();

			for (int j = left; j < right; ++j)
			{
				
				sticks[j]->stick_view->setFillColor(collection_model->processing_element_color);
				number_of_array_access += 2;
				number_of_comparisons++;

				if (sticks[j]->data < sticks[right]->data)
				{
					++i;
					std::swap(sticks[i], sticks[j]);
					number_of_array_access += 3;
					sound->playSound(SoundType::COMPARE_SFX);

					
					updateStickPosition();
					std::this_thread::sleep_for(std::chrono::milliseconds(current_operation_delay));
				}

				// Reset the color of the processed element if it's not swapped
				sticks[j]->stick_view->setFillColor(collection_model->element_color);
			}

			std::swap(sticks[i + 1], sticks[right]);
			number_of_array_access += 3;

			// Final placement color for the pivot
			sticks[i + 1]->stick_view->setFillColor(collection_model->placement_position_element_color);
			updateStickPosition();
			return i + 1;
		}

		void StickCollectionController::quickSort(int left, int right)
		{
			if (left < right)
			{
				int pivot_index = partition(left, right);

				quickSort(left, pivot_index - 1);
				quickSort(pivot_index + 1, right);
			}
		}

		void StickCollectionController::processQuickSort()
		{
			quickSort(0, sticks.size() - 1);

			setCompletedColor();
		}



		void StickCollectionController::setCompletedColor()
		{
			for (int k = 0; k < sticks.size(); k++)
			{
				sticks[k]->stick_view->setFillColor(collection_model->element_color);
			}
			SoundService* sound = Global::ServiceLocator::getInstance()->getSoundService();

			for (int i = 0; i < sticks.size(); ++i)
			{
				sound->playSound(SoundType::COMPARE_SFX);
				sticks[i]->stick_view->setFillColor(collection_model->placement_position_element_color);

				// Delay to visualize the final color change
				std::this_thread::sleep_for(std::chrono::milliseconds(color_delay));

			}

			if (color_delay != 0)
			{
				sound->playSound(SoundType::SCREAM);
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
			if (sort_thread.joinable() && isCollectionSorted()) sort_thread.join();
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
			color_delay = 0;
			current_operation_delay = 0;
			if (sort_thread.joinable()) sort_thread.join();

			shuffleSticks();
			resetSticksColor();
			resetVariables();
		}

		void StickCollectionController::sortElements(SortType sort_type)
		{
			current_operation_delay = collection_model->operation_delay;
			color_delay = collection_model->initial_color_delay;
			this->sort_type = sort_type;

			switch (sort_type)
			{
			case Gameplay::Collection::SortType::BUBBLE_SORT:
				time_complexity = "O(n^2)";
				sort_thread = std::thread(&StickCollectionController::processBubbleSort, this);
				break;
			case Gameplay::Collection::SortType::INSERTION_SORT:
				time_complexity = "O(n^2)";
				sort_thread = std::thread(&StickCollectionController::processInsertionSort, this);
				break;
			case Gameplay::Collection::SortType::SELECTION_SORT:
				time_complexity = "O(n^2)";
				sort_thread = std::thread(&StickCollectionController::processSelectionSort, this);
				break;
			case Gameplay::Collection::SortType::MERGE_SORT:
				time_complexity = "O(n Log n)";
				sort_thread = std::thread(&StickCollectionController::processMergeSort, this);
				break;
			case Gameplay::Collection::SortType::QUICK_SORT:
				time_complexity = "O(n Log n)";
				sort_thread = std::thread(&StickCollectionController::processQuickSort, this);
				break;
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
	}
}


