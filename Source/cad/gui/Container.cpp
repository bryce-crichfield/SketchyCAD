#pragma once

#include <cad/gui/Container.h>

namespace Cad {
void Container::Layout() {
    float layout_x = position.x;
    float layout_y = position.y;

    bool is_horizontal = direction == Direction::Horizontal;
    bool is_vertical = direction == Direction::Vertical;
    bool is_center = alignment == Alignment::Center;
    bool is_end = alignment == Alignment::End;
    bool is_start = alignment == Alignment::Start;

    if (is_horizontal) {
        // Calculate the total width of all children
        float total_width = 0;
        for (auto& child : children) {
            total_width += child->size.x;
        }

        // Calculate the x position of each child
        float layout_x = position.x;
        for (auto& child : children) {
            float child_width = child->size.x;
            float child_height = child->size.y;
            float child_x = layout_x;
            float child_y = layout_y;
            if (is_center) {
                child_x += (size.x - total_width) / 2;
            }
            if (is_end) {
                child_x += size.x - total_width;
            }
            child->position = {child_x, child_y};
            bool child_is_container = dynamic_cast<Container*>(child.get()) != nullptr;
            if (child_is_container) {
                Container* child_container = dynamic_cast<Container*>(child.get());
                child_container->Layout();
            }
            layout_x += child_width;
        }
    }

    if (is_vertical) {
        // Calculate the total height of all children
        float total_height = 0;
        for (auto& child : children) {
            total_height += child->size.y;
        }

        // Calculate the y position of each child
        float layout_y = position.y;
        for (auto& child : children) {
            float child_width = child->size.x;
            float child_height = child->size.y;
            float child_x = layout_x;
            float child_y = layout_y;
            if (is_center) {
                child_y += (size.y - total_height) / 2;
            }
            if (is_end) {
                child_y += size.y - total_height;
            }
            child->position = {child_x, child_y};
            bool child_is_container = dynamic_cast<Container*>(child.get()) != nullptr;
            if (child_is_container) {
                Container* child_container = dynamic_cast<Container*>(child.get());
                child_container->Layout();
            }
            layout_y += child_height;
        }
    }
}
} // namespace Cad
