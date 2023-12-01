#pragma once
#include "UI/UIElement/RectangleShapeView.h"

namespace Gameplay
{
    namespace Collection
    {
        struct Element
        {
            int data;
            UI::UIElement::RectangleShapeView* element_view;

            Element() { }

            Element(int data)
            {
                this->data = data;
                element_view = new UI::UIElement::RectangleShapeView();
            }

            ~Element() { delete(element_view); }
        };
    }
}