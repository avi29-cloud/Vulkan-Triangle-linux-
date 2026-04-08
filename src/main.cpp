#define GLFW_INCLUDE_VULKAN
#include <glm/glm.hpp>
#include<array>
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
#include<fstream>

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
struct Vertex{
    glm::vec2 pos;
    glm::vec3 color;
    //tells vulkan how much memory to step forward for each new vertex
    static VkVertexInputBindingDescription getBindingDescription(){
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
  //tells Vulkan how to extract the position and color from the memory chunk
  static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription(){
    std::array<VkVertexInputAttributeDescription,2> attributeDescriptions{};

    //Attribute 0 : Position (Matches 'layout(location = 0)' in the vertex shader)
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT ;// a 2D float vector
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    //Attribute 1: Color(Matches 'layout(location = 1)' in the vertex shader)
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT ;// a 3D float vector
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
  }
    
};

const std::vector<Vertex> vertices ={
    {{0.0f, -0.5f},{0.0f,1.0f, 1.0f}},//top corner
    {{0.5f,0.5f},{0.0f,1.0f,0.3f}},//bottom right
    {{-0.5f,0.5f},{0.0f,0.2f,1.0f}}//bottom left
};
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
      std::vector<VkImageView> swapChainImageViews;
      VkPipelineLayout pipelineLayout; 
      VkRenderPass renderPass;
      VkPipeline graphicsPipeline;
      std::vector<VkFramebuffer> swapChainFramebuffers;
      VkCommandPool commandPool; 
      VkCommandBuffer commandBuffer;
      VkSemaphore imageAvailableSemaphore;
      VkSemaphore renderFinishedSemaphore;
      VkFence inFlightFence;
      VkBuffer vertexBuffer;
      VkDeviceMemory vertexBufferMemory;

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
     static std::vector<char> readFile(const std::string& filename){
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        //binary means read it as raw data , not as text
        //ate means start at the End of the file (so we know its size immediately)
        if (!file.is_open()){
            throw std::runtime_error("failed to open file");
        }
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);// Jump back to the beginning of the file and read it all at once
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;


     }

     VkShaderModule createShaderModule(const std::vector<char>& code){
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();

        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;

        if (vkCreateShaderModule(device, &createInfo,nullptr, &shaderModule) != VK_SUCCESS){
            throw std::runtime_error("failed to create shader Module");

        }
        return shaderModule;
     }
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
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createCommandBuffer();
        createSyncObjects();

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

void createImageViews(){
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i =0; i<swapChainImages.size();i++){
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;//tell Vulkan its a standard 2Dtexture and use our chosen color format
        createInfo.format = swapChainImageFormat;

        createInfo.components.r =VK_COMPONENT_SWIZZLE_IDENTITY;//Dont mix up the colors
        createInfo.components.g =VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b =VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a =VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask= VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel=0;
        createInfo.subresourceRange.levelCount=1;
        createInfo.subresourceRange.baseArrayLayer=0;
        createInfo.subresourceRange.layerCount =1;

        if (vkCreateImageView(device, &createInfo ,nullptr, &swapChainImageViews[i]) != VK_SUCCESS){
            std::runtime_error("failed to create image views");
        }
    }
}
void createRenderPass(){
        VkAttachmentDescription colorAttachment{};//whiteboard manager
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //wipe it clean before drawing
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//save it in memory
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp =VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        //The subpass reference( tells the pipeline where to find the whigteboard)
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0; //the index of the attachment array
        colorAttachmentRef.layout =VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // The Subpass (A render pass can have multiple steps; we just need one)
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint =VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount =1;
        subpass.pColorAttachments = &colorAttachmentRef;

        //create actual render pass object
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount =1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass)!= VK_SUCCESS){
            throw std::runtime_error("failed to create render pass!");
        }
    }

void createGraphicsPipeline(){

    auto vertshaderCode = readFile("vert.spv");
    auto fragshaderCode = readFile("frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertshaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragshaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};// configure the vertex shader stage
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage= VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module =vertShaderModule;
    vertShaderStageInfo.pName ="main";// tell Vulkan which function to run inside the shader

   VkPipelineShaderStageCreateInfo fragShaderStageInfo{};// configure the fragment shader stage
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage= VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module =fragShaderModule;
    fragShaderStageInfo.pName ="main";

    VkPipelineShaderStageCreateInfo shaderStages[] ={vertShaderStageInfo,fragShaderStageInfo};
    
    std::vector<VkDynamicState> dynamicStates = {   //Dynamic state
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    //Vertex input 
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount =1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    //Input assembly (tell vulkan we are drawing triangles)
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    //ViewPort and Scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width =(float) swapChainExtent.width;
    viewport.height =(float) swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset ={0,0};
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount =1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount =1;
    viewportState.pScissors = &scissor;

    //Rasterizer(turns geometry into physical pixels)
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType =VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable= VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //draw solid triangles
    rasterizer.lineWidth =1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //hides the back of the triangle
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    //Multisampling (Anti-Aliasing - smoothing out jagged edges)
    // we will disable this for now

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    //Color blending 
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE; //no transparency yet

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType=VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount =1;
    colorBlending.pAttachments = &colorBlendAttachment;

   //Pipeline Layout (for passing global variable to the shaders later)
   VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
   pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   pipelineLayoutInfo.setLayoutCount = 0;
   pipelineLayoutInfo.pushConstantRangeCount = 0;
   pipelineLayoutInfo.pPushConstantRanges = nullptr;


   if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr , &pipelineLayout)!= VK_SUCCESS){
    throw std::runtime_error("failed to create pipeline layout!");
   }

  VkGraphicsPipelineCreateInfo  pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

  //Programmable Shaders
  pipelineInfo.stageCount =2;
  pipelineInfo.pStages = shaderStages;

  // fixed function States
 pipelineInfo.pVertexInputState = &vertexInputInfo;
 pipelineInfo.pInputAssemblyState =&inputAssembly;
 pipelineInfo.pViewportState = &viewportState;
 pipelineInfo.pRasterizationState = &rasterizer;
 pipelineInfo.pMultisampleState =&multisampling;
 pipelineInfo.pDepthStencilState = nullptr;
 pipelineInfo.pColorBlendState = &colorBlending; 
 pipelineInfo.pDynamicState = &dynamicState;

 //pipeline layout
 pipelineInfo.layout = pipelineLayout;

// render pass
pipelineInfo.renderPass = renderPass;
pipelineInfo.subpass =0; //the index of subpass we created

//optimization
pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
pipelineInfo.basePipelineIndex =-1;

if (vkCreateGraphicsPipelines(device ,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&graphicsPipeline)!= VK_SUCCESS){
    throw std::runtime_error("failed to create graphics pipeline!");
}
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);

}

void createFramebuffers(){
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i< swapChainImageViews.size();i++){
        VkImageView attachments[]={
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType =VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass; // tell it which manager to use
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments; //Plug in the image view
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device,&framebufferInfo, nullptr,&swapChainFramebuffers[i])!=VK_SUCCESS){
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice , &memProperties);

    for (uint32_t i = 0 ; i <memProperties.memoryTypeCount; i++){
        if((typeFilter & (1 << i )) && (memProperties.memoryTypes[i].propertyFlags) & properties){
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type ")
}
 void createCommandPool(){
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    //this flag allows us to easily re-record the command buffer every single frame
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device, &poolInfo, nullptr ,& commandPool) != VK_SUCCESS){
        throw std::runtime_error("failed to create command pool");
    }
 }

 void createCommandBuffer(){
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;

    // primary buffers can be submitted directly to the queue for execution
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer)!= VK_SUCCESS){
        throw std::runtime_error("failed to allocate command buffers");
    }
 }

 void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex){
    //start writing to the work ordeer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo)!= VK_SUCCESS){
        throw std::runtime_error("failed to begin recording command buffer ");
    }

    //Start the render pass (The Whiteboard Manager)
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = swapChainExtent;

    //Set background color to black
    VkClearValue clearColor = {{{0.0f,0.0f,0.0f,1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    //this is a command being written to the buffer , not executed immediately
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Bind our Graphics Pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS , graphicsPipeline);

    //Dynamic States
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y =0.0f;

    viewport.width = static_cast<float> (swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainExtent.height);
    viewport.minDepth =0.0f;
    viewport.maxDepth = 1.f;
    vkCmdSetViewport(commandBuffer, 0,1 ,&viewport);

    VkRect2D scissor{};
    scissor.offset = {0,0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer,0,1, &scissor);

    vkCmdDraw(commandBuffer, 3,1,0,0);//Draw the triangle

    vkCmdEndRenderPass(commandBuffer);//finish render pass

    if (vkEndCommandBuffer(commandBuffer)!= VK_SUCCESS){
        throw std::runtime_error("failed to record command buffer");
    }

 }

 void createSyncObjects(){
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateSemaphore(device, &semaphoreInfo , nullptr , &imageAvailableSemaphore)!=VK_SUCCESS ||
       vkCreateSemaphore(device, &semaphoreInfo , nullptr, &renderFinishedSemaphore) !=VK_SUCCESS ||
       vkCreateFence(device, &fenceInfo , nullptr , &inFlightFence) != VK_SUCCESS){

        throw std::runtime_error("failed to create synchronization objects for a frame");
       }
 }

 void drawFrame(){
    //wait for the previous frame to finish
    vkWaitForFences(device, 1, &inFlightFence , VK_TRUE, UINT64_MAX);
    vkResetFences(device , 1, &inFlightFence);

    //ask swap chain for next available image
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain , UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    //reset work order and record new commands
    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, imageIndex);

    //submit work order to gpu
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    //wait to draw till image is actually available
    VkSemaphore waitSemaphore[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] ={VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount =1;
    submitInfo.pWaitSemaphores = waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // signal the semaphore when drawing is completely finished
    VkSemaphore signalSemaphores[]= {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount =1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(graphicsQueue,1 , &submitInfo , inFlightFence)!= VK_SUCCESS){
        throw std::runtime_error("failed to submit draw command buffer ");
    }

    //present finished image to the monitor
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores; // wait for drawing to finish

    VkSwapchainKHR swapChains[] ={swapChain};
    presentInfo.swapchainCount =1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);

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
            drawFrame();
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

        for (auto framebuffer : swapChainFramebuffers){
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        vkDestroyCommandPool(device , commandPool , nullptr);

        vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
        vkDestroyFence(device , inFlightFence,nullptr);
        
        for(auto imageView : swapChainImageViews){
            vkDestroyImageView(device, imageView, nullptr);
        }
        vkDestroyPipeline(device, graphicsPipeline,nullptr);
        vkDestroyPipelineLayout(device , pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
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