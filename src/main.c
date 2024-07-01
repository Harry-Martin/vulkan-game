#include "gpu_vulkan.h"

#include "window_glfw.h"

void mainLoop(void);
void cleanUp(void);


int main(void) {
	window_init_window();
	gpu_init_vulkan();
	mainLoop();

	cleanUp();
	return 0;
}


void mainLoop(void) {
	while (!window_should_window_close()) {
		window_poll_events();
	}
}

void cleanUp(void) {
	gpu_clean();
	window_clean();
}



