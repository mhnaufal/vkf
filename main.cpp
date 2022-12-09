#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <cstring>

const uint64_t WIDTH = 800;
const uint64_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#if NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = false;
#endif

bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr); // enumerate all the available layers

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) { // check if the requested validation layers is exists
                std::cout << "[layer name]\t" << layerProperties.layerName << std::endl;
                layerFound = true;
                break;
            } else {
                std::cout << "[DEBUG]] no validation layer named: " << layerProperties.layerName << std::endl;
            }
        }

        if (!layerFound) return false;
    }

    return true;
}

class HelloTriangleApplication
{
    public:
        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:
        GLFWwindow* m_window;
        VkInstance m_instance;

        // create window using GLFW
        void initWindow()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // prevent calling the OpenGL API
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // prevent the window to be resizeable

            this->m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Triangle", nullptr, nullptr);
        }

        // initialize vulkan by creating instance
        void createInstance()
        {
            // a struct that holds information about our application
            VkApplicationInfo app_info{};
            app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            app_info.pApplicationName = "Vulkan Triangle";
            app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            app_info.pEngineName = "No Engine";
            app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            app_info. apiVersion = VK_API_VERSION_1_0;

            // a struct that tells Vulkan driver which global extensions and validation layers to use
            VkInstanceCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // instance type
            create_info.pApplicationInfo = &app_info; // instance app info

            // get the needed extensions info
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            // continue the global extension setup
            create_info.enabledExtensionCount = glfwExtensionCount; // global extensions
            create_info.ppEnabledExtensionNames = glfwExtensions; // global extensions
            // create_info.enabledLayerCount = 0; // global extensions
            if (enableValidationLayers) {
                create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                create_info.ppEnabledLayerNames = validationLayers.data(); // set the validation layers
            } else {
                create_info.enabledLayerCount = 0;
            }

            // create vulkan instance based on the above configuration
            VkResult result = vkCreateInstance(&create_info, nullptr, &m_instance);
            if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to create instance");
            } else {
                std::cout << "instance created successfully" << std::endl;
            }

            /* Retrieve a list of supported extensions */
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr); // request the number of extensions

            std::vector<VkExtensionProperties> extensions(extensionCount); // array to hold the extension details

            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()); // query the extension details

            std::cout << "available extensions" << std::endl;
            for (const auto &extension : extensions) {
                std::cout << '\t' << extension.extensionName << " " << extension.specVersion << std::endl;
            }

            if (enableValidationLayers && !checkValidationLayerSupport()) {
                throw std::runtime_error("requested validation layer(s) is not exists");
            }
        }

        // Hold the Vulkan object
        void initVulkan()
        {
            createInstance();
        }

        // Iterate until the window is closed
        void mainLoop()
        {
            // to keep application running
            while (!glfwWindowShouldClose(this->m_window)) {
                glfwPollEvents();
            }
        }

        // After the mainLoop() end and the window is closed, deallocate all of
        // the resource
        void cleanup()
        {
            vkDestroyInstance(m_instance, nullptr);
            glfwDestroyWindow(this->m_window);
            glfwTerminate();
        }
};

int main()
{
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
