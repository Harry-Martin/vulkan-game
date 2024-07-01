#ifndef GAME_VULKAN_H
#define GAME_VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define ENABLE_VALIDATION_LAYERS 1

struct vulkanExtensions_s {
	const char *names[64];
	uint32_t count;
};

void gpu_init_vulkan(void);
void gpu_clean(void);


#endif // GAME_VULKAN_H
