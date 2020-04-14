#include <glad/glad.h>

#include "Axes.hpp"
#include "Camera.hpp"
#include "Grid.hpp"
#include "GridRenderer.hpp"

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
      camera.OnMouseButtonRelease();
    }
  }
}

Grid grid({0.5, 2., 0.5}, {.5, .5, .5}, {4, 4, 4}, 100.f, .4f, 1.f);
const auto time_step = 1E-3f;

auto wireframe = false;
auto simulating = false;

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
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    simulating = !simulating;
  }
  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    wireframe = !wireframe;
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

  // glad setup
  if (gladLoadGL() == 0) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
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
    const auto dt = glfwGetTime() - last_time;
    last_time = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Update(dt);
    // Do multiple physical simulation in one render loop
    for (auto ddt = dt; simulating && ddt > 0.f; ddt -= time_step) {
      grid.Update(time_step);
    }
    renderer.Update(grid.Particles());

    axes.Draw(camera);
    if (wireframe) {
      renderer.DrawTetrahedra(camera);
    } else {
      renderer.DrawSurface(camera);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwDestroyWindow(window);
  glfwTerminate();
}