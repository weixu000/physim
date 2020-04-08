#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/types_to_string.h>
#include <glbinding/gl/gl.h>
#include <globjects/globjects.h>

#include "Axes.hpp"
#include "Camera.hpp"
#include "Grid.hpp"
#include "GridRenderer.hpp"

using namespace gl;

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

Camera camera({-2, 1, -2}, {0, 0, 0}, 640, 480);

void FramebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
  camera.Resize(width, height);
}

void CursorPosCallback(GLFWwindow *, double x, double y) {
  camera.OnMouseMove(x, y);
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (action == GLFW_PRESS) {
      camera.OnMouseButtonPress(x, y);
    } else {
      camera.OnMouseButtonRelease(x, y);
    }
  }
}

Grid grid({0.5, .5, 0.5}, {1, 1, 1});
const auto time_step = 5E-3f;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  switch (key) {
  case GLFW_KEY_W:
    camera.forward_ = action != GLFW_RELEASE;
    break;
  case GLFW_KEY_A:
    camera.left_ = action != GLFW_RELEASE;
    break;
  case GLFW_KEY_S:
    camera.backward_ = action != GLFW_RELEASE;
    break;
  case GLFW_KEY_D:
    camera.right_ = action != GLFW_RELEASE;
    break;
  }
  if (key == GLFW_KEY_SPACE && action != GLFW_RELEASE) {
    grid.Update(time_step);
  }
}

GLFWwindow *Initialize() {
  // GLFW setup
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  // GLFW window
  const auto window = glfwCreateWindow(640, 480, "My Title", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  glfwSetCursorPosCallback(window, CursorPosCallback);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSetKeyCallback(window, KeyCallback);

  // GL context
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // globjects setup
  globjects::init(glfwGetProcAddress);

  std::cout << std::endl
            << "OpenGL Version:  " << glbinding::aux::ContextInfo::version()
            << std::endl
            << "OpenGL Vendor:   " << glbinding::aux::ContextInfo::vendor()
            << std::endl
            << "OpenGL Renderer: " << glbinding::aux::ContextInfo::renderer()
            << std::endl;
  return window;
}

int main() {
  const auto window = Initialize();

  Axes axes;
  GridRenderer renderer(grid.Particles(), grid.ParticleIndices());

  auto last_time = glfwGetTime();

  // Rendering
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glEnable(GL_DEPTH_TEST);
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto dt = glfwGetTime() - last_time;
    camera.Update(dt);
    renderer.Update(grid.Particles());
    last_time = glfwGetTime();

    axes.Draw(camera);
    renderer.Draw(camera);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwDestroyWindow(window);
  glfwTerminate();
}