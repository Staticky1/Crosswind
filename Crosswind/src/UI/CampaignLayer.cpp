#include "CampaignLayer.h"
#include "UI/CampaignMenu/SquadronInfoWindow.h"
#include "Campaign/CampaignManager.h"
#include "Core/CrosswindApp.h"
#include "UI/CampaignMenu/PlayerPilotsWindow.h"
#include "UI/CampaignMenu/SquadronMissionsWindow.h"
#include "UI/Windows/MissionBriefingWindow.h"

void CampaignLayer::OnUIRender()
{
	//CrosswindLayer::OnUIRender(); //dont use parent behavour
   // ImGui::SetNextWindowFocus();
    m_Windows[m_SelectedTabIndex]->OnRender();

	//Render Tab bar
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
   
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + main_viewport->WorkSize.x - 635, main_viewport->WorkPos.y + 57));

    Squadron playerSquadron = Core::Instance().GetApp()->GetCampaignData().CurrentTheater.GetSquadronsFromID(Core::Instance().GetApp()->GetCampaignData().PlayerPilots[m_SelectedPlayerPilotIndex].SquadronId);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 15)); // Wider & taller tab buttons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));    // More spacing between tabs
    ImGui::SetNextWindowSize(ImVec2(main_viewport->WorkSize.x, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2));
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + 56));

    if (ImGui::Begin("##CampaignTabs", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text(LOCAL("CurrentDate")); ImGui::SameLine();
        ImGui::Text(Core::ReformatDate(Core::Instance().GetApp()->GetCampaignData().CurrentDateTime.ToDateString()).c_str()); 
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(50, 0));
        ImGui::SameLine();
        ImGui::Text(LOCAL("CurrentTime")); ImGui::SameLine();
        ImGui::Text((Core::Instance().GetApp()->GetCampaignData().CurrentDateTime.ToTimeString()).c_str());
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(50, 0));
        ImGui::SameLine();
        ImGui::Text(LOCAL("Airfield")); ImGui::SameLine();
        const auto* AirfieldValue = Core::GetActiveEntryRef<ValueStartDate>(
            playerSquadron.locations,
            Core::Instance().GetApp()->GetCampaignData().CurrentDateTime,
            [](const ValueStartDate& e) { return e.startDate; }
        );
        if (Core::Instance().GetApp()->GetCampaignData().CurrentTheater.TheaterAirfields.LoadedAirfields.size() == 0)
            Core::Instance().GetApp()->GetCampaignData().CurrentTheater.LoadAirfields();

        Airfield CurrentAirfield = Core::Instance().GetApp()->GetCampaignData().CurrentTheater.TheaterAirfields.LoadedAirfields.at(AirfieldValue->value);

        ImGui::Text(CurrentAirfield.name.c_str());
        ImGui::SameLine();
        float tabBarWidth = 0.f;
        std::vector<std::string> tabLabels = { LOCAL("Tab_PlayerPilot"), LOCAL("Tab_Squadrons"), LOCAL("Tab_Headquarters") };
        // Get current style for padding
        ImGuiStyle& style = ImGui::GetStyle();
        float tabPaddingX = style.FramePadding.x * 2.0f; // Left and right
        float tabSpacing = style.ItemSpacing.x; // Space between tabs

        for (size_t i = 0; i < tabLabels.size(); ++i)
        {
            ImVec2 labelSize = ImGui::CalcTextSize(tabLabels[i].c_str());
            tabBarWidth += labelSize.x + tabPaddingX;

            if (i < tabLabels.size() - 1)
                tabBarWidth += tabSpacing;
        }
        tabBarWidth += 50;

        ImGui::SetCursorScreenPos(ImVec2(main_viewport->WorkPos.x + main_viewport->WorkSize.x - tabBarWidth, main_viewport->WorkPos.y + 57));

        if (ImGui::BeginTabBar("##CampaignTabBar")) {
            if (ImGui::BeginTabItem(LOCAL("Tab_PlayerPilot"))) { m_SelectedTabIndex = 0; ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem(LOCAL("Tab_Squadron"))) { m_SelectedTabIndex = 1; ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem(LOCAL("Tab_Missions"))) { m_SelectedTabIndex = 2; ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem(LOCAL("Tab_Headquarters"))) { m_SelectedTabIndex = 1; ImGui::EndTabItem(); }
            ImGui::EndTabBar();
        }
        ImGui::PopStyleVar(2);
    }

    if (bShouldShowNews)
    {
        newsPopup.ShowNewsCarouselModal(CurrentNewsItems, newsItemIndex, bShouldShowNews);
    }
    ImGui::End();

    if (bShowMissionBriefing)
    {
        if (m_MissionBriefingWindow)
        {
            m_MissionBriefingWindow->OnRender();
        }
    }
}

void CampaignLayer::OnAttach()
{
    m_CampaignContext = std::make_shared<CampaignContext>(Core::Instance().GetApp()->GetCampaignData(), m_SelectedPlayerPilotIndex);

    PushWindow(std::make_unique<PlayerPilotWindow>(m_CampaignContext));
    PushWindow(std::make_unique<SquadronInfoWindow>(m_CampaignContext));
    PushWindow(std::make_unique<SquadronMissionsWindow>(m_CampaignContext));



    CampaignManager::Instance().SetCampaignLayer(this);
}

void CampaignLayer::OnUpdate(float DeltaTime)
{
	//CrosswindLayer::OnUpdate(DeltaTime); //dont use parent behavour
    m_Windows[m_SelectedTabIndex]->OnUpdate(DeltaTime);

    
    PilotData& selectedPlayerPilotData(Core::Instance().GetApp()->GetCampaignData().PlayerPilots[m_SelectedPlayerPilotIndex]);
    Squadron& playerSquadron = Core::Instance().GetApp()->GetCampaignData().CurrentTheater.GetSquadronsFromID(selectedPlayerPilotData.SquadronId);
    
    if (Squadron::GetCurrentMission(playerSquadron) && Squadron::GetCurrentMission(playerSquadron) != m_CurrentMission)
    {
        m_CurrentMission = Squadron::GetCurrentMission(playerSquadron);
        m_MissionContext = std::make_shared<MissionContext>(Core::Instance().GetApp()->GetCampaignData(), m_SelectedPlayerPilotIndex, *m_CurrentMission);

    } else if (!Squadron::GetCurrentMission(playerSquadron))    
    {
        m_MissionContext = nullptr;
        m_CurrentMission = nullptr;
    }

    for (size_t i = 0; i < m_Windows.size(); ++i) {
        CrosswindWindow* window = m_Windows[i].get();
        auto* missionBriefing = dynamic_cast<MissionBriefingWindow*>(m_Windows[i].get());
        auto* squadronMisionWindow = dynamic_cast<SquadronMissionsWindow*>(m_Windows[i].get());

        switch (window->GetRequestedAction()) {
        case EWindowActions::CLOSE:
            if (missionBriefing)
            {
                bShowMissionBriefing = false;
            }
            break;
        case EWindowActions::NEXT:

            if (missionBriefing)
            {
                missionBriefing->NextStage();
            }
            if (squadronMisionWindow)
            {
                bShowMissionBriefing = true;
            }
            break;
        case EWindowActions::BACK:
            if (missionBriefing)
            {
                missionBriefing->PreviousStage();
            }
            break;
        case EWindowActions::NONE:
            break;
        }
    }

    if (bShowMissionBriefing)
    {
        if (m_MissionBriefingWindow)
        {
            m_MissionBriefingWindow->OnUpdate(DeltaTime);
        }
        else
        {
            if (m_MissionContext)
                m_MissionBriefingWindow = std::make_unique<MissionBriefingWindow>(m_MissionContext);
        }
    }
}

void CampaignLayer::AddNewsItem(NewsItem& item)
{
    
    if (item.newsImagePath != "")
        item.newsImage = std::make_shared<Walnut::Image>(item.newsImagePath);
    CurrentNewsItems.push_back(item);
    newsPopup.AddNewsItem(item.newsHeading, item.newsText, item.newsImagePath);
}

void CampaignLayer::ShowNewsItems()
{
    if (CurrentNewsItems.size() > 0)
    bShouldShowNews = true;
}

void CampaignLayer::ClearNewsItems()
{
    bShouldShowNews = false;
    CurrentNewsItems.clear();
    newsPopup.ClearNewsItems();
}
