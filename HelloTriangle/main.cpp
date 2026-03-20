#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include<iostream>
#include<stdexcept>
#include<cstdlib>
#include<vector>
#include<cstring>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkallocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr){
        return func(instance, pCreateInfo,pAllocator, pDebugMessenger);
    }
    else{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator ){
    auto func =(PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr){
        func(instance, debugMessenger, pAllocator);
    }
}
class HelloTriangleApplication {
    public:
    const uint32_t WIDTH =800;
    const uint32_t HEIGHT = 600;
    const std::vector<const
    char*> validationLayers ={
        "VK_LAYER_KHRONOS_validation"};
    #ifdef NDEBUG
    const bool enableValidationLayers = false;
    #else 
    const bool enableValidationLayers =true;
    #endif   
    
    void run(){
        initWindow();
        initVulkan();
        mainloop();
        cleanup();
        
    }
    private:
     GLFWwindow*window;
     VkInstance instance;
      VkDebugUtilsMessengerEXT debugMessenger;
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
        setupDebugMessenger();

    }
    void setupDebugMessenger(){
        if(!enableValidationLayers)  return;
       VkDebugUtilsMessengerCreateInfoEXT createInfo{};
       createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
       createInfo.messageSeverity=VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                                   //tells vulkan which severity we want to hear about
     createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                                  //this tells vulkan what type of message we want

      createInfo.pfnUserCallback = debugCallback;
      create.pUserData = nullptr;                            

      if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger)!= VK_SUCCESS){
        throw std::runtime_error("failed to set up debug messenger!");
      }
    }
    void mainloop(){
        while (!glfwWindowShouldClose(window)){
            glfwPollEvents();
        }
    }
    bool checkValidationLayerSupport(){//Validation layer
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount,nullptr);
         std::vector<VkLayerProperties> availableLayers(layerCount);
         vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }   
    }
    return true;
}

std::vector<const char*> getRequiredExtensions(){
    uint32_t glfwExtensionCount =0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData){
        std::cerr <<"validation layer" <<pCallbackData->pMessage <<std::endl;
       
        return VK_FALSE;
    }

    
    void createInstance(){

        if (enableValidationLayers && !checkValidationLayerSupport()){
            throw std::runtime_error("validation layers requested, but not availablr");

        }
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName ="Hello Triangle :)";
        appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.pEngineName ="No Engine";
        appInfo.engineVersion =VK_MAKE_VERSION(1,0,0);
        appInfo.apiVersion =VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        if (enableValidationLayers){
            createInfo.enabledLayerCount =static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

        }
        else{
            createInfo.enabledLayerCount =0;
        }

       auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
       

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount,availableExtensions.data());

        std::cout <<"available extensions : \n";
        for  (const auto& extension : availableExtensions){
            std :: cout <<'\t'<<extension.extensionName <<'\n';
        }

        if (vkCreateInstance(&createInfo,nullptr, &instance)!= VK_SUCCESS)
        {
            throw
            std::runtime_error("failed to create Instance! ");

        }
      
       

    }

    void cleanup(){
        if (enableValidationLayers){
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
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