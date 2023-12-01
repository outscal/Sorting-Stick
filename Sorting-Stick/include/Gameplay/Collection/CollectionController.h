#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>

namespace Gameplay
{
    namespace Collection
    {
        class CollectionView;
        class CollectionModel;
        struct Element;
        enum class SortType;

        class CollectionController
        {
        private:
            CollectionView* collection_view;
            CollectionModel* collection_model;

            std::vector<Element*> elements;
            SortType sort_type;

            std::thread sort_thread;

            int number_of_comparisons;
            int number_of_array_access;
            int current_operation_delay;

            void initializeElements();
            float calculateElementWidth();

            void updateElementsPosition();
            void shuffleElements();
            bool compareElementsByData(const Element* a, const Element* b) const;

            void resetElementsColor();
            void resetVariables();

            void processSortThreadState();
            void processBubbleSort();

            bool isCollectionSorted();
            void destroy();

        public:
            CollectionController();
            ~CollectionController();

            void initialize();
            void update();
            void render();

            void reset();
            void sortElements(SortType sort_type);

            SortType getSortType();
            int getNumberOfComparisons();
            int getNumberOfArrayAccess();
        };
    }
}