#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include<iostream>
#include<stdexcept>
#include<cstdlib>
#include<vector>


class HelloTriangleApplication {
    public:
    const uint32_t WIDTH =800;
    const uint32_t HEIGHT = 600;
    void run(){
        initWindow();
        initVulkan();
        mainloop();
        cleanup();
        
    }
    private:
     GLFWwindow*window;
     VkInstance instance;
    void initWindow(){
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW!");}
        //integrating GLFW
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
        window=glfwCreateWindow(WIDTH,HEIGHT,"Vulkan",nullptr,nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window!");
        }

    }
    void initVulkan(){
        createInstance();

    }
    void mainloop(){
        while (!glfwWindowShouldClose(window)){
            glfwPollEvents();
        }

    }
    void createInstance(){
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicatiomName ="Hello Triangle :)";
        appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.pEngineName ="No Engine";
        appInfo.engineVersion =VK_MAKE_VERSION(1,0,0);
        appInfo.apiVersion =VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount =0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtension(&glfwExtensionsCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        create.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,nullptr)
        std::vector<VkExtensiomProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount,extensions.data());

        std::cout <<"available extensions : \n";
        for  (const auto& extension : extension){
            std :: cout <<'\t'<<extension.extensionName <<'\n';
        }

        VkResult result = vkCreateInstance(&createeInfo, nullptr,&instance);
        if (vkCreateInstance(&createInfo,nullptr, &instance)!= VK_SUCCESS)
        {
            throw
            std::runtime_errpr("failed to create Instance! ");

        }


    }
    void cleanup(){
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();//terminating glfw

    }
};


int main(){
    HelloTriangleApplication app;
 try {
    app.run();

 }catch (const std::exception& e){
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
 }
 return EXIT_SUCCESS;
}