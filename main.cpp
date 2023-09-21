#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <rustlib.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApp
{
	public:
		void run()
		{
			initWindow();
			initVulkan();
			mainLoop();
			cleanup();
		}

	private:
		void initWindow()
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // don't create an opengl context
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			window = glfwCreateWindow(WIDTH, HEIGHT, "Goop EDITOR", nullptr, nullptr);
		}

		void initVulkan()
		{
			createInstance();
		}

		void test(int a, int b)
		{
		}

		void createInstance()
		{
			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Hello Triangle";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "goop";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensionNames;
			glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			createInfo.enabledExtensionCount = glfwExtensionCount;
			createInfo.ppEnabledExtensionNames = glfwExtensionNames;
			createInfo.enabledLayerCount = 0;

			if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create instance!");
			}
			else
			{
				std::cout << "Instance created successfullly!\n";
				say_hello();
			}
		}

		void mainLoop()
		{
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
			}
		}

		void cleanup()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		GLFWwindow* window;
		VkInstance instance;
};

int main()
{
	HelloTriangleApp app;

	try
	{
		app.run();
	} catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

