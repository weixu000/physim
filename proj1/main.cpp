#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Axes.hpp"
#include "Camera.hpp"
#include "Grid.hpp"
#include "GridRenderer.hpp"

namespace {
Camera camera({-2, 1, -2}, {0, 0, 0}, 640, 480);

auto translation = glm::vec3(.5f, 2.f, .5f);
auto yaw_pitch_roll = glm::vec3(0.f, 0.f, 0.f);
auto cell = glm::vec3(.5f, .5f, .5f);
auto size = glm::uvec3(4, 4, 4);
auto E = 100.f, nu = .4f, eta = 1.f;

Grid grid(translation, yaw_pitch_roll, cell, size, E, nu, eta);
auto time_step = 1E-3f;

GridRenderer renderer;

void Restart() {
  grid = Grid(translation, yaw_pitch_roll, cell, size, E, nu, eta);
  renderer = GridRenderer(grid.Particles(), grid.ParticleIndices());
}

auto wireframe = false;
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

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (ImGui::GetIO().WantCaptureKeyboard) {
    camera.forward_ = camera.left_ = camera.backward_ = camera.right_ = false;
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
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    Restart();
  }
  if (key == GLFW_KEY_ENTER && action == GLFW_REPEAT) {
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

  static auto show_demo_window = false;
  if (show_demo_window) {
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  ImGui::Begin("Control", nullptr);
  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
  if (grid.GetError()) {
    ImGui::Text("Grid blown up");
  }
  ImGui::Separator();
  ImGui::Text("W, A, S, D to move camera");
  ImGui::Text("Hold left mouse button to rotate camera");
  ImGui::Checkbox("ImGui Demo", &show_demo_window);
  ImGui::Checkbox("Draw wireframe (C)", &wireframe);
  ImGui::Checkbox("Simulate (Space)", &simulating);
  if (ImGui::Button("Step (Enter)")) {
    grid.Update(time_step);
  }
  ImGui::SameLine();
  if (ImGui::Button("Restart (R)")) {
    Restart();
  }
  ImGui::Separator();
  if (ImGui::SliderFloat3("Origin translation", glm::value_ptr(translation),
                          0.f, 10.f) |
      ImGui::SliderFloat3("Origin rotation", glm::value_ptr(yaw_pitch_roll),
                          -180.f, 180.f) |
      ImGui::SliderFloat3("Cell size", glm::value_ptr(cell), .1f, 1.f) |
      ImGui::SliderInt3("Grid size",
                        reinterpret_cast<int *>(glm::value_ptr(size)), 1, 10)) {
    Restart();
  }
  ImGui::Separator();
  ImGui::SliderFloat("Time step", &time_step, .0001f,
                     1 / ImGui::GetIO().Framerate, "%.4f");
  if (ImGui::SliderFloat("Young's modulus", &E, 1.f, 2000.f) |
      ImGui::SliderFloat("Poisson's ratio", &nu, -.9f, .49f)) {
    grid.SetElasticParams(E, nu);
  }
  if (ImGui::SliderFloat("Viscosity", &eta, 0.f, 150.f)) {
    grid.SetDamping(eta);
  }
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace

int main() {
  const auto window = Initialize();

  Axes axes;
  renderer = GridRenderer(grid.Particles(), grid.ParticleIndices());

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

    RenderUI();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwDestroyWindow(window);
  glfwTerminate();
}