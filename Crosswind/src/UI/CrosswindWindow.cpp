#include "CrosswindWindow.h"

CrosswindWindow::CrosswindWindow(const std::string& name)
	: m_Name(name), m_Open(true)
{
}

void CrosswindWindow::OnRender()
{
    if (!m_Open) return;

    
    //Get center of screen
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(m_WindowPos);
    ImGui::SetNextWindowSize(m_WindowSize);

    bool* bCloseButton = m_hasCloseButton ? &m_Open : nullptr;

    ImGui::Begin(m_Name.c_str(), bCloseButton, m_WindowFlags);
    RenderContents();
    ImGui::End();
}

std::unique_ptr<CrosswindWindow> CrosswindWindow::CreateNextWindow(std::shared_ptr<WindowContext> context)
{
    return nullptr;
}

std::unique_ptr<CrosswindWindow> CrosswindWindow::CreateBackWindow(std::shared_ptr<WindowContext> context)
{
    return nullptr;
}

bool CrosswindWindow::IsOpen()
{
    return m_Open;
}

void CrosswindWindow::Open()
{
    m_Open = true;
}

void CrosswindWindow::Close()
{
    m_Open = false;
}

EWindowActions CrosswindWindow::GetRequestedAction()
{
    return CurrentAction;
}
