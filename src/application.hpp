#ifndef TETRAGON_APPLICATION_HPP
#define TETRAGON_APPLICATION_HPP

#include <GLFW/glfw3.h>

namespace tetragon {

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

} // tetragon

#endif // TETRAGON_APPLICATION_HPP
