#include "Gameplay/Collection/CollectionView.h"

namespace Gameplay
{
    namespace Collection
    {
        CollectionView::CollectionView() { }

        CollectionView::~CollectionView() { }

		void CollectionView::initialize(CollectionController* collection_controller)
		{
			this->collection_controller = collection_controller;
		}

		void CollectionView::update() { }

		void CollectionView::render() { }
    }
}