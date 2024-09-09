#include <map>
#include <utility>
#include <glad/glad.h>

#include "applications.hpp"
#include "initializations.hpp"

namespace tetragon {

void callbacks::window_resize_callback(GLFWwindow* glfwWindow, const int width, const int height) {
	Window* window = WindowManager::INSTANCE->get_window(glfwWindow);
	if (window == nullptr) return;
	const int oldWidth = window->m_width;
	const int oldHeight = window->m_height;
	window->m_width = width;
	window->m_height = height;
	window->on_resize(oldWidth, oldHeight);
}

Window::Window(const char* title, const int width, const int height):
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

void Window::swap_buffers() const {
	glfwSwapBuffers(m_window);
}

const char* Window::title() const {
	return m_title;
}

void Window::set_title(const char* title) {
	m_title = title;
	glfwSetWindowTitle(m_window, title);
}

int Window::width() const {
	return m_width;
}

int Window::height() const {
	return m_height;
}

void Window::set_size(const int width, const int height) {
	m_width = width;
	m_height = height;
	glfwSetWindowSize(m_window, width, height);
}

bool Window::should_close() const {
	return glfwWindowShouldClose(m_window);
}

void Window::set_should_close(const bool state) {
	glfwSetWindowShouldClose(m_window, state);
}

int Window::key(const int key) const {
	return glfwGetKey(m_window, key);
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

GLFWwindow* WindowManager::get_glfw_window(const Window* window) {
	return window->m_window;
}

namespace {
	class MultikeyBindingPredicate {
		const std::vector<Controls::Key> m_keys;

	public:
		MultikeyBindingPredicate(std::initializer_list<Controls::Key> keys):
				m_keys(keys) {}

		bool operator()(const Window& window) const {
			return std::all_of(m_keys.begin(), m_keys.end(),
				[](const Controls::Key key) { return key == GLFW_PRESS; });
		}
	};
}

Controls::Controls(Window& window):
		m_window(window) {}

Controls& Controls::add_binding(Key key, BindingFn const& fn) {
	return add_binding({ key }, fn);
}

Controls& Controls::add_binding(const std::initializer_list<Key> keys, BindingFn const& fn) {
	m_bindings.emplace_back(*this, MultikeyBindingPredicate(keys), fn);
	return *this;
}

Controls& Controls::remove_binding(Binding const& binding) {
	m_bindings.remove(binding);
	return *this;
}

void Controls::process() const {
	std::lock_guard lock(m_processMutex);
	for (Binding const& binding : m_bindings) {
		if (binding.is_triggered()) binding.execute();
	}
}

Window& Controls::window() const {
	return m_window;
}

Controls::Binding::Binding(Controls& controls, BindingPredicate predicate, BindingFn fn):
	m_controls(controls), m_predicate(std::move(predicate)), m_function(std::move(fn)) {}

bool Controls::Binding::is_triggered() const {
	return m_predicate(m_controls.m_window);
}

void Controls::Binding::execute() const {
	m_function(m_controls.m_window);
}

bool Controls::Binding::operator==(Binding const& other) const {
	return this == &other;
}

} // tetragon
