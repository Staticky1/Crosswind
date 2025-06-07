#include "NewCampaignWindow.h"
#include "UI/WindowContexts/CampaignSetupContext.h"
#include "Walnut/Image.h"
#include "UI/MainMenu/SquadronSelectionWindow.h"

NewCampaignWindow::NewCampaignWindow(std::shared_ptr<WindowContext> context)
	:CrosswindWindow(LOCAL("NewCampaign_Title"))
	,m_Context(std::static_pointer_cast<CampaignSetupContext>(context))
	, m_CampaignData(m_Context->campaignData)
{

	m_WindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	m_hasCloseButton = false;

	m_WindowSize = ImVec2(500, 550);
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));
}

void NewCampaignWindow::RenderContents()
{
	constexpr int MIN_NAME_LENGTH = 3;
	constexpr int MAX_NAME_LENGTH = 24;

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));


	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(LOCAL("NewCampaign_CampaignName")).x) * 0.5f);
	ImGui::Text(LOCAL("NewCampaign_CampaignName"));

	static char CampaignNameInput[MAX_NAME_LENGTH] = "NewCampaign";

	ImGui::SetNextItemWidth(ImGui::GetWindowSize().x - 20);
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x - 20)) * 0.5f);
	ImGui::InputText("NewCampaign", CampaignNameInput, IM_ARRAYSIZE(CampaignNameInput));
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip((LOCAL("NewCampaign_CampaignNameHint")));

	m_CampaignData.NewCampaignName = CampaignNameInput;

	// Path resolution
	std::string saveDirectory = "data/saves/";
	std::string campaignFileName = saveDirectory + CampaignNameInput + ".xml";

	// Validation checks
	bool nameTooShort = std::strlen(CampaignNameInput) < MIN_NAME_LENGTH;
	bool fileExists = std::filesystem::exists(campaignFileName);


	// Warnings
	if (nameTooShort) {
		ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), LOCAL("Warning_NameTooShort"), MIN_NAME_LENGTH);
	}
	else if (fileExists) {
		ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), LOCAL("Warning_NameExists"));
	}
	else
	{
		ImGui::Dummy(ImVec2(0, ImGui::GetFontSize()));
	}

	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(LOCAL("NewCampaign_Side")).x) * 0.5f);
	ImGui::TextUnformatted(LOCAL("NewCampaign_Side"));

	ImVec2 ButtonSize = ImVec2(75, 75);
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (ButtonSize.x * 2.5f)) * 0.5f - 6);
	if (m_CampaignData.bAlliesSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.5f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.8f));
		if (ImGui::Button(LOCAL("NewCampaign_Side_Allies"), ButtonSize))
		{
			m_CampaignData.bAlliesSelected = true;
			m_CampaignData.bAxisSelected = false;
		}
		ImGui::PopStyleColor(3);
	}
	else
	{
		if (ImGui::Button(LOCAL("NewCampaign_Side_Allies"), ButtonSize))
		{
			m_CampaignData.bAlliesSelected = true;
			m_CampaignData.bAxisSelected = false;
		}
	}
	ImGui::SameLine(); ImGui::Dummy(ImVec2(ButtonSize.x / 2, 0)); ImGui::SameLine();
	if (m_CampaignData.bAxisSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.5f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.8f));

		if (ImGui::Button(LOCAL("NewCampaign_Side_Axis"), ButtonSize))
		{
			m_CampaignData.bAlliesSelected = false;
			m_CampaignData.bAxisSelected = true;
		}
		ImGui::PopStyleColor(3);
	}
	else
	{
		if (ImGui::Button(LOCAL("NewCampaign_Side_Axis"), ButtonSize))
		{
			m_CampaignData.bAlliesSelected = false;
			m_CampaignData.bAxisSelected = true;
		}
	}

	ImGui::Dummy(ImVec2(0, 20));

	//Theater


	std::string Stalingrad = Core::Instance().GetLocalizationManager().GetString("Theater_Stalingrad");

	//check if we have a theater loaded, if not load index 0
	if (m_CampaignData.LoadedTheater.name == std::string())
	{
		m_CampaignData.LoadedTheater = Theater::LoadTheaterFromId(0);
	}

	const char* Theateritems[] = { Stalingrad.c_str() };
	//static int SelectedTheater = 0; // Here we store our selection data as an index.
	const char* combo_preview_value = Theateritems[m_CampaignData.SelectedTheaterIndex];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo(LOCAL("NewCampaign_Theater"), combo_preview_value, 0))
	{
		for (int n = 0; n < IM_ARRAYSIZE(Theateritems); n++)
		{
			const bool is_selected = (m_CampaignData.SelectedTheaterIndex == n);
			if (ImGui::Selectable(Theateritems[n], is_selected))
				if (n != m_CampaignData.SelectedTheaterIndex)
				{
					m_CampaignData.SelectedTheaterIndex = n;
					m_CampaignData.LoadedTheater = Theater::LoadTheaterFromId(m_CampaignData.SelectedTheaterIndex);

				}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	//Start Date

	if (ImGui::BeginCombo(LOCAL("NewCampaign_StartingPhase"), LOCAL(m_CampaignData.LoadedTheater.phases[m_CampaignData.SelectedPhaseIndex].name))) {
		for (int i = 0; i < m_CampaignData.LoadedTheater.phases.size(); ++i) {
			bool isSelected = (i == m_CampaignData.SelectedPhaseIndex);
			if (ImGui::Selectable(LOCAL(m_CampaignData.LoadedTheater.phases[i].name), isSelected)) {
				m_CampaignData.SelectedPhaseIndex = i;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Text(LOCAL("NewCampaign_StartingDate"));
	ImGui::Text(m_CampaignData.LoadedTheater.phases[m_CampaignData.SelectedPhaseIndex].startDate.c_str());
	m_CampaignData.StartDate = m_CampaignData.LoadedTheater.phases[m_CampaignData.SelectedPhaseIndex].startDate;
	ImGui::Text(LOCAL("NewCampaign_Description"));
	ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
	ImGui::Text(m_CampaignData.LoadedTheater.phases[m_CampaignData.SelectedPhaseIndex].description.c_str());
	ImGui::PopTextWrapPos();

	ImGui::Dummy(ImVec2(0, 10));

	if (ImGui::Button(LOCAL("Button_Cancel")))
	{
		bCancelButtonPressed = true;
	}
	ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x);

	ImGui::BeginDisabled((nameTooShort || fileExists) || (!m_CampaignData.bAxisSelected && !m_CampaignData.bAlliesSelected));
	if (ImGui::Button(LOCAL("Button_Next")))
	{
		bNextButtonPressed = true;
	}
	ImGui::EndDisabled();

}

void NewCampaignWindow::OnUpdate(float deltaTime)
{

	if (bNextButtonPressed)
	{
		CurrentAction = EWindowActions::NEXT;
	}

	if (bCancelButtonPressed)
	{
		CurrentAction = EWindowActions::CLOSE;
	}
}

std::unique_ptr<CrosswindWindow> NewCampaignWindow::CreateNextWindow(std::shared_ptr<WindowContext> context)
{
	auto campaignContext = std::static_pointer_cast<CampaignSetupContext>(context);
	return std::make_unique<SelectSquadronWindow>(campaignContext);
}
