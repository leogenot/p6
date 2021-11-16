#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <stdexcept>
#include "Color.h"
#include "KeyEvent.h"
#include "MouseButton.h"
#include "MouseDrag.h"
#include "MouseMove.h"
#include "MouseScroll.h"
#include "RectangleParams.h"
#include "Shader.h"
#include "details/RectRenderer.h"
#include "details/Time/Clock.h"
#include "details/Time/Clock_FixedTimestep.h"
#include "details/Time/Clock_Realtime.h"
#include "details/UniqueGlfwWindow.h"

namespace p6 {

class Context {
public:
    Context(WindowCreationParams window_creation_params = {});
    void run();

    /* ---------------------------------------- *
     * ---------CUSTOMIZABLE FUNCTIONS--------- *
     * ---------------------------------------- */

    /// This function is called repeatedly, once every 1/framerate() seconds (or at least it will try, if your update() code is too slow then the next updates will necessarily be delayed).
    std::function<void()> update = []() {};
    /// This function is called whenever the mouse is moved
    std::function<void(MouseMove)> mouse_moved = [](MouseMove) {};
    /// This function is called whenever the mouse is dragged
    std::function<void(MouseDrag)> mouse_dragged = [](MouseDrag) {};
    /// This function is called whenever a mouse button is pressed
    std::function<void(MouseButton)> mouse_pressed = [](MouseButton) {};
    /// This function is called whenever a mouse button is released
    std::function<void(MouseButton)> mouse_released = [](MouseButton) {};
    /// This function is called whenever the mouse wheel is scrolled
    std::function<void(MouseScroll)> mouse_scrolled = [](MouseScroll) {};
    /// This function is called whenever a keyboard key is pressed
    std::function<void(KeyEvent)> key_pressed = [](KeyEvent) {};
    /// This function is called whenever a keyboard key is released
    std::function<void(KeyEvent)> key_released = [](KeyEvent) {};
    /// This function is called whenever a keyboard key is held for a little while.
    std::function<void(KeyEvent)> key_repeated = [](KeyEvent) {};
    /// This function is called whenever an error occurs.
    std::function<void(std::string&&)> on_error = [](std::string&& error_message) {
        throw std::runtime_error{error_message};
    };

    /* ------------------------- *
     * ---------DRAWING--------- *
     * ------------------------- */

    Color fill{1.f, 1.f, 1.f, 0.5f};
    Color stroke{0.f, 0.f, 0.f};
    float stroke_weight = 0.01f;

    /// Sets the color and alpha of each pixel of the canvas.
    /// NB: No blending is applied ; even if you specify an alpha of 0.5 the old canvas is completely erased. This means that setting an alpha here doesn't matter much. It is only meaningful if you export the canvas as a png, or if you try to blend the canvas on top of another image.
    void background(Color color) const;

    void rectangle(RectangleParams params) const;

    /* ----------------------- *
     * ---------INPUT--------- *
     * ----------------------- */

    /// Returns the current mouse position
    glm::vec2 mouse() const;
    /// Returns the movement of the mouse since last update()
    glm::vec2 mouse_delta() const;
    /// Returns true iff the coordinates returned by mouse() correspond to a position inside the window, and the window is focused
    bool mouse_is_in_window() const;

    /// Returns true iff the CTRL key is pressed (or CMD on Mac)
    bool ctrl() const;
    /// Returns true iff the SHIFT key is pressed
    bool shift() const;
    /// Returns true iff the ALT key is pressed
    bool alt() const;

    /* ------------------------ *
     * ---------WINDOW--------- *
     * ------------------------ */

    /// Returns the aspect ratio of the window (a.k.a. width / height)
    float aspect_ratio() const;
    /// Returns true iff the window is currently focused
    bool window_is_focused() const;
    /// Maximizes the window
    void maximize_window();

    /* ---------------------- *
     * ---------TIME--------- *
     * ---------------------- */

    /// Returns the time in seconds since the creation of the Context
    float time() const;

    /// Returns the time in seconds since the last update() call (or 0 if this is the first update)
    float delta_time() const;

    /// Sets the time_mode as realtime.
    /// This means that what is returned by time() and delta_time() corresponds to the actual time that elapsed in the real world.
    /// This is ideal when you want to do realtime animation and interactive sketches.
    void set_time_mode_realtime();

    /// Sets the time_mode as fixedstep.
    /// This means that what is returned by time() and delta_time() corresponds to an ideal world where there is exactly 1/framerate seconds between each updates.
    /// This is ideal when you are exporting a video and don't want the long export time to influence your animation.
    void set_time_mode_fixedstep();

    /* ------------------------------- *
     * ---------MISCELLANEOUS--------- *
     * ------------------------------- */

    /// Starts the loop again if it was paused with no_loop()
    void loop();
    /// Pauses the loop. No update() will be called, until you call loop(). User inputs are still processed.
    void no_loop();
    /// Returns true iff we are currently looping. See loop() and no_loop()
    bool is_looping() const;

private:
    glm::vec2 window_to_relative_coords(glm::vec2 pos) const;

    void        on_window_resize(int width, int height);
    friend void window_size_callback(GLFWwindow* window, int width, int height);
    void        on_mouse_scroll(double x, double y);
    friend void scroll_callback(GLFWwindow* window, double x, double y);
    void        on_mouse_button(int button, int action, int mods);
    friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void        on_key(int key, int scancode, int action, int mods);
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void      on_mouse_move();
    void      check_for_mouse_movements();
    glm::vec2 compute_mouse_position() const;

private:
    mutable details::UniqueGlfwWindow _window;
    std::unique_ptr<details::Clock>   _clock = std::make_unique<details::Clock_Realtime>();
    details::RectRenderer             _rect_renderer;
    int                               _width;
    int                               _height;
    glm::vec2                         _mouse_position;
    glm::vec2                         _mouse_position_delta{0.f, 0.f};
    glm::vec2                         _drag_start_position{};
    bool                              _is_dragging = false;
    Shader                            _rect_shader{R"(
#version 330

in vec2 _uv;
in vec2 _uv_canvas_scale;
out vec4 _frag_color;

uniform vec4 _fill_color;
uniform vec4 _stroke_color;
uniform float _stroke_weight;
uniform vec2 _rect_size;

void main() {
    vec2 dist = _rect_size - abs(_uv_canvas_scale);
    const float m = 0.0005;
    float t = smoothstep(-m, m, _stroke_weight - min(dist.x, dist.y));
    _frag_color = vec4(mix(_fill_color, _stroke_color, t));
}
    )"};
};

} // namespace p6
