#include "CrosswindLayer.h"
#include "CrosswindWindow.h"

void CrosswindLayer::OnAttach()
{

}

void CrosswindLayer::OnUpdate(float deltaTime)
{
    for (auto& window : m_Windows)
        window->OnUpdate(deltaTime);
}

void CrosswindLayer::OnUIRender()
{
    for (auto& window : m_Windows)
        window->OnRender();
}

void CrosswindLayer::PushWindow(std::unique_ptr<CrosswindWindow> window)
{
    m_Windows.push_back(std::move(window));
}

void CrosswindLayer::RemoveWindow(CrosswindWindow* target)
{
    auto it = std::remove_if(m_Windows.begin(), m_Windows.end(),
        [target](const std::unique_ptr<CrosswindWindow>& window) {
            return window.get() == target;
        });

    if (it != m_Windows.end()) {
        m_Windows.erase(it, m_Windows.end());
    }
}
