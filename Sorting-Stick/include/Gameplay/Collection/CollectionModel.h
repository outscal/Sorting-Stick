#pragma once
#include <SFML/Graphics.hpp>

namespace Gameplay
{
    namespace Collection
    {
        enum class SortType
        {
            BUBBLE_SORT,
            INSERTION_SORT,
            SELECTION_SORT,
            MERGE_SORT,
            QUICK_SORT,
            RADIX_SORT,
        };

        class CollectionModel
        {
        public:
            const int number_of_elements = 50;
            const float max_element_height = 820.f;
            const float elements_spacing = 25.f;
            const float element_y_position = 1020.f;

            const sf::Color element_color = sf::Color::White;
            const sf::Color processing_element_color = sf::Color::Red;
            const sf::Color placement_position_element_color = sf::Color::Green;
            const sf::Color selected_element_color = sf::Color::Blue;

            const long operation_delay = 100;

            CollectionModel();
            ~CollectionModel();
        };
    }
}