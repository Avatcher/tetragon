#ifndef TETRAGON_APPLICATION_H
#define TETRAGON_APPLICATION_H

#include <GLFW/glfw3.h>
#include <exception>
#include <thread>
#include <mutex>
#include <vector>

namespace tetragon {

using Object = GLuint;

namespace callbacks {
   void window_resize_callback(GLFWwindow* glfwWindow, int width, int height);
} // callbacks

class Window {
private:
   GLFWwindow* m_window;
   const char* m_title;
   int m_width, m_height;

public:
   Window(const char* title, int width, int height);
   Window(Window const&) = delete;
   virtual ~Window();

   virtual void on_resize(int oldWidth, int oldHeight);

   void make_context() const;
   void swap_buffers();

   const char* get_title() const;
   void set_title(const char* title);

   int get_width() const;
   int get_height() const;
   void set_size(int width, int height);

   bool should_close() const;
   void set_should_close(bool state);

   friend void callbacks::window_resize_callback(GLFWwindow* glfwWindow, int width, int heigh);
   friend class WindowManager;
};

class WindowManager {
protected:
   GLFWwindow* get_glfw_window(Window* window);
public:
   static WindowManager* const INSTANCE;

   virtual void register_window(Window* window) = 0;
   virtual void remove_window(Window* window) = 0;

   virtual Window* get_window(GLFWwindow* window) const = 0;
};

enum class ShaderType {
   VERTEX, FRAGMENT   
};

class Shader {
   const ShaderType m_type;
   const GLuint m_object;
public:
   Shader(ShaderType type, const char* source);
   Shader(Shader const&) = delete;
   virtual ~Shader();

   ShaderType get_type() const;

   friend class ShaderProgram;
};

class ShaderProgram {
   const GLuint m_object;

   ShaderProgram(GLuint program);
public:
   ShaderProgram(ShaderProgram const&) = delete;
   ~ShaderProgram();

   void bind();

   class Builder {
      GLuint m_object;
   public:
      Builder();

      ShaderProgram::Builder& attach_shader(Shader const& shader);
      ShaderProgram build() const;
   };
};

class VertexAttribute {

};

class VertexBuffer {
   const Object m_object;
public:
   enum class Usage : GLenum {
      STREAM = GL_STREAM_DRAW,
      STATIC = GL_STATIC_DRAW,
      DYNAMIC = GL_DYNAMIC_DRAW
   };

   VertexBuffer();
   virtual ~VertexBuffer();

   void bind();
   void buffer(const void* ptr, unsigned long size);
   void buffer(const void* ptr, unsigned long size, Usage usage);
};

class VertexArray {
   const Object m_object;
public:
   VertexArray();
   virtual ~VertexArray();

   void bind();
};

} // tetragon

#endif // TETRAGON_APPLICATION_H
