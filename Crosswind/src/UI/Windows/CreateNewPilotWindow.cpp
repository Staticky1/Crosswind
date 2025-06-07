#include "CreateNewPilotWindow.h"
#include <fstream>
#include <random>
#include <filesystem>
#include "Core/TextureGarbageCollector.h"
#include "UI/MainMenu/CreateNewCampaignPopupWindow.h"
#include "UI/WindowContexts/CampaignSetupContext.h"
#include "UI/MainMenu/SquadronSelectionWindow.h"
#include "Campaign/NewPilot.h"

CreateNewPilotWindow::CreateNewPilotWindow(std::shared_ptr<WindowContext> context)
	:CrosswindWindow(LOCAL("NewPilot_Title"))
	, m_Context(std::static_pointer_cast<NewPilotContext>(context))
	, m_CampaignData(m_Context->campaignData)
	, m_PilotData(m_Context->pilotData)
{
	m_WindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	m_WindowSize = ImVec2(std::min(ImGui::GetMainViewport()->WorkSize.x, DesiredWindowSize.x), std::min(ImGui::GetMainViewport()->WorkSize.y, DesiredWindowSize.y));
	m_hasCloseButton = false;
	//place center screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));
}

void CreateNewPilotWindow::RenderContents()
{
	constexpr int MIN_NAME_LENGTH = 3;
	constexpr int MAX_NAME_LENGTH = 24;
	
	//update size
	m_WindowSize = ImVec2(std::min(ImGui::GetMainViewport()->WorkSize.x, DesiredWindowSize.x), std::min(ImGui::GetMainViewport()->WorkSize.y, DesiredWindowSize.y));
	m_hasCloseButton = false;
	//place center screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));


	//Load random button image
	if (!RandomNameButtonImage)
	{
		RandomNameButtonImage = std::make_shared<Walnut::Image>(RandomImagePath);
	}
			//Load random button image
	if (!RandomBackgroundButtonImage)
	{
		RandomBackgroundButtonImage = std::make_shared<Walnut::Image>(RandomImagePath);
	}

	//Name Input


	ImGui::BeginChild("##PilotInfo", ImVec2(ImGui::GetContentRegionAvail().x - 300, ImGui::GetContentRegionAvail().y * 0.6f));
	ImGui::Text(LOCAL("NewPilot_PilotName"));
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40);
	ImGui::InputText("##PilotName", m_NameBuffer, IM_ARRAYSIZE(m_NameBuffer));
	ImGui::SameLine(); 
	if (ImGui::ImageButton(RandomNameButtonImage->GetDescriptorSet(), ImVec2(20, 20)))
	{
		m_PilotData.PilotName = "";
		m_NameInitialized = false;
	}

	if (m_CampaignData.FinalSelectedService.nationality == "Russia")
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text(LOCAL("NewPilot_FemalePilot")); ImGui::SameLine();
		if (ImGui::Checkbox("##FemalePilot", &m_PilotData.bFemalePilot))
		{
			m_NameInitialized = false;
			ClearPilotImages();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.0f); // suitable width
			ImGui::TextUnformatted(
				LOCAL("NewPilot_FemalePilotTooltip")
			);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//Pilot Rank
	std::vector<Rank> ranks = m_CampaignData.FinalSelectedService.Ranks;
	if (m_PilotData.Rank.id == "") m_PilotData.Rank = ranks[0]; 
	//Order No.5/1943 - pilots now start as mLt
	if (Core::HasDatePassed(m_CampaignData.StartDate, "1943-01-6") && m_CampaignData.FinalSelectedService.id == "VVS")
	{
		ranks.erase(
			std::remove_if(ranks.begin(), ranks.end(),
				[](const Rank& r) {
					return r.id == "VVS_szt" || r.id == "VVS_sszt"; // Remove sargent ranks
				}),
			ranks.end());
	}

	
	ImGui::Text(LOCAL("NewPilot_Rank"));
	ImGuiComboFlags RankBoxFlags = ImGuiComboFlags_None;
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::BeginCombo("##Rank", LOCAL(ranks[0].id), RankBoxFlags) )
	{
		for (int i = 0; i < ranks.size(); ++i) {
			const bool isSelected = (SelectedRankIndex == i);
			if (ImGui::Selectable(LOCAL(ranks[i].id), isSelected)) {
				SelectedRankIndex = i;
				m_PilotData.Rank = ranks[SelectedRankIndex];
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::EndChild();
	ImGui::SameLine();

	//Picture

	ImGui::BeginChild("##PilotPicture", ImVec2(300, ImGui::GetContentRegionAvail().y * 0.6f));

	if (!m_PilotImages.empty())
	{
		// Ensure index is in range
		m_SelectedImageIndex = std::clamp(m_SelectedImageIndex, 0, (int)m_PilotImages.size() - 1);
		auto& image = m_PilotImages[m_SelectedImageIndex];

		ImGui::SetCursorPosX((300 - ImGui::CalcTextSize(LOCAL("NewPilot_Image")).x) * 0.5f);
		ImGui::Text(LOCAL("NewPilot_Image"));

		// Image display (scale as needed)
		ImVec2 imageSize = ImVec2(180, 250); // change size as needed
		ImGui::SetCursorPosX((300 - 180) * 0.5f);
		ImGui::Image((ImTextureID)(uintptr_t)image->GetDescriptorSet(), imageSize);

		ImGui::SetCursorPosX((300 - 180) * 0.5f);
		// Arrow buttons
		if (ImGui::ArrowButton("##LeftArrow", ImGuiDir_Left)) {
			m_SelectedImageIndex = (m_SelectedImageIndex - 1 + m_PilotImages.size()) % m_PilotImages.size(); // wrap-around
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX((300 - 180) * 0.5f + 150);

		if (ImGui::ArrowButton("##RightArrow", ImGuiDir_Right)) {
			m_SelectedImageIndex = (m_SelectedImageIndex + 1) % m_PilotImages.size(); // wrap-around
		}

	}
	else
	{
		ImGui::Text("No images available.");
	}

	ImGui::EndChild();
	ImGui::BeginChild("##PilotBackground", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 40));
	//Background

	//ImGui::SetCursorPosY(m_WindowSize.y * 0.5f);
	ImGui::Text(LOCAL("NewPilot_Background"));

	ImGui::PushTextWrapPos(0.0f);
	if (strlen(m_BackgroundBuffer) > 0)
		ImGui::TextUnformatted(m_BackgroundBuffer);
	else
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "<No background entered>");
	ImGui::PopTextWrapPos();

	ImGui::Spacing();

	// Edit button opens popup
	if (ImGui::Button(LOCAL("NewPilot_EditBackground"))) {
		isEditing = true;
		std::string tempStr(m_BackgroundBuffer);
		InsertLineBreaks(tempStr, 80);  // Wrap at ~60 chars
		strncpy_s(m_TempBackgroundBuffer, tempStr.c_str(), sizeof(m_TempBackgroundBuffer));
		m_TempBackgroundBuffer[sizeof(m_TempBackgroundBuffer) - 1] = '\0';

		ImGui::OpenPopup("##EditBackgroundPopup");
	}

	ImGui::SameLine();
	if (ImGui::ImageButton(RandomBackgroundButtonImage->GetDescriptorSet(), ImVec2(20, 20)))
	{
		m_PilotData.Background = "";
		m_BackgroundInitialized = false;
	}

	if (ImGui::BeginPopupModal(LOCAL("##EditBackgroundPopup"), &isEditing, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::InputTextMultiline(
			"##editbackground",
			m_TempBackgroundBuffer,
			IM_ARRAYSIZE(m_TempBackgroundBuffer),
			ImVec2(m_WindowSize.x, ImGui::GetTextLineHeight() * 12),
			ImGuiInputTextFlags_AllowTabInput
		);

		ImGui::Spacing();
		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			std::string cleanStr(m_TempBackgroundBuffer);
			//RemoveLineBreaks(cleanStr);
			strncpy_s(m_BackgroundBuffer, cleanStr.c_str(), sizeof(m_BackgroundBuffer));
			m_BackgroundBuffer[sizeof(m_BackgroundBuffer) - 1] = '\0';
			isEditing = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
			isEditing = false;
		}

		ImGui::EndPopup();
	}
	ImGui::EndChild();

	if (ImGui::Button(LOCAL("Button_Back")))
	{
		SavePilotData();
		CurrentAction = EWindowActions::BACK;
	}
	ImGui::SameLine();

	if (ImGui::Button(LOCAL("Button_Next")))
	{
		SavePilotData();
		CurrentAction = EWindowActions::NEXT;
	}

}

void CreateNewPilotWindow::InsertLineBreaks(std::string& text, int maxCharsPerLine) {
	int lastSpace = -1;
	int lineStart = 0;

	for (size_t i = 0; i < text.size(); ++i) {
		char c = text[i];
		if (c == ' ' || c == '\t') {
			lastSpace = (int)i;
		}
		if (c == '\n') {  // Reset on existing line breaks
			lineStart = (int)i + 1;
			lastSpace = -1;
		}
		if ((int)(i - lineStart) >= maxCharsPerLine) {
			if (lastSpace != -1) {
				// Replace last space with newline
				text[lastSpace] = '\n';
				lineStart = lastSpace + 1;
				lastSpace = -1;
			}
			else {
				// No space found, force break at max length
				text.insert(i, 1, '\n');
				lineStart = (int)i + 1;
				++i; // Skip inserted newline
			}
		}
	}
}

void CreateNewPilotWindow::RemoveLineBreaks(std::string& text) {
	text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
}

void CreateNewPilotWindow::ClearPilotImages()
{
	for (int i = 0; i < m_PilotImages.size(); ++i)
	{
		TextureGarbageCollector::Submit(m_PilotImages[i]);
	}

	m_PilotImages.clear();
}

void CreateNewPilotWindow::SavePilotData()
{
	std::string basePath = "data/images/pilotPictures/" + m_CampaignData.FinalSelectedService.picture;
	std::string prefix = m_CampaignData.FinalSelectedService.picture;

	if (m_PilotData.bFemalePilot)
	{
		basePath = basePath + "/female";
		prefix = prefix + "Female";
	}

	std::stringstream ss;
	ss << basePath << "/" << prefix << std::setfill('0') << std::setw(2) << m_SelectedImageIndex << ".jpg";
	std::string filePath = ss.str();
	if (!std::filesystem::exists(filePath))
	m_PilotData.IconPath = filePath;

	m_PilotData.Nationality = m_CampaignData.FinalSelectedService.nationality;
	m_PilotData.ServiceId = m_CampaignData.FinalSelectedService.id;
	m_PilotData.SquadronId = m_CampaignData.FinalSelectedSquadron.id;
	m_PilotData.PilotName = m_NameBuffer;
	m_PilotData.Background = m_BackgroundBuffer;
	m_PilotData.bIsPlayerPilot = true;
}

void CreateNewPilotWindow::OnUpdate(float deltaTime)
{
	//generate a random name
	if (!m_NameInitialized) {

		if (m_PilotData.PilotName == "")
		{
			std::string generatedName = Crosswind::GetName(m_CampaignData.FinalSelectedService.nationality,m_PilotData.bFemalePilot);
			strncpy_s(m_NameBuffer, generatedName.c_str(), sizeof(m_NameBuffer));
			m_NameBuffer[sizeof(m_NameBuffer) - 1] = '\0'; // Ensure null termination
			m_NameInitialized = true;
		}
		else
		{
			std::string generatedName = m_PilotData.PilotName;
			strncpy_s(m_NameBuffer, generatedName.c_str(), sizeof(m_NameBuffer));
			m_NameBuffer[sizeof(m_NameBuffer) - 1] = '\0'; // Ensure null termination
			m_NameInitialized = true;
			m_NameInitialized = true;
		}

		if (m_BackgroundInitialized)
		{
			std::string NewBackground = Crosswind::GetBackground(m_CampaignData.FinalSelectedService.id, m_NameBuffer, false, m_PilotData.bFemalePilot);
			strncpy_s(m_BackgroundBuffer, NewBackground.c_str(), sizeof(m_BackgroundBuffer));
			m_BackgroundBuffer[sizeof(m_BackgroundBuffer) - 1] = '\0'; // Ensure null termination
		}
		SavePilotData();
	}	

	//generate a random background
	if (!m_BackgroundInitialized)
	{
		if (m_PilotData.Background == "")
		{
			std::string NewBackground = Crosswind::GetBackground(m_CampaignData.FinalSelectedService.id, m_NameBuffer, true, m_PilotData.bFemalePilot);
			strncpy_s(m_BackgroundBuffer, NewBackground.c_str(), sizeof(m_BackgroundBuffer));
			m_BackgroundBuffer[sizeof(m_BackgroundBuffer) - 1] = '\0'; // Ensure null termination
			m_BackgroundInitialized = true;
		}
	}

	//Load pilot images
	if (m_PilotImages.empty())
	{
		m_PilotImages.clear();
		m_SelectedImageIndex = 0;

		std::string basePath = "data/images/pilotPictures/" + m_CampaignData.FinalSelectedService.picture;
		std::string prefix = m_CampaignData.FinalSelectedService.picture;

		if (m_PilotData.bFemalePilot)
		{
			basePath = basePath + "/female";
			prefix = prefix + "Female";
		}
		
		int index = 1;
		while (true)
		{
			// Format the filename (e.g., Russian01.png)
			std::stringstream ss;
			ss << basePath << "/" << prefix << std::setfill('0') << std::setw(2) << index << ".jpg";
			std::string filePath = ss.str();
			if (!std::filesystem::exists(filePath))
				break;

			// Load with Walnut
			std::shared_ptr<Walnut::Image> image = std::make_shared<Walnut::Image>(filePath);
			if (image && image->GetWidth() > 0 && image->GetHeight() > 0) {
				m_PilotImages.push_back(image);
			}

			index++;
		}

		// Random selection
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, static_cast<int>(m_PilotImages.size()) - 1);
		m_SelectedImageIndex = dis(gen);

		SavePilotData();
	}



}

std::unique_ptr<CrosswindWindow> CreateNewPilotWindow::CreateNextWindow(std::shared_ptr<WindowContext> context)
{
	auto newPilotContext = std::static_pointer_cast<NewPilotContext>(context);
	return std::make_unique<CreateNewCampaignPopupWindow>(newPilotContext);
}

std::unique_ptr<CrosswindWindow> CreateNewPilotWindow::CreateBackWindow(std::shared_ptr<WindowContext> context)
{
	auto campaignContext = std::static_pointer_cast<CampaignSetupContext>(context);
	return std::make_unique<SelectSquadronWindow>(campaignContext);
}

