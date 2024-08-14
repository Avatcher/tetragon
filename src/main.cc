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

void postpone_closing(tetragon::Window& window, int seconds);

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

   using namespace tetragon;

   const Triangle triangle{
      { -.5f, -.5f },
      {  .5f, -.5f },
      {  .0f,  .75f }
   };
   const Triangle triangleBravo{
      { -.5f, -.6f },
      {  .5f, -.6f },
      {  .0f, -.8f }
   };
   const Square square{
      { .5, .5 },
      { -.5, -.5 }
   };

   VertexArray VAO;
   VAO.bind(); 

   VertexBuffer VBO;
   VBO.bind();
   const VertexBuffer::Usage usage = VertexBuffer::Usage::STATIC;

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

   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   postpone_closing(window, 2);

   const Shape* shape = static_cast<const Shape*>(&square);

   VBO.buffer(*shape, usage);
   spdlog::warn("Shape vertecies: {}", shape->vertex_count());
   spdlog::warn("Expected buffer size: {}", shape->vertex_count() * 3 * sizeof(float));
   spdlog::warn("Actual buffer size: {}", VBO.size());

   while (!window.should_close()) {
      glClearColor(.3f, .3f, .5f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);
   
      shaderProgram.bind();
      VAO.bind();
      
      glDrawArrays(GL_TRIANGLES, 0, shape->vertex_count());

      window.swap_buffers();
      glfwPollEvents();
   }

   glfwTerminate();
   return 0;
}

void postpone_closing(tetragon::Window& window, int seconds) {
   std::thread t([&window, seconds]() {
      spdlog::info("Postponing closing for {} seconds", seconds);
      std::this_thread::sleep_for(std::chrono::seconds(seconds));
      spdlog::info("Closing the application...");
      window.set_should_close(true);
   });
   t.detach();
}
