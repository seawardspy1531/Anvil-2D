#pragma once
#include "anvLayer.hpp"
#include <vector>

namespace AnvilEngine
{
    class AnvilLayerStack
    {
        public:
         std::vector<AnvilLayer*> Layers;
         unsigned int insertIndex;
         
         ~AnvilLayerStack();
         void PushLayer(AnvilLayer* layer);
         void PopLayer(AnvilLayer* layer);
    };
}