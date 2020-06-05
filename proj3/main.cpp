#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Axes.hpp"
#include "Camera.hpp"
#include "Collision.hpp"
#include "RigidBody.hpp"
#include "RigidBodyRenderer.hpp"

using namespace glm;

namespace {
Camera camera({-2, 1, -2}, {0, 0, 0}, 640, 480);

auto center = vec3{0, 5, 0};
auto yaw_pitch_roll = glm::vec3(0.f, 0.f, 0.f);
auto size = vec3{1.f};
auto L = vec3{0.f};

const auto time_step = 1E-5f;

RigidBody rb;
Collision collision;
RigidBodyRenderer renderer;

void Restart() {
  rb = RigidBody(
      center,
      yawPitchRoll(radians(yaw_pitch_roll.x), radians(yaw_pitch_roll.y),
                   radians(yaw_pitch_roll.z)),
      L, size, 1.f);
  renderer = RigidBodyRenderer(size);
}

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
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    Restart();
  }
  if (key == GLFW_KEY_ENTER && action == GLFW_REPEAT) {
    rb.Update(time_step);
  }
}

GLFWwindow *Initialize() {
  // GLFW setup
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  // GLFW window
  const auto window =
      glfwCreateWindow(640, 480, "Rigid Body", nullptr, nullptr);
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

  ImGui::Begin("Control", nullptr);
  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
  ImGui::Separator();
  ImGui::Text("W, A, S, D to move camera");
  ImGui::Text("Hold left mouse button to rotate camera");
  ImGui::Checkbox("Simulate (Space)", &simulating);
  if (ImGui::Button("Step (Enter)")) {
    rb.Update(time_step);
  }
  ImGui::SameLine();
  if (ImGui::Button("Restart (R)")) {
    Restart();
  }
  ImGui::Separator();
  if (ImGui::SliderFloat3("Size", glm::value_ptr(size), .1f, 5.f) |
      ImGui::SliderFloat3("original translation", glm::value_ptr(center), 0.f,
                          20.f) |
      ImGui::SliderFloat3("original rotation", glm::value_ptr(yaw_pitch_roll),
                          -180.f, 180.f) |
      ImGui::SliderFloat3("original angular momentum", glm::value_ptr(L), -5.f,
                          5.f)) {
    Restart();
  }
  ImGui::Separator();
  ImGui::SliderFloat("Restitution", &collision.eps_, 0.f, 1.f);
  ImGui::SliderFloat("Friction", &collision.mu_, 0.f, 1.5f);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace

int main() {
  const auto window = Initialize();

  Axes axes;
  Restart();

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
      rb.AddForce({0, -9.8f, 0}, {0, 0, 0});
      collision.Compute(rb);
      rb.Update(time_step);
    }
    renderer.Update(rb.GetTransform());

    axes.Draw(camera);
    renderer.Draw(camera);

    RenderUI();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwDestroyWindow(window);
  glfwTerminate();
}