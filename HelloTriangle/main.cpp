#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include<iostream>
#include<stdexcept>
#include<cstdlib>
#include<vector>
#include<cstring>
#include<map>
#include<optional>
#include<set>
#include<string>
#include<algorithm>
#include<limits>
#include<cstdint>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr){
        return func(instance, pCreateInfo,pAllocator, pDebugMessenger);
    }
    else{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator ){
    auto func =(PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
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

    const std::vector<const char*> deviceExtensions ={
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };    
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
      VkSurfaceKHR surface;
      VkDebugUtilsMessengerEXT debugMessenger;
      VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
      VkDevice device;
      VkQueue graphicsQueue;
      VkQueue presentQueue;
      VkSwapchainKHR swapChain;
      std::vector<VkImage> swapChainImages;
      VkFormat swapChainImageFormat;
      VkExtent2D swapChainExtent;
     struct QueueFamilyIndices{
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete(){
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
     };
     struct SwapChainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
     };
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
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();

    }
   /* void pickPhysicalDevice(){
        uint32_t deviceCount = 0;
        std::multimap<int, VkPhysicalDevice> candidates;
        for(const auto& device : devices){
            int score = rateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }
        if ( candidates.rbegin()->first > 0){
            physicalDevice = candidates.rbegin()->second;
        }
        else{
            throw std::runtime_error("failed to find a suitable GPU!");
        }
       vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount ==0){
            throw std::runtime_error("failed to find GPUs with Vulkan support");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
          for (const auto& device : devices){
            if (isDeviceSuitable(device)){
                physicalDevice = device;
                break;
            }
        }
        if (physicalDevice == VK_NULL_HANDLE){
            throw std::runtime_error("failed to find any suitable Gpu!");
        }
    }
   int rateDeviceSuitability(VkPhysicalDevice device){
         int score = 0;
          if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
               score+= 1000;//because discrete gpu have performance advantage
        }
      score += deviceProperties.limits.maxImageDimensions2D;//maximum possible size of textures affects graphics quality
       if (!deviceFeatures.geometryShader){
           return 0;
       }
            return score;
    } */  //This is a robust way to pick a GPU using a scoring system. It checks 
                            // device properties and favors dedicated graphics cards (Discrete GPUs) 
                            // over integrated graphics, while also checking maximum texture sizes.
    
    /*bool isDeviceSuitable(VkPhysicalDevice device){
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
      
        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               deviceFeatures.geometryShader;

    }*/
    void createSurface(){
        if (glfwCreateWindowSurface(instance, window, nullptr,&surface)!= VK_SUCCESS){
            throw std::runtime_error("failed to create a window surfce");
        }


   } void pickPhysicalDevice(){
    uint32_t deviceCount =0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount ==0){
        throw std::runtime_error("failed to find gpus with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    for( const auto& device : devices){
        if (isDeviceSuitable(device)){
            physicalDevice = device;
            break;
        }
    }
    if (physicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("failed to find a suitable GPU!");
    }
 }

 QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,queueFamilies.data());

    int i = 0;
    for(const auto& queueFamily : queueFamilies){
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily =i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&presentSupport);
        
        if (presentSupport){
            indices.presentFamily = i;
        }

        if (indices.isComplete()){
            break;
        }
        i++;
    }
    return indices;
 }

 bool checkDeviceExtensionSupport(VkPhysicalDevice device){
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),deviceExtensions.end());
    //Create a set of extensions we require

    for (const auto& extension : availableExtensions){
        requiredExtensions.erase(extension.extensionName);
     }   //Tick them off the list as we find them

     return requiredExtensions.empty();//If the set is empty we found all of them
 }
 SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device){
    SwapChainSupportDetails details;
     uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);// get the supported surface formats

    if (formatCount != 0){
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,details.formats.data());

    }
    uint32_t presentModeCount;//get supported presentation modes
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount !=0){
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        
    }
    return details;
 }
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats){
    for (const auto& availableFormat : availableFormats){

        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            return availableFormat;
        }
    }
        return availableFormats[0];//if we can't find our favourite , just settle for the first one 
    
  }
  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes){
    for (const auto& availablePresentMode: availablePresentModes){
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR){
            return availablePresentMode;
        }
    }
        return VK_PRESENT_MODE_FIFO_KHR;//if mailbox isn't available(this is standard V-sync)
}
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities){
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
        return capabilities.currentExtent;// the gpu requires us to use exactly this resolution
    }
    else{
        int width, height;// the Gpu says pick whatever you want within these min/max limits
        glfwGetFramebufferSize(window,&width,&height);

        VkExtent2D actualExtent ={
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        actualExtent.width =std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
  }
 bool isDeviceSuitable(VkPhysicalDevice device){
        QueueFamilyIndices indices = findQueueFamilies(device);;
         bool extensionsSupported = checkDeviceExtensionSupport(device);
         bool swapChainAdequate = false;
     if(extensionsSupported){// only query the swap chain if extensions exists
       SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
       swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

     }  

    
    return indices.isComplete() && extensionsSupported && swapChainAdequate; // this because right now any gpu is fine
    //the tutorial was teaching the ways for gpu selection for AAA games which I commented out
 }

 void createLogicalDevice(){
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
   /*VkDeviceQueueCreateInfo queueCreateInfo{};//Describe the number of queues we want
   queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
   queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
   queueCreateInfo.queueCount =1;*/

   std::set<uint32_t> uniqueQueueFamilies ={indices.graphicsFamily.value(),indices.presentFamily.value()};
   std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

   float queuePriority =1.0f;//assign priorities to the queues
    for (uint32_t queueFamily : uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount =1;
        queueCreateInfo.pQueuePriorities =&queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }
   

   VkPhysicalDeviceFeatures deviceFeatures{};
   VkDeviceCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
   //createInfo.pQueueCreateInfos = &queueCreateInfo;
   //createInfo.queueCreateInfoCount =1;
   createInfo.pEnabledFeatures = &deviceFeatures;
   createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
   createInfo.pQueueCreateInfos = queueCreateInfos.data();
  
   if (enableValidationLayers){
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
   }
   else{
    createInfo.enabledExtensionCount =0;
   }
 createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
 createInfo.ppEnabledExtensionNames = deviceExtensions.data();
   if (vkCreateDevice(physicalDevice,&createInfo, nullptr,&device)!=VK_SUCCESS){
    throw std:: runtime_error("failed to create logical device!");
   }
   vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
   vkGetDeviceQueue(device,indices.presentFamily.value(),0,&presentQueue);
}

void createSwapChain(){
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount =swapChainSupport.capabilities.minImageCount +1;// how many images should we keep in swap chain
    if (swapChainSupport.capabilities.maxImageCount>0 && imageCount > swapChainSupport.capabilities.maxImageCount){
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers =1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//drawing directly to these images

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t QueueFamilyIndices[] = {indices.graphicsFamily.value(),indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily){    //if both are different queues ,we need to share images between them
        createInfo.imageSharingMode= VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount =2;
        createInfo.pQueueFamilyIndices = QueueFamilyIndices;
   
    }
    else{
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;//dont rotate the screen
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //ignore the alpha channels(no transparent)
    createInfo.presentMode = presentMode;
    createInfo.clipped =VK_TRUE; //Clip pixels that are hidden behind other windows
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain)!=VK_SUCCESS){
        throw std::runtime_error("Failed to create Swap Chain");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);//retieve the handles to the actual images craeted for us by vulkam
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device,swapChain, &imageCount, swapChainImages.data());
 
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

}
    void setupDebugMessenger(){
     if(!enableValidationLayers)  return;
       VkDebugUtilsMessengerCreateInfoEXT createInfo;
       populateDebugMessengerCreateInfo(createInfo);//this does all the work of the previous code
      
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
 void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
    createInfo ={};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT|
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT|
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT|
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT|
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback=debugCallback;
    createInfo.pUserData = nullptr;                                                        
 }
    
 /*uint32_t findQueueFamilies(VkPhysicialDevice device){
 //Logic to find graphics queue family
 }*/ //was just a placeholder


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

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers){
            createInfo.enabledLayerCount =static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
             populateDebugMessengerCreateInfo(debugCreateInfo);
             createInfo.pNext =(VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        }
        else{
            createInfo.enabledLayerCount =0;
            createInfo.pNext = nullptr;
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

        vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);
        if (enableValidationLayers){
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroySurfaceKHR(instance, surface, nullptr);    
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