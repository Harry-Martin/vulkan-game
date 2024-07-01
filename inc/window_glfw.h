#ifndef WINDOW_GLFW_H
#define WINDOW_GLFW_H

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void window_init_window(void);

uint8_t window_should_window_close(void);

void window_poll_events(void);
void window_clean(void);

#endif // WINDOW_GLFW_H
