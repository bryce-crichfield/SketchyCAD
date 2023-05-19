#pragma once

#include <vector>
namespace Core {
struct InputState {
    std::vector<char> char_buffer;

    float last_mouse_x, last_mouse_y;
    float current_mouse_x, current_mouse_y;
    float scroll_delta;

    bool previous_mouse[3];
    bool current_mouse[3];

    bool previous_keys[256];
    bool current_keys[256];

    InputState();
    
    void Update();
    void Clear();
};
}