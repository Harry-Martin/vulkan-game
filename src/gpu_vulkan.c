
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gpu_vulkan.h"

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(*x))


//TODO: cleanup vulkan debug layer
//TODO: get vulkan debug messages working for vkCreateInstance and vkDestroyInstance
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

static void create_instance(void);
static void setup_debug_messenger(void);
static uint8_t check_extension_support(struct vulkanExtensions_s *extensions);
static uint8_t check_validation_layer_support(void);

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;


const char *validationLayers[] = {
	"VK_LAYER_KHRONOS_validation",
};

struct vulkanExtensions_s extensions;


void gpu_init_vulkan(void) {
	create_instance();
	setup_debug_messenger();
}


void gpu_clean(void) {
	if (ENABLE_VALIDATION_LAYERS) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
	}

	vkDestroyInstance(instance, NULL);
}


static void create_instance(void) {
	// setup app info structure
	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Game",
		.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = VK_API_VERSION_1_0,
	};

	// get required extensions from GLFW
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&extensions.count);

	// copy the extensions from GLFW 
	memcpy(extensions.names, glfwExtensions, extensions.count * sizeof(extensions.names[0]));

	// setup instance create info struct
	// with extensions that we require
	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.ppEnabledExtensionNames = extensions.names,
	};

	// if we have enabled validation layers then check for validation support
	if (ENABLE_VALIDATION_LAYERS) {
		if (!check_validation_layer_support()) {
			printf("Validation Layer Support check failed\n");
			exit(1);
		}

		// add the VK_EXT_debug_utils extension
		extensions.names[extensions.count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
		++extensions.count;
		
		createInfo.enabledLayerCount = ARRAY_COUNT(validationLayers);
		createInfo.ppEnabledLayerNames = validationLayers;
	} else {
		createInfo.enabledLayerCount = 0;
	}
	createInfo.enabledExtensionCount = extensions.count;

	printf("Required Instance Extensions:\n");
	for (size_t i = 0; i < extensions.count; ++i) {
		printf("[%ld] %s\n", i, extensions.names[i]);
	}
	printf("\n");


	if (!check_extension_support(&extensions)) {
		printf("Extension support check failed\n");
		exit(1);
	}

	// create instance with the create info struct
	if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS) {
		printf("failed to create instance!");
		exit(1);
	}

}

static uint8_t check_extension_support(struct vulkanExtensions_s *extensions) {
	uint32_t extensionPropCount;
	VkExtensionProperties extensionProperties[1024];

	// get the available extension count 
	if (vkEnumerateInstanceExtensionProperties(NULL, &extensionPropCount, NULL) !=
			VK_SUCCESS) {
		printf("Enumerate Instance Extension Property count failed \n");
	}

	// then get the actual extensions
	if (vkEnumerateInstanceExtensionProperties(
				NULL, &extensionPropCount, extensionProperties) != VK_SUCCESS) {
		printf("Enumerate Instance Extension Property count failed \n");
	}
	printf("Available Instance Extensions:\n");
	for (size_t i = 0; i < extensionPropCount; ++i) {
		printf("[%ld] %s\n", i, extensionProperties[i].extensionName);
	}
	printf("\n");

	// check that all of our extensions are available
	uint8_t found = 0;
	for (size_t i = 0; i < extensions->count; ++i) {
		for (size_t j = 0; j < extensionPropCount; ++j) {
			if (!strcmp(extensionProperties[j].extensionName, extensions->names[i])) {
				found = 1;
				break;
			}
			found = 0;
		}
		if (!found)
			break;
	}

	return found;
}

// returns 1 if all 
static uint8_t check_validation_layer_support(void) {

	printf("Required Layers:\n");
	for (size_t i = 0; i < ARRAY_COUNT(validationLayers); ++i) {
		printf("[%ld] %s\n", i, validationLayers[i]);
	}
	printf("\n");

	// first get validation layer count
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, NULL);

	// then get the available validation layers
	VkLayerProperties availableLayers[1024];
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

	printf("Available Layers:\n");
	for (size_t i = 0; i < layerCount; ++i) {
		printf("[%ld] %s\n", i, availableLayers[i].layerName);
	}
	printf("\n");

	// check that all of our validation layers exist in the 
	uint8_t found = 0;
	for (size_t i = 0; i < ARRAY_COUNT(validationLayers); ++i) {
		for (size_t j = 0; j < layerCount; ++j) {
			if (!strcmp(availableLayers[j].layerName, validationLayers[i])) {
				found = 1;
				break;
			}
			found = 0;
		}
		if (!found)
			break;
	}

	return found;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    printf("validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != NULL) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

static void setup_debug_messenger(void) {

	if (!ENABLE_VALIDATION_LAYERS) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = debug_callback,
		.pUserData = NULL, // Optional
	};

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
		printf("failed to set up debug messenger!\n");
		exit(1);
	}
}
