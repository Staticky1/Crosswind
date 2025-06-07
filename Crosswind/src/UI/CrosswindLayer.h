#pragma once

#include "Walnut/Layer.h"
#include "Core.h"
#include "UI/CrosswindWindow.h"

class CrosswindLayer : public Walnut::Layer {
public:
    void OnAttach() override;

    void OnUpdate(float deltaTime) override;

    void OnUIRender() override;

    void PushWindow(std::unique_ptr<CrosswindWindow> window);

    void RemoveWindow(CrosswindWindow* target);

  

protected:
    std::vector<std::unique_ptr<CrosswindWindow>> m_Windows;

};