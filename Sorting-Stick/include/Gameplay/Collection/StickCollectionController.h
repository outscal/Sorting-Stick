#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>

namespace Gameplay
{
    namespace Collection
    {
        class StickCollectionView;
        class StickCollectionModel;
        struct Stick;
        enum class SortType;
        enum class SortingState;

        class StickCollectionController
        {
        private:
            StickCollectionView* collection_view;
            StickCollectionModel* collection_model;

            std::vector<Stick*> sticks;
            SortType sort_type;
            SortingState sort_State;

            std::thread sort_thread;

            int number_of_comparisons;
            int number_of_array_access;
            int current_operation_delay;

            int delay_in_ms;
            sf::String time_complexity;

            void initializeSticks();
            float calculateStickWidth();
            float calculateStickHeight(int array_pos);

            void updateStickPosition();
            void shuffleSticks();
            bool compareSticksByData(const Stick* a, const Stick* b) const;

            void resetSticksColor();
            void resetVariables();

            void processSortThreadState();

            bool isCollectionSorted();
            void destroy();

            int color_delay;

        public:
            StickCollectionController();
            ~StickCollectionController();

            void initialize();
            void update();
            void render();

            void reset();
            void sortElements(SortType sort_type);

            SortType getSortType();
            int getNumberOfComparisons();
            int getNumberOfArrayAccess();

            int getNumberOfSticks();
            int getDelayMilliseconds();
            sf::String getTimeComplexity();
            void processBubbleSort();
			void processInsertionSort();
            void processSelectionSort();
			void processMergeSort();
            void InPlaceMerge(int left, int mid, int right);
			void MergeSort(int left, int right);
			void processQuickSort();
			int partition(int low, int high);
            void quickSort(int low, int high);
            void ProcessRadixSort();
            void completedColour();
            void  countSort(int exponent);
			void RadixSort();
           void updateStickPosition(int i);
        };
    }
}