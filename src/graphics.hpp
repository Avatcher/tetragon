#ifndef TETRAGON_APPLICATION_H
#define TETRAGON_APPLICATION_H

#include <GLFW/glfw3.h>
#include <exception>
#include <thread>
#include <mutex>
#include <vector>
#include <type_traits>

namespace tetragon {

using Object = GLuint;

namespace callbacks {
   void window_resize_callback(GLFWwindow* glfwWindow, int width, int height);
} // callbacks

struct Textable {
   virtual std::string to_string() const = 0;
};

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
   using byte = char;
   
   const Object m_object;
   byte* m_buffer;
   byte* m_ptr;
   unsigned int m_size;
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
   
   template<class T>
   void buffer(T const& data) {
      buffer(data, Usage::DYNAMIC);
   }

   template<class T>
   void buffer(T const& data, Usage usage) {
      data.buffer_to(*this, usage);
   }

   unsigned long size() const;
};

class VertexArray {
   const Object m_object;
public:
   VertexArray();
   virtual ~VertexArray();

   void bind();
};

class Vertex : public Textable {
public:
   float x, y, z;

   Vertex();
   Vertex(std::initializer_list<float> values);

   void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const;
   
   std::string to_string() const override;
};

class Shape {
public:
   virtual const Vertex* vertecies() const = 0;
   virtual unsigned int vertex_count() const = 0;
   virtual void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const = 0;
};

class Triangle : public Shape, public Textable {
public:
   Vertex a, b, c;

   Triangle();
   Triangle(Vertex, Vertex, Vertex);

   const Vertex* vertecies() const override;
   unsigned int vertex_count() const override;
   void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const override;
   
   std::string to_string() const override;
};

class Square : public Shape, public Textable {
   Triangle a, b;
public:
   Square(Vertex, Vertex);

   const Vertex* vertecies() const override;
   unsigned int vertex_count() const override;
   void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const override;

   std::string to_string() const override;
};

} // tetragon

template<class T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of_v<tetragon::Textable, T>, char>>
      : fmt::formatter<std::string> {
	auto format(T const& value, fmt::format_context& ctx) const {
      return formatter<std::string>::format(value.to_string(), ctx);
	}
};

#endif // TETRAGON_APPLICATION_H
