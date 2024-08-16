#include <map>
#include "application.hpp"
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

} // tetragon
