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
void update_uniforms(tetragon::ShaderProgram& shaderProgram);

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

   VertexAttribute::Builder vertexAttribBuilder = VertexAttribute::Builder()
         .set_type(GL_FLOAT)
         .set_size(3);

   const VertexAttribute posAttrib =
      vertexAttribBuilder.set_name("pos").build();
   const VertexAttribute colorAttrib =
      vertexAttribBuilder.set_name("color").build();

   vbo1.add_attribute(posAttrib);
   vbo2.add_attribute(colorAttrib);

   Vertex colorRed   = { 1, 0, 0 };
   Vertex colorGreen = { 0, 1, 0 };

   const VertexBuffer::Usage usage = VertexBuffer::Usage::STATIC;
   vbo1.buffer(triangle, usage);
   vbo1.buffer(triangleBravo, usage);

   vbo2.buffer(Vertex{ 1, 0, 0 }, usage);
   vbo2.buffer(Vertex{ 1, 1, 0 }, usage);
   vbo2.buffer(Vertex{ 1, 1, 1 }, usage);
   vbo2.buffer(Vertex{ 0, 1, 0 }, usage);
   vbo2.buffer(Vertex{ 0, 1, 1 }, usage);
   vbo2.buffer(Vertex{ 1, 1, 1 }, usage);

   const unsigned int verteciesCount = vbo1.size() / sizeof(Vertex);

   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   // postpone_closing(window, 2);
   while (!window.should_close()) {
      glClearColor(.3f, .3f, .5f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);
   
      update_uniforms(shaderProgram);

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

void update_uniforms(tetragon::ShaderProgram& shaderProgram) {
   float time = glfwGetTime();
   float greenValue = fabs(sin(time));

   const char* uniformName = "globalGreen";
   int uniformLocation = glGetUniformLocation(shaderProgram.get_object_id(), uniformName);
   shaderProgram.bind();
   glUniform1f(uniformLocation, greenValue);
}
