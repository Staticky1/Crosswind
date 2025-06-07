#include "CampaignLayer.h"

void CampaignLayer::OnUIRender()
{
	//CrosswindLayer::OnUIRender(); //dont use parent behavour

	//Render Tab bar
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + main_viewport->WorkSize.x - 635, main_viewport->WorkPos.y + 57));
    if (ImGui::Begin("##CampaignTabs", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
    {
        ImVec2 ButtonSize = ImVec2(200, 50);
        if (ImGui::Button("Pilot", ButtonSize)) m_SelectedTabIndex = 0;
        ImGui::SameLine();
        if (ImGui::Button("Squadron", ButtonSize)) m_SelectedTabIndex = 1;
        ImGui::SameLine();
        if (ImGui::Button("Headquarters", ButtonSize)) m_SelectedTabIndex = 2;
    }
    ImGui::End();



}

void CampaignLayer::OnAttach()
{
}

void CampaignLayer::OnUpdate(float DeltaTime)
{
	//CrosswindLayer::OnUpdate(DeltaTime); //dont use parent behavour
}
