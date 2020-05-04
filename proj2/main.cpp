#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Axes.hpp"
#include "Camera.hpp"
#include "SPHRenderer.hpp"
#include "SPHSimulator.hpp"

namespace {
Camera camera({2, 2, 2}, {0, 0, 0}, 640, 480);
SPHRenderer renderer;
SPHSimulator simulator;

const auto time_step = 1E-3f;

auto simulating = false;

void FramebufferSizeCallback(GLFWwindow *, int width, int height) {
  if (width && height) {
    glViewport(0, 0, width, height);
    camera.Resize(width, height);
  }
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
  if (key == GLFW_KEY_ENTER && action == GLFW_REPEAT) {
    simulator.Update(time_step);
  }
}

GLFWwindow *Initialize() {
  // GLFW setup
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  // GLFW window
  const auto window =
      glfwCreateWindow(640, 480, "Solid Mechanics", nullptr, nullptr);
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

}  // namespace

int main() {
  const auto window = Initialize();

  Axes axes;
  simulator =
      SPHSimulator({-3.f, -3.f, -3.f}, {5.f, 5.f, 5.f}, [](const glm::vec3 &x) {
        return glm::distance(x, glm::vec3(0.f, 1.f, 0.f)) <= .5f;
      });
  renderer = SPHRenderer(simulator.GetParticles(), simulator.GetBox(),
                         simulator.GetDensityGrid());

  auto last_time = glfwGetTime();

  // Rendering
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  while (!glfwWindowShouldClose(window)) {
    const auto dt = glfwGetTime() - last_time;
    last_time = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPointSize(5.f);

    camera.Update(dt);
    if (simulating) {
      simulator.Update(time_step);
      renderer.Update(simulator.GetParticles());
      renderer.UpdateMarchingCubes(simulator.GetDensityGrid());
    }

    axes.Draw(camera);
    renderer.Draw(camera);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwDestroyWindow(window);
  glfwTerminate();
}