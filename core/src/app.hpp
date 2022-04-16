#pragma once
#include "window_manager.hpp"
#include "anvPipeline.hpp"
#include "anvSwapChain.hpp"
#include "anvModel.hpp"
//#include "anvImLayer.hpp"
#include <array>
#include <GLFW/glfw3.h>
#include <memory>

namespace AnvilEngine{

    class AnvilEngineApplication{

        public:
            
            WindowManager WindowManager{800, 600, "Anvil Engine ver 0.0.0"};
            AnvDevice anvDevice{WindowManager.Window};
            VkPipelineLayout pipelineLayout;
            std::unique_ptr<anvPipeline> AnvilPipeline;
            std::vector<VkCommandBuffer> CommandBuffers;
            anvSwapChain AnvilSwapChain{anvDevice, WindowManager.GetExtent()};
            //anvImLayer ImLayer{anvDevice, WindowManager.Window, AnvilSwapChain};
            std::unique_ptr<anvModel> model;

            

            AnvilEngineApplication()
            {
                ENGINE_INFO("Starting Engine");
                ENGINE_INFO(glfwGetVersionString());
                LoadModels();
                CreatePipelineLayout();
                CreatePipeline(AnvilSwapChain);
                CreateCommandBuffers();
            }


            anvSwapChain* EngineGetSwapChain()
            {
                return &AnvilSwapChain;
            }


            void CreatePipelineLayout()
            {
                VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
                pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.setLayoutCount = 0;
                pipelineLayoutInfo.pSetLayouts = nullptr;
                pipelineLayoutInfo.pushConstantRangeCount = 0;
                pipelineLayoutInfo.pPushConstantRanges = nullptr;
                if (vkCreatePipelineLayout(anvDevice.m_device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
                    ENGINE_ERROR("Failed to create pipeline layout");
                }
            }



            void CreatePipeline(anvSwapChain &swap_chain)
            {
                auto pipelineConfig = anvPipeline::DefaultPipelinecfgInfo(swap_chain.width(), swap_chain.height());
                pipelineConfig.renderPass = swap_chain.getRenderPass();
                pipelineConfig.pipelineLayout = pipelineLayout;
                AnvilPipeline = std::make_unique<anvPipeline>(
                    anvDevice, 
                    "shaders/vshader.spv", 
                    "shaders/fshader.spv", 
                    pipelineConfig);
            }



            void CreateCommandBuffers()
            {
                CommandBuffers.resize(AnvilSwapChain.imageCount());

                VkCommandBufferAllocateInfo allocInfo{};
                allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocInfo.commandPool = anvDevice.commandPool;
                allocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

                if (vkAllocateCommandBuffers(anvDevice.m_device, &allocInfo, CommandBuffers.data()) != VK_SUCCESS)
                {
                    ENGINE_ERROR("Failed to allocate command buffers");
                }

                for (int i = 0; i < CommandBuffers.size(); i++)
                {
                    VkCommandBufferBeginInfo beginInfo{};
                    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    if (vkBeginCommandBuffer(CommandBuffers[i], &beginInfo) != VK_SUCCESS)
                    {
                        ENGINE_ERROR("Failed to begin recording command buffer");
                    }

                    VkRenderPassBeginInfo renderPassInfo{};
                    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                    renderPassInfo.renderPass = AnvilSwapChain.getRenderPass();
                    renderPassInfo.framebuffer = AnvilSwapChain.getFrameBuffer(i);

                    renderPassInfo.renderArea.offset = {0, 0};
                    renderPassInfo.renderArea.extent = AnvilSwapChain.getSwapChainExtent();

                    std::array<VkClearValue, 2> clearValues{};
                    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
                    clearValues[1].depthStencil = {1.0f, 0};
                    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
                    renderPassInfo.pClearValues = clearValues.data();

                    vkCmdBeginRenderPass(CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

                    AnvilPipeline->Bind(CommandBuffers[i]);
                    model->Bind(CommandBuffers[i]);
                    model->Draw(CommandBuffers[i]);

                    vkCmdEndRenderPass(CommandBuffers[i]);
                    if (vkEndCommandBuffer(CommandBuffers[i]) != VK_SUCCESS)
                    {
                        ENGINE_ERROR("Failed to record command buffer");
                    }
                }
            }
            void DrawFrame()
            {
                uint32_t imgIndex;
                auto result = AnvilSwapChain.acquireNextImage(&imgIndex);

                if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
                {
                    ENGINE_ERROR("Failed to aquire swap chain image");
                }
                
                //ImLayer.ImDraw(CommandBuffers[imgIndex]);
                result = AnvilSwapChain.submitCommandBuffers(&CommandBuffers[imgIndex], &imgIndex);
                if (result != VK_SUCCESS)
                {
                    ENGINE_ERROR("Failed to present swap chain image");
                }
            }


            void Run(){
                ENGINE_INFO("Running. . .");
                while (!glfwWindowShouldClose(WindowManager.Window)){

                    //ImGui::Begin("Hello World!");
            

                    glfwPollEvents();
                    DrawFrame();
                }
            }

            


            void LoadModels()
            {
                std::vector<anvModel::Vertex> vertices{
                    /*TRIANGLE 1*/
                    {{0.0f, -0.5f},{1.0f, 0.0f, 0.0f}},
                    {{0.5f, 0.5f},{0.0f, 1.0f, 0.0f}},
                    {{-0.5f, 0.5f},{0.0f, 0.0f, 1.0f}},
                    /* TRIANGLE 2 */
                    {{0.7f, -0.4f},{1.0f, 0.0f, 0.8f}},
                    {{1.0f, 0.5f},{0.2f, 0.8f, 0.7f}},
                    {{0.5f, 0.0f},{0.0f, 0.7f, 1.0f}},
                    /* TRIANGLE 3 */
                    {{0.2f, -0.1f},{1.0f, 0.4f, 1.0f}},
                    {{-1.0f, -0.2f},{1.0f, 1.0f, 0.4f}},
                    {{-0.5f, -0.0f},{0.5f, 1.0f, 1.0f}},

                };

                model = std::make_unique<anvModel>(anvDevice, vertices);
            }

    };

}
