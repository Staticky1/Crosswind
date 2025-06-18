#include "PlayerPilotsWindow.h"
#include "Core/CrosswindApp.h"
#include "Core/EnumHelpers.h"
#include "Walnut/Image.h"

PlayerPilotWindow::PlayerPilotWindow(std::shared_ptr<CampaignContext> context)
	:CrosswindWindow("##PilotInfo")
	, m_Context(context)
	, m_CampaignData(context->campaignData)
	, m_SelectedPlayerPilotIndex(context->selectedPlayerPilotIndex)
{
	m_WindowFlags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar;
	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x - 24, ImGui::GetMainViewport()->WorkSize.y - 120);
	m_hasCloseButton = false;
	//place center screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));

}

void PlayerPilotWindow::RenderContents()
{
	//recalculate window pos and size
	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x - 24, ImGui::GetMainViewport()->WorkSize.y - 120);
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2) + 58));

	PilotData& selectedPlayerPilotData(m_CampaignData.PlayerPilots[m_SelectedPlayerPilotIndex]);
	Squadron& playerSquadron = m_CampaignData.CurrentTheater.GetSquadronsFromID(selectedPlayerPilotData.SquadronId);

	if (!m_PilotImage)
	{
		m_PilotImage = std::make_shared<Walnut::Image>(selectedPlayerPilotData.IconPath);
	}


	float ImageWidth = 200.f;
	float aspectRatio = (float)m_PilotImage->GetHeight() / m_PilotImage->GetWidth();
	ImVec2 imageSize = ImVec2(ImageWidth, ImageWidth * aspectRatio);

	std::string pilotHeading = selectedPlayerPilotData.Rank.abbrevation + " " + selectedPlayerPilotData.PilotName;

	ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
	float nameTextSizeX = ImGui::CalcTextSize(pilotHeading.c_str()).x + ImGui::GetStyle().ItemSpacing.x * 2;
	float childWidth = std::max(nameTextSizeX, (m_WindowSize.x - m_WindowSize.x * 0.3f) - 200 );
	float cursorStartX = ((m_WindowSize.x - childWidth - imageSize.x - ImGui::GetStyle().ItemSpacing.x) * 0.5f);
	ImGui::PopFont();
	ImGui::SetCursorPosX(cursorStartX);
	ImGui::Image(m_PilotImage->GetDescriptorSet(), imageSize);
	ImGui::SameLine();
	ImGui::BeginChild("##PilotHeading",ImVec2(childWidth, ImageWidth * aspectRatio), false, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
	ImGui::Text(pilotHeading.c_str());
	ImGui::PopFont();
	ImGui::Text(LOCAL("NewPilot_Rank")); ImGui::SameLine(); ImGui::Text(selectedPlayerPilotData.Rank.name.c_str());
	ImGui::Text(LOCAL("PilotWindow_PilotStatus")); ImGui::SameLine(); ImGui::Text(LOCAL(Crosswind::ToString<EPilotStatus>(selectedPlayerPilotData.PilotStatus)));
	ImGui::Text(LOCAL("NewPilot_Background"));
	ImGui::PushTextWrapPos(childWidth);
	ImGui::TextWrapped(selectedPlayerPilotData.Background.c_str());
	ImGui::PopTextWrapPos();
	ImGui::EndChild();

	ImGui::Dummy(ImVec2(0, 40));

	std::vector<std::string> victoryCategories = { "AIRCRAFT", "VEHICLE", "RAIL", "WEAPON", "BUILDING", "MARINE" };

	for (int i = 0; i < victoryCategories.size(); ++i)
	{
		std::string id = "##" + victoryCategories[i];
		float itemSizeX = (m_WindowSize.x / 6) - ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosX((itemSizeX + ImGui::GetStyle().ItemSpacing.x) * i);
		CreateVictoryDisplay(id.c_str(), victoryCategories[i], ImVec2(itemSizeX, 300), selectedPlayerPilotData);
		if (i != victoryCategories.size()) ImGui::SameLine();
	}

	//put medals here


}

void PlayerPilotWindow::OnUpdate(float deltaTime)
{
}

void PlayerPilotWindow::CreateVictoryDisplay(std::string displayID, std::string prefix, ImVec2 size, const PilotData& pilotData)
{
	std::string typeName = Crosswind::GetCategoryName(prefix);
	int totalVictoriesOfType = Crosswind::CountVictoriesByCategory(pilotData, prefix);

	ImGui::BeginChild(displayID.c_str(), size, false);
	ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
	float headingSize = ImGui::CalcTextSize(LOCAL(typeName)).x;
	ImGui::SetCursorPosX((size.x - headingSize) * 0.5f);
	ImGui::Text(LOCAL(typeName));
	ImGui::SetCursorPosX((size.x - ImGui::CalcTextSize(std::to_string(totalVictoriesOfType).c_str()).x)*0.5f);
	ImGui::Text(std::to_string(totalVictoriesOfType).c_str());
	ImGui::PopFont();
	ImGui::Separator();

	std::unordered_map<EVictoryType, int> breakdown = Crosswind::GetVictoryBreakdown(pilotData, prefix, true);

	for (const auto& [type, count] : breakdown)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().FramePadding.x);
		ImGui::Text("%s:", LOCAL(Crosswind::ToString<EVictoryType>(type)));
		ImGui::SameLine();
		ImGui::SetCursorPosX(size.x - 20 - ImGui::GetStyle().FramePadding.x);	
		ImGui::Text("%d", count);
	}

	ImGui::EndChild();
}
