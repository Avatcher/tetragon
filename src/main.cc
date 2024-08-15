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
tetragon::ShaderProgram create_shader_program();

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
      { -.5f, -.25f },
      {  .5f, -.25f },
      {  .0f,  .75f }
   };
   const Triangle triangleBravo{
      { -.5f, -.25f },
      {  .5f, -.25f },
      {  .0f, -.8f }
   };
   const Square square{
      { .5, .5 },
      { -.5, -.5 }
   };

   VertexArray VAO;
   VAO.bind(); 

   VertexBuffer vbo1, vbo2;

   ShaderProgram shaderProgram = create_shader_program();
   shaderProgram.bind();

   vbo1.bind();
   GLuint posLocation = shaderProgram.get_attribute_location("pos");
   glVertexAttribPointer(posLocation, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
   glEnableVertexAttribArray(0);

   vbo2.bind();
   GLuint colorLocation = shaderProgram.get_attribute_location("color");
   glVertexAttribPointer(colorLocation, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
   glEnableVertexAttribArray(1);

   Vertex colorRed   = { 1, 0, 0 };
   Vertex colorGreen = { 0, 1, 0 };

   const VertexBuffer::Usage usage = VertexBuffer::Usage::STATIC;
   vbo1.buffer(triangle, usage);
   vbo1.buffer(triangleBravo, usage);

   for (int i = 0; i < triangle.vertex_count(); i++) {
      vbo2.buffer(colorRed, usage);
   }
   for (int i = 0; i < triangleBravo.vertex_count(); i++) {
      vbo2.buffer(colorGreen, usage);
   }

   const unsigned int verteciesCount = vbo1.size() / sizeof(Vertex);

   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   postpone_closing(window, 2);
   while (!window.should_close()) {
      glClearColor(.3f, .3f, .5f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);
   
      VAO.bind();
      glDrawArrays(GL_TRIANGLES, 0, vbo1.size() / (3 * sizeof(float)));

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

tetragon::ShaderProgram create_shader_program() {
   using namespace tetragon;
   const char* vertexShaderSource = RESOURCE_VERTEX_VERT;
   const char* fragmentShaderSource = RESOURCE_FRAGMENT_FRAG;
   Shader vertexShader(ShaderType::VERTEX, vertexShaderSource);
   Shader fragmentShader(ShaderType::FRAGMENT, fragmentShaderSource);
   return ShaderProgram::Builder()
      .attach_shader(vertexShader)
      .attach_shader(fragmentShader)
      .build();
}
