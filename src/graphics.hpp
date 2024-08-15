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

class VertexAttribute {
   const char* const m_name;
   const uint m_size;
   const GLenum m_type;
   const bool m_normalized;
   const uint m_stride;

public:
   VertexAttribute(const char* name, uint size, GLenum type,
         bool normalized, uint stride);

   const char* name() const;
   uint size() const;
   GLenum type() const;
   bool normalized() const;
   uint stride() const;

   class Builder {
      const char* m_name;
      uint m_size;
      GLenum m_type;
      bool m_normalized;
      uint m_stride;
   public:
      Builder& set_name(const char* name);
      Builder& set_size(uint size);
      Builder& set_type(GLenum type);
      Builder& set_normalized(bool normalized);
      Builder& set_stride(uint stride);

      VertexAttribute build();
   };
};

class ShaderProgram {
   static ShaderProgram* boundInstance;

   const GLuint m_object;

   ShaderProgram(GLuint program);
public:
   ShaderProgram(ShaderProgram const&) = delete;
   ~ShaderProgram();

   static ShaderProgram* get_bound_instance();

   void bind();
   bool is_bound() const;
   uint get_attribute_location(VertexAttribute const& attribute) const;

   class Builder {
      GLuint m_object;
   public:
      Builder();

      ShaderProgram::Builder& attach_shader(Shader const& shader);
      ShaderProgram build() const;
   };
};

class VertexBuffer {
   using byte = char;
   
   const Object m_object;
   byte* m_buffer;
   byte* m_ptr;
   uint m_size;
   uint m_maxSize;

public:
   enum class Usage : GLenum {
      STREAM = GL_STREAM_DRAW,
      STATIC = GL_STATIC_DRAW,
      DYNAMIC = GL_DYNAMIC_DRAW
   };

   VertexBuffer();
   virtual ~VertexBuffer();

   void bind();
   void add_attribute(VertexAttribute const& attribute);

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

private:
   void ensure_capability(uint additionalSize, Usage usage);
};

class VertexArray {
   const Object m_object;
public:
   VertexArray();
   virtual ~VertexArray();

   void bind();
};

class Vertex {
public:
   float x, y, z;

   Vertex();
   Vertex(std::initializer_list<float> values);

   void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const;
   
   // std::string to_string() const override;
};

class Shape {
public:
   virtual const Vertex* vertecies() const = 0;
   virtual unsigned int vertex_count() const = 0;
   virtual void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const = 0;
};

class Triangle : public Shape {
public:
   Vertex a, b, c;

   Triangle();
   Triangle(Vertex, Vertex, Vertex);

   const Vertex* vertecies() const override;
   unsigned int vertex_count() const override;
   void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const override;
};

class Square : public Shape {
   Triangle a, b;
public:
   Square(Vertex, Vertex);

   const Vertex* vertecies() const override;
   unsigned int vertex_count() const override;
   void buffer_to(VertexBuffer& buffer, VertexBuffer::Usage usage) const override;
};

} // tetragon

#endif // TETRAGON_APPLICATION_H
