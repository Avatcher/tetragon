#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <map>
#include <iostream>
#include <thread>
#include <chrono>
#include "graphics.hpp"
#include "initializations.hpp"

namespace tetragon {

void callbacks::window_resize_callback(GLFWwindow* glfwWindow, int width, int height) {
   Window* window = WindowManager::INSTANCE->get_window(glfwWindow);
   if (window == nullptr) return;
   int oldWidth = window->m_width, oldHeight = window->m_height;
   window->m_width = width;
   window->m_height = height;
   window->on_resize(oldWidth, oldHeight);
}

Window::Window(const char* title, int width, int height):
      m_title(title), m_width(width), m_height(height) {
   init_glfw();
   m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
   glfwSetFramebufferSizeCallback(m_window, callbacks::window_resize_callback);
   glfwMakeContextCurrent(m_window);
   init_glad();
   WindowManager::INSTANCE->register_window(this);
}

Window::~Window() {
   WindowManager::INSTANCE->remove_window(this);
   glfwDestroyWindow(m_window);
}

void Window::on_resize(int oldWidth, int oldHeight) {
   glViewport(0, 0, m_width, m_height);
}

void Window::make_context() const {
   glfwMakeContextCurrent(m_window);
}

void Window::swap_buffers() {
   glfwSwapBuffers(m_window);
}

const char* Window::get_title() const {
   return m_title;
}

void Window::set_title(const char* title) {
   m_title = title;
   glfwSetWindowTitle(m_window, title);
}

int Window::get_width() const {
   return m_width;
}

int Window::get_height() const {
   return m_height;
}

void Window::set_size(int width, int height) {
   m_width = width;
   m_height = height;
   glfwSetWindowSize(m_window, width, height);
}

bool Window::should_close() const {
   return glfwWindowShouldClose(m_window);
}

void Window::set_should_close(bool state) {
   glfwSetWindowShouldClose(m_window, state);
}

namespace {
   class DefaultWindowManager : public WindowManager {
      std::map<GLFWwindow*, Window*> m_windows;
   public:
      void register_window(Window* window) override {
         m_windows.insert({get_glfw_window(window), window});
      }

      void remove_window(Window* window) override {
         m_windows.erase(get_glfw_window(window));
      }

      Window* get_window(GLFWwindow* window) const override {
         return m_windows.at(window);
      }
   } windowManager;
}

WindowManager* const WindowManager::INSTANCE = &windowManager;

GLFWwindow* WindowManager::get_glfw_window(Window* window) {
   return window->m_window;
}

namespace {
   int convert_shader_type(ShaderType type) {
      switch (type) {
         case ShaderType::VERTEX: return GL_VERTEX_SHADER;
         case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
      }
      throw std::runtime_error("Unexpected shader type");
   }

   GLuint create_shader(ShaderType type, const char* source) {
      GLuint shader = glCreateShader(convert_shader_type(type));
      glShaderSource(shader, 1, &source, nullptr);
      glCompileShader(shader);
      int success;
      char message[512];
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success) {
         glGetShaderInfoLog(shader,512, nullptr, message);
         spdlog::error("Failed to build a shader: {}", message);
         throw std::runtime_error(message);
      }
      return shader;
   }
}

Shader::Shader(ShaderType type, const char* source):
      m_type(type), m_object(create_shader(type, source)) {
}

Shader::~Shader() {
   glDeleteShader(m_object);
}

ShaderType Shader::get_type() const {
   return m_type;
}

ShaderProgram::ShaderProgram(GLuint program): m_object(program) {
}

ShaderProgram::~ShaderProgram() {
   glDeleteProgram(m_object);
}

void ShaderProgram::bind() {
   glUseProgram(m_object);
}

ShaderProgram::Builder::Builder() {
   m_object = glCreateProgram();
}

ShaderProgram::Builder& ShaderProgram::Builder::attach_shader(Shader const& shader) {
   glAttachShader(m_object, shader.m_object);
   return *this;
}

ShaderProgram ShaderProgram::Builder::build() const {
   glLinkProgram(m_object);
   int success;
   char message[512];
   glGetProgramiv(m_object, GL_LINK_STATUS, &success);
   if (!success) {
      glGetProgramInfoLog(m_object, 512, nullptr, message);
      spdlog::error("Failed to link a shader program: {}", message);
      throw std::runtime_error(message);
   }
   return ShaderProgram(m_object);
}

} // tetragon

