#define GL_GLEXT_PROTOTYPES

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "graphics.hpp"
#include "initializations.hpp"
#include "resources.hpp"

#define WINDOW_NAME "Tetragon"
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

using spdlog::info, spdlog::debug, spdlog::warn, spdlog::error;

int main() {
   tetragon::init_logs();

   class TetragonWindow : public tetragon::Window {
   public:
      TetragonWindow(): Window(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT) {}

      std::string construct_title() {
         return fmt::format("{} {}x{}", WINDOW_NAME, get_width(), get_height());
      }

      void on_resize(int oldWidth, int oldHeight) override {
         glViewport(0, 0, get_width(), get_height());
         std::string title = construct_title();
         set_title(title.c_str());
      }
   } window;
   window.make_context();

   float vertecies[] = {
      -.5f, -.5f, .0f,
       .5f, -.5f, .0f,
       .0f,  .5f, .0f   
   };

   using Object = unsigned int;

   Object VAO;
   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);

   Object VBO;
   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertecies), vertecies, GL_STATIC_DRAW);

   using namespace tetragon;

   const char* vertexShaderSource = RESOURCE_VERTEX_VERT;
   const char* fragmentShaderSource = RESOURCE_FRAGMENT_FRAG;

   Shader vertexShader(ShaderType::VERTEX, vertexShaderSource);
   Shader fragmentShader(ShaderType::FRAGMENT, fragmentShaderSource);
   ShaderProgram shaderProgram = ShaderProgram::Builder()
      .attach_shader(vertexShader)
      .attach_shader(fragmentShader)
      .build();
   shaderProgram.bind();

   glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
   glEnableVertexAttribArray(0);
   glBindVertexArray(VAO);

   while (!window.should_close()) {
      glClearColor(.3f, .3f, .5f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);
   
      shaderProgram.bind();
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 3);

      window.swap_buffers();
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}
