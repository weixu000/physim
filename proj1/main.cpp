#include <glad/glad.h>

#include "Axes.hpp"
#include "Camera.hpp"
#include "Grid.hpp"
#include "GridRenderer.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera camera({-2, 1, -2}, {0, 0, 0}, 640, 480);

void FramebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
  camera.Resize(width, height);
}

void CursorPosCallback(GLFWwindow *, double x, double y) {
  camera.OnMouseMove(x, y);
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  if (ImGui::GetIO().WantCaptureMouse) {
    camera.OnMouseButtonRelease();
    return;
  }
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

auto origin = glm::vec3(.5f, 2.f, .5f);
auto cell = glm::vec3(.5f, .5f, .5f);
auto size = glm::uvec3(4, 4, 4);

Grid grid(origin, cell, size, 100.f, .4f, 1.f);
auto time_step = 1E-3f;

std::unique_ptr<GridRenderer> renderer;

auto wireframe = false;
auto simulating = false;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (ImGui::GetIO().WantCaptureKeyboard) {
    return;
  }
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

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  auto &io = ImGui::GetIO();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");

  return window;
}

void RenderUI() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  static auto show_demo_window = true;
  if (show_demo_window) {
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  ImGui::Begin("Control", nullptr);
  ImGui::Text("W, A, S, D to move camera");
  ImGui::Text("Hold left mouse button to rotate camera");
  ImGui::Checkbox("Draw wireframe (C)", &wireframe);
  ImGui::Checkbox("Simulate (Space)", &simulating);
  ImGui::Checkbox("ImGui Demo", &show_demo_window);
  ImGui::Separator();
  ImGui::InputFloat3("Origin position", glm::value_ptr(origin));
  ImGui::InputFloat3("Cell size", glm::value_ptr(cell));
  ImGui::InputInt3("Grid size", reinterpret_cast<int *>(glm::value_ptr(size)));
  if (ImGui::Button("Restart")) {
    grid = Grid(origin, cell, size, 100.f, .4f, 1.f);
    renderer = std::make_unique<GridRenderer>(grid.Particles(),
                                              grid.ParticleIndices());
  }
  ImGui::Separator();
  ImGui::SliderFloat("Time step", &time_step, .01f / ImGui::GetIO().Framerate,
                     1 / ImGui::GetIO().Framerate, "%.4f");
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
  const auto window = Initialize();

  Axes axes;
  renderer =
      std::make_unique<GridRenderer>(grid.Particles(), grid.ParticleIndices());

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
    renderer->Update(grid.Particles());

    axes.Draw(camera);
    if (wireframe) {
      renderer->DrawTetrahedra(camera);
    } else {
      renderer->DrawSurface(camera);
    }

    RenderUI();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwDestroyWindow(window);
  glfwTerminate();
}