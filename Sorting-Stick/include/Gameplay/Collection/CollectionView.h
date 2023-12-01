#pragma once
#include <SFML/Graphics.hpp>
#include "UI/UIElement/ImageView.h"

namespace Gameplay
{
    namespace Collection
    {
        class CollectionController;

        class CollectionView
        {
        private:
            CollectionController* collection_controller;

        public:
            CollectionView();
            ~CollectionView();

            void initialize(CollectionController* collection_controller);
            void update();
            void render();
        };
    }
}