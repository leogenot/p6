#include "Context.h"
#include <glad/glad.h>

namespace p6 {

Context::Context(WindowCreationParams window_creation_params)
    : _window{window_creation_params}
{
}

void Context::run()
{
    while (!glfwWindowShouldClose(*_window)) {
        glfwSwapBuffers(*_window);
        glfwPollEvents();
    }
}

void Context::background(Color color) const
{
    glClearColor(color.r(), color.g(), color.b(), color.a());
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace p6