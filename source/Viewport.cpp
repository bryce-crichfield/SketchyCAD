#include <Engine.h>

#include <algorithm>

#include <glad/glad.h>
#include <glfw/glfw3.h>

inline Key mapGlfwKeyToCustomKeyEnum(int keycode)
{
    // clang-format off
    switch (keycode) {
    case GLFW_KEY_A: return Key::A;     case GLFW_KEY_J: return Key::J;     case GLFW_KEY_S: return Key::S;
    case GLFW_KEY_B: return Key::B;     case GLFW_KEY_K: return Key::K;     case GLFW_KEY_T: return Key::T;
    case GLFW_KEY_C: return Key::C;     case GLFW_KEY_L: return Key::L;     case GLFW_KEY_U: return Key::U;
    case GLFW_KEY_D: return Key::D;     case GLFW_KEY_M: return Key::M;     case GLFW_KEY_V: return Key::V;
    case GLFW_KEY_E: return Key::E;     case GLFW_KEY_N: return Key::N;     case GLFW_KEY_W: return Key::W;
    case GLFW_KEY_F: return Key::F;     case GLFW_KEY_O: return Key::O;     case GLFW_KEY_X: return Key::X;
    case GLFW_KEY_G: return Key::G;     case GLFW_KEY_P: return Key::P;     case GLFW_KEY_Y: return Key::Y;
    case GLFW_KEY_H: return Key::H;     case GLFW_KEY_Q: return Key::Q;     case GLFW_KEY_Z: return Key::Z;
    case GLFW_KEY_I: return Key::I;     case GLFW_KEY_R: return Key::R; 
    
    case GLFW_KEY_0: return Key::Num0;
    case GLFW_KEY_1: return Key::Num1;
    case GLFW_KEY_2: return Key::Num2;
    case GLFW_KEY_3: return Key::Num3;
    case GLFW_KEY_4: return Key::Num4;
    case GLFW_KEY_5: return Key::Num5;
    case GLFW_KEY_6: return Key::Num6;
    case GLFW_KEY_7: return Key::Num7;
    case GLFW_KEY_8: return Key::Num8;
    case GLFW_KEY_9: return Key::Num9;

    case GLFW_KEY_ESCAPE: return Key::Escape;           case GLFW_KEY_MENU: return Key::Menu;
    case GLFW_KEY_LEFT_CONTROL: return Key::LControl;   case GLFW_KEY_LEFT_BRACKET: return Key::LBracket;
    case GLFW_KEY_LEFT_SHIFT: return Key::LShift;       case GLFW_KEY_RIGHT_BRACKET: return Key::RBracket;
    case GLFW_KEY_LEFT_ALT: return Key::LAlt;           case GLFW_KEY_SEMICOLON: return Key::Semicolon;
    case GLFW_KEY_LEFT_SUPER: return Key::LSystem;      case GLFW_KEY_COMMA: return Key::Comma;
    case GLFW_KEY_RIGHT_CONTROL: return Key::RControl;  case GLFW_KEY_PERIOD: return Key::Period;
    case GLFW_KEY_RIGHT_SHIFT: return Key::RShift;      case GLFW_KEY_APOSTROPHE: return Key::Quote;
    case GLFW_KEY_RIGHT_ALT: return Key::RAlt;          case GLFW_KEY_SLASH: return Key::Slash;
    case GLFW_KEY_RIGHT_SUPER: return Key::RSystem;     case GLFW_KEY_BACKSLASH: return Key::Backslash;

    case GLFW_KEY_GRAVE_ACCENT: return Key::Tilde;      case GLFW_KEY_EQUAL: return Key::Equal;
    case GLFW_KEY_MINUS: return Key::Hyphen;           case GLFW_KEY_SPACE: return Key::Space;
    case GLFW_KEY_ENTER: return Key::Enter;            case GLFW_KEY_BACKSPACE: return Key::Backspace;
    case GLFW_KEY_TAB: return Key::Tab;                case GLFW_KEY_PAGE_UP: return Key::PageUp;
    case GLFW_KEY_PAGE_DOWN: return Key::PageDown;     case GLFW_KEY_END: return Key::End;
    case GLFW_KEY_HOME: return Key::Home;              case GLFW_KEY_INSERT: return Key::Insert;
    case GLFW_KEY_DELETE: return Key::Delete;          case GLFW_KEY_KP_ADD: return Key::Add;
    case GLFW_KEY_KP_SUBTRACT: return Key::Subtract;   case GLFW_KEY_KP_MULTIPLY: return Key::Multiply;
    case GLFW_KEY_KP_DIVIDE: return Key::Divide;       case GLFW_KEY_LEFT: return Key::Left;
    case GLFW_KEY_RIGHT: return Key::Right;            case GLFW_KEY_UP: return Key::Up;
    case GLFW_KEY_DOWN: return Key::Down;              
    
    case GLFW_KEY_KP_0: return Key::Kp0;           case GLFW_KEY_KP_1: return Key::Kp1;
    case GLFW_KEY_KP_2: return Key::Kp2;           case GLFW_KEY_KP_3: return Key::Kp3;
    case GLFW_KEY_KP_4: return Key::Kp4;           case GLFW_KEY_KP_5: return Key::Kp5;
    case GLFW_KEY_KP_6: return Key::Kp6;           case GLFW_KEY_KP_7: return Key::Kp7;
    case GLFW_KEY_KP_8: return Key::Kp8;           case GLFW_KEY_KP_9: return Key::Kp9;
    default: return Key::Unknown;
    }
    // clang-format on
}

bool Viewport::IsOpen() const
{
    return !glfwWindowShouldClose((GLFWwindow *)m_data);
}

unsigned Viewport::GetWidth() const
{
    return m_width;
}

unsigned Viewport::GetHeight() const
{
    return m_height;
}

void Viewport::SetTitle(const std::string &title)
{
    glfwSetWindowTitle((GLFWwindow *)m_data, title.c_str());
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    InputState *input_state = (InputState *)glfwGetWindowUserPointer(window);
    input_state->scroll_delta += yoffset;
}

void char_callback(GLFWwindow *window, unsigned int codepoint)
{
    InputState *input_state = (InputState *)glfwGetWindowUserPointer(window);
    input_state->char_buffer.push_back(codepoint);
}

Viewport::Viewport(unsigned width, unsigned height, unsigned viewarea_width, unsigned viewarea_height) throw()
    : m_width(width), m_height(height)
{
    if (!glfwInit())
    {
        throw std::string("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    m_data = glfwCreateWindow(width, height, "", nullptr, nullptr);
    if (!m_data)
    {
        glfwTerminate();
        throw std::string("Failed to create window");
    }

    glfwMakeContextCurrent((GLFWwindow *)m_data);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        throw std::string("Failed to initialize GLAD");
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, width, height);

    this->framebuffer = std::make_unique<Image>(viewarea_width, viewarea_height);

    // Set glfw user pointer to this viewport
    glfwSetWindowUserPointer((GLFWwindow *)m_data, &this->m_input_state);

    // Register callback for scroll input
    glfwSetScrollCallback((GLFWwindow *)m_data, scroll_callback);

    // Register callback for text entry
    glfwSetCharCallback((GLFWwindow *)m_data, char_callback);

    glfwSetInputMode((GLFWwindow *)m_data, GLFW_STICKY_KEYS, GLFW_FALSE);
    glfwSetInputMode((GLFWwindow *)m_data, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

Viewport::~Viewport()
{
    glfwTerminate();
}

void Viewport::Flush()
{
    m_input_state.Clear();
    glfwSwapBuffers((GLFWwindow *)m_data);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
}

void Viewport::UpdateFramebuffer()
{
    Image &viewarea_image = *this->framebuffer;
    // 1.) Create a texture and bind the image data to it
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewarea_image.GetWidth(), viewarea_image.GetHeight(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, viewarea_image.GetData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    // 2.) Transform the projection matrix such that the image is drawn in the center of the screen
    // To achieve this, determine the required x and y scale factors to essentially squeeze the image into the screen
    float largest_viewport_dimension = std::max(m_width, m_height);
    float x_scale = largest_viewport_dimension / m_width;
    float y_scale = largest_viewport_dimension / m_height;

    x_scale *= viewarea_image.GetWidth() / (float)largest_viewport_dimension;
    y_scale *= viewarea_image.GetHeight() / (float)largest_viewport_dimension;

    float scale = std::max(x_scale, y_scale);
    x_scale /= scale;
    y_scale /= scale;

    // Flip over the y axis
    y_scale *= -1.0f;

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScalef(x_scale, y_scale, 1.0f);
    // 3.) Draw a transformed quad with the texture
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);

    glEnd();
    // 4.) Unbind all targets, and delete the texture
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture);
    glPopMatrix();
}

Input Viewport::UpdateInput()
{
    // TODO: Mouse picking breaks down if the viewarea is larger than the viewport
    m_input_state.Update();

    unsigned viewarea_width = this->framebuffer->GetWidth();
    unsigned viewarea_height = this->framebuffer->GetHeight();
    bool mouse_inside_viewarea = false;
    // Calculate mouse position in viewarea and copy to input state ----------------------------------------------------
    {
        double mouse_x, mouse_y = 0;
        glfwGetCursorPos((GLFWwindow *)m_data, &mouse_x, &mouse_y);
        double screen_aspect = m_width / (double)m_height;
        double viewarea_aspect = viewarea_width / (double)viewarea_height;
        bool screen_is_wider_than_viewarea = screen_aspect > viewarea_aspect;

        if (screen_is_wider_than_viewarea)
        {
            double viewarea_calculated_width = m_height * viewarea_aspect;
            double viewarea_start_x = (m_width - viewarea_calculated_width) / 2.0f;
            double viewarea_end_x = viewarea_start_x + viewarea_calculated_width;

            bool is_mouse_x_inside_viewarea = mouse_x > viewarea_start_x && mouse_x < viewarea_end_x;
            if (is_mouse_x_inside_viewarea)
            {
                // mouse_x = std::clamp(mouse_x, viewarea_start_x, viewarea_end_x);
                // mouse_y = std::clamp(mouse_y, 0.0, (double)m_height);
                mouse_x -= viewarea_start_x;
                m_input_state.current_mouse_x = (mouse_x / viewarea_calculated_width) * viewarea_width;
                m_input_state.current_mouse_y = (mouse_y / m_height) * viewarea_height;
                mouse_inside_viewarea = true;
            }
        }
        else
        {
            double viewarea_calculated_height = m_width / viewarea_aspect;
            double viewarea_start_x = (m_height - viewarea_calculated_height) / 2.0f;
            double viewarea_end_x = viewarea_start_x + viewarea_calculated_height;

            bool is_mouse_y_inside_viewarea = mouse_y > viewarea_start_x && mouse_y < viewarea_end_x;
            if (is_mouse_y_inside_viewarea)
            {
                // mouse_x = std::clamp(mouse_x, 0.0, (double)m_width);
                // mouse_y = std::clamp(mouse_y, viewarea_start_x, viewarea_end_x);
                mouse_x = (mouse_x / m_width) * viewarea_width;
                m_input_state.current_mouse_y -= viewarea_start_x;
                m_input_state.current_mouse_x = (mouse_x / viewarea_width) * viewarea_width;
                m_input_state.current_mouse_y = (mouse_y / viewarea_calculated_height) * viewarea_height;
                mouse_inside_viewarea = true;
            }
        }
    }
    // Copy the mouse and keyboard button states -----------------------------------------------------------------------
    {
        if (mouse_inside_viewarea)
        {
            m_input_state.current_mouse[static_cast<unsigned char>(MouseButton::LEFT)] =
                glfwGetMouseButton((GLFWwindow *)m_data, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

            m_input_state.current_mouse[static_cast<unsigned char>(MouseButton::RIGHT)] =
                glfwGetMouseButton((GLFWwindow *)m_data, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

            m_input_state.current_mouse[static_cast<unsigned char>(MouseButton::MIDDLE)] =
                glfwGetMouseButton((GLFWwindow *)m_data, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
        }

        for (int i = 0; i < 512; i++)
        {
            Key key = mapGlfwKeyToCustomKeyEnum(i);
            int ordinal = static_cast<int>(key);

            m_input_state.current_keys[ordinal] = glfwGetKey((GLFWwindow *)m_data, i) == GLFW_PRESS;
        }
    }
    // -----------------------------------------------------------------------------------------------------------------

    return Input(m_input_state);
}

Graphics Viewport::GetGraphics()
{
    return this->framebuffer->GetGraphics();
}