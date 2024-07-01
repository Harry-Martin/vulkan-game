#include "window_glfw.h"

GLFWwindow *window;

void window_init_window(void) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game", NULL, NULL);
}

uint8_t window_should_window_close(void) {
	return glfwWindowShouldClose(window);
}

void window_poll_events(void) {
	glfwPollEvents();
}

void window_clean(void) {
	glfwDestroyWindow(window);
	glfwTerminate();
}
