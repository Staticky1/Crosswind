#include "SquadronSelectionWindow.h"
#include <filesystem>
#include "UI/MainMenu/NewCampaignWindow.h"
#include "UI/WindowContexts/NewPilotContext.h"
#include "UI/Windows/CreateNewPilotWindow.h"

SelectSquadronWindow::SelectSquadronWindow(std::shared_ptr<WindowContext> context)
	:CrosswindWindow::CrosswindWindow(LOCAL("NewCampaign_Title"))
	, m_Context(std::static_pointer_cast<CampaignSetupContext>(context))
	, m_CampaignData(m_Context->campaignData)
{
	m_WindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x, ImGui::GetMainViewport()->WorkSize.y - 100);
	m_hasCloseButton = false;
	//place center screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));
}

void SelectSquadronWindow::RenderContents()
{
	m_WindowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	m_WindowSize = ImVec2(ImGui::GetMainViewport()->WorkSize.x, ImGui::GetMainViewport()->WorkSize.y - 100);

	//place center screen
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_WindowPos = (ImVec2(main_viewport->WorkPos.x + (main_viewport->WorkSize.x / 2) - (m_WindowSize.x / 2), main_viewport->WorkPos.y + (main_viewport->WorkSize.y / 2) - (m_WindowSize.y / 2)));

	if (!m_Initilized) return;

	//Set map frontlines
	if (m_CampaignData.LoadedTheater.LoadedFrontlines.size() == 0)
	{
		m_CampaignData.LoadedTheater.LoadFrontLines(m_CampaignData.StartDate);
		MapWidget.UpdateFrontlines(m_CampaignData.LoadedTheater.LoadedFrontlines);
	}


	MapWidget.Draw("CampaignMap", ImVec2(ImGui::GetContentRegionAvail().x - 350, ImGui::GetContentRegionAvail().y));

	ImGui::SameLine();

	ImGui::BeginChild("Squardrons", ImVec2(350, ImGui::GetContentRegionAvail().y));
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(LOCAL("NewCampaign_AirService")).x) * 0.5f);
	ImGui::Text(LOCAL("NewCampaign_AirService"));


	//service selection box





	std::string ServicesSelectionBoxId = "##ServicesSelection";

	for (int i = 0; i < ServiceItems.size(); ++i)
	{
		if (!ServiceItems[i].m_iconTexture && Core::ApplicationIsVisible()) //load icon texture into memory
		{
			ServiceItems[i].m_iconTexture = std::make_shared<Walnut::Image>(ServiceItems[i].texturePath);
		}
	}

	CrosswindUI::ImageSelectableListBox(ServicesSelectionBoxId, ServiceItems, SelectedServiceIndex, ImVec2(ImGui::GetContentRegionAvail().x, 200), ImVec2(54, 32), 40.0f);



	ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(LOCAL("NewCampaign_Squadron")).x) * 0.5f);
	ImGui::Text(LOCAL("NewCampaign_Squadron"));



	//New Service Selected
	if (SelectedServiceIndex != Prev_SelectedServiceIndex)
	{
		SelectedSquadronIndex = 0;

		AllSquadrons.clear();

		for (int i = 0; i < m_CampaignData.LoadedTheater.AllSquadrons.size(); ++i) {

			if ((m_CampaignData.LoadedTheater.AllSquadrons[i].side == "Allies" && m_CampaignData.bAlliesSelected) || (m_CampaignData.LoadedTheater.AllSquadrons[i].side == "Axis" && m_CampaignData.bAxisSelected))
			{

				if (ServiceItems[SelectedServiceIndex].id == m_CampaignData.LoadedTheater.AllSquadrons[i].service)
				{
					ListItem NewListItem;
					NewListItem.id = m_CampaignData.LoadedTheater.AllSquadrons[i].id;
					NewListItem.label = m_CampaignData.LoadedTheater.AllSquadrons[i].name;
					NewListItem.texturePath = m_CampaignData.LoadedTheater.AllSquadrons[i].iconPath;

					AllSquadrons.push_back(NewListItem);
				}

			}

		}

		//build airfield widget map
		std::unordered_map<std::string, AirfieldWidget> airfieldWidgets;

		for (int i = 0; i < AllSquadrons.size(); i++) {

			const Squadron& squadron = m_CampaignData.LoadedTheater.GetSquadronsFromID(AllSquadrons[i].id);

			const auto* AirfieldValue = Core::GetActiveEntryRef<ValueStartDate>(
				squadron.locations,
				m_CampaignData.StartDate,
				[](const ValueStartDate& e) { return e.startDate; }
			);
			std::string airfieldName = AirfieldValue->value;

			// Load icon (if not already done)
			auto icon = std::make_shared<Walnut::Image>(squadron.iconPath); // returns shared_ptr<Walnut::Image>

			if (!airfieldWidgets.contains(airfieldName)) {
				AirfieldWidget widget;
				widget.mapPos = MapWidget.WorldPosToMapPos(m_CampaignData.LoadedTheater.GetAirfields(m_CampaignData.StartDate)->LoadedAirfields.at(airfieldName).position);
				airfieldWidgets[airfieldName] = widget;
			}

			airfieldWidgets[airfieldName].squadronIcons.emplace_back(std::pair<std::shared_ptr<Walnut::Image>, const Squadron*>(icon, &squadron));
		}

		//add airfield widget items
		MapWidget.ClearItems();



		for (auto& [airfieldName, widget] : airfieldWidgets)
		{

			std::function<void(const Squadron*)> onSquadronClicked = [this](const Squadron* sq)
				{
					if (sq)
					{
						SelectSquadron(sq); // Call squadron selection logic
					}
				};

			MapWidget.AddItem(
				{
					.mapPosition = widget.mapPos,
					.widgetSize = ImVec2(64,64),
					.renderWidget = [icons = widget.squadronIcons, onClick = onSquadronClicked](ImVec2 screenPos, ImVec2 size)
					{
						ImVec2 cursorStart = screenPos;

						int iconCount = static_cast<int>(icons.size());
						float spacing = 4.0f;
						float totalWidth = iconCount * size.x + (iconCount - 1) * spacing;
						float totalHeight = size.y;

						ImVec2 bgMin = ImVec2(cursorStart.x - spacing, cursorStart.y - spacing);
						ImVec2 bgMax = ImVec2(cursorStart.x + totalWidth + spacing, cursorStart.y + totalHeight + spacing);

						auto* drawList = ImGui::GetWindowDrawList();

						// Draw background
						drawList->AddRectFilled(bgMin, bgMax, IM_COL32(0, 0, 0, 150), 4.0f); // rounded black background
						drawList->AddRect(bgMin, bgMax, IM_COL32(255, 255, 255, 100), 4.0f); // light border

						for (size_t i = 0; i < icons.size(); ++i)
						{
							const auto& [image, squadron] = icons[i];
							ImVec2 iconPos = {
								cursorStart.x + i * (size.x + spacing),
								cursorStart.y
							};
							ImGui::SetCursorScreenPos(iconPos);

							ImGui::PushID((void*)squadron);
							if (ImGui::InvisibleButton("##SquadronIcon", size))
							{
								if (onClick) onClick(squadron);
							}
							if (ImGui::IsItemHovered() && squadron)
								ImGui::SetTooltip(squadron->name.c_str());
							ImGui::SetCursorScreenPos(ImGui::GetItemRectMin());

							ImGui::Image(image->GetDescriptorSet(), size);
							ImGui::PopID();
						}
					}
				});
		}

		Prev_SelectedServiceIndex = SelectedServiceIndex;
	}

	std::string squadronSelectionBoxId = "##SquadronSelection";

	for (int i = 0; i < AllSquadrons.size(); ++i)
	{
		if (!AllSquadrons[i].m_iconTexture && Core::ApplicationIsVisible()) //load icon texture into memory
		{
			AllSquadrons[i].m_iconTexture = std::make_shared<Walnut::Image>(AllSquadrons[i].texturePath);
		}
	}

	CrosswindUI::ImageSelectableListBox(squadronSelectionBoxId, AllSquadrons, SelectedSquadronIndex, ImVec2(ImGui::GetContentRegionAvail().x, 200), ImVec2(32, 32), 32.0f);

	Squadron SelectedSquadron = m_CampaignData.LoadedTheater.GetSquadronsFromID(AllSquadrons[SelectedSquadronIndex].id);

	//New Squadron Selected
	if (Prev_SelectedSquadronID != SelectedSquadron.id)
	{
		Squadron CurrentSquadron = m_CampaignData.LoadedTheater.GetSquadronsFromID(AllSquadrons[SelectedSquadronIndex].id);

		const auto* AirfieldValue = Core::GetActiveEntryRef<ValueStartDate>(
			CurrentSquadron.locations,
			m_CampaignData.StartDate,
			[](const ValueStartDate& e) { return e.startDate; }
		);

		Airfield SquadronAirfield = m_CampaignData.LoadedTheater.GetAirfields(m_CampaignData.StartDate)->LoadedAirfields.at(AirfieldValue->value);
		ImVec2 SquadronLocation = MapWidget.WorldPosToMapPos(SquadronAirfield.position);

		MapWidget.LerpToMapPosition(SquadronLocation, 5.0f);


		Prev_SelectedSquadronID = SelectedSquadron.id;
	}


	//Squadron Info Box

	ImGui::Text(LOCAL("SquadronType"));

	ImGui::Text(SelectedSquadron.type.c_str());

	ImGui::Text(LOCAL("SquadronAircraft"));

	const auto* AircraftValue = Core::GetActiveEntryRef<ValueStartDate>(
		SelectedSquadron.aircraft,
		m_CampaignData.StartDate,
		[](const ValueStartDate& e) { return e.startDate; }
	);

	ImGui::Text(AircraftValue->value.c_str());

	ImGui::Text(LOCAL("SquadronAirfield"));

	const auto* AirfieldValue = Core::GetActiveEntryRef<ValueStartDate>(
		SelectedSquadron.locations,
		m_CampaignData.StartDate,
		[](const ValueStartDate& e) { return e.startDate; }
	);

	ImGui::Text(AirfieldValue->value.c_str());


	ImGui::SetCursorPosY(ImGui::GetMainViewport()->WorkSize.y - 200);
	if (ImGui::Button(LOCAL("Button_Back")))
	{
		CurrentAction = EWindowActions::BACK;
	}
	ImGui::SameLine();

	if (ImGui::Button(LOCAL("Button_Next")))
	{
		m_CampaignData.FinalSelectedService = m_CampaignData.LoadedTheater.GetServiceFromID(ServiceItems[SelectedServiceIndex].id);
		m_CampaignData.FinalSelectedSquadron = m_CampaignData.LoadedTheater.GetSquadronsFromID(AllSquadrons[SelectedSquadronIndex].id);
		CurrentAction = EWindowActions::NEXT;
	}

	ImGui::EndChild();
}

void SelectSquadronWindow::OnUpdate(float deltaTime)
{
	//Set map texture
	bool fileExists = std::filesystem::exists(m_CampaignData.LoadedTheater.MapTexturePath);
	if (!fileExists)
	{
		std::cerr << "Map texture not found." << std::endl;
	}
	if (!MapWidget.bMapTextureSet) //dont want to be setting the texture on every frame
	{
		MapWidget.SetMapTexture(m_CampaignData.LoadedTheater.MapTexturePath, m_CampaignData.LoadedTheater.MapTextureSize, m_CampaignData.LoadedTheater.MapWorldSize);
	}


	
	//Build Service Items
	if (ServiceItems.size() == 0)
	{
		for (int i = 0; i < m_CampaignData.LoadedTheater.Services.size(); ++i)
		{
			if ((m_CampaignData.LoadedTheater.Services[i].side == "Allies" && m_CampaignData.bAlliesSelected) || (m_CampaignData.LoadedTheater.Services[i].side == "Axis" && m_CampaignData.bAxisSelected))
			{
				ListItem NewItem;

				std::string prefix = "AirService_";

				NewItem.label = LOCAL(prefix.append(m_CampaignData.LoadedTheater.Services[i].id));

				NewItem.texturePath = m_CampaignData.LoadedTheater.Services[i].icon;

				NewItem.id = m_CampaignData.LoadedTheater.Services[i].id;

				ServiceItems.push_back(NewItem);
			}
		}
	}
	
	//build squadron list

	if (m_CampaignData.LoadedTheater.AllSquadrons.size() == 0)
	{
		m_CampaignData.LoadedTheater.LoadSquadronsFromTheater();
	}

	//load airfield data
	if (m_CampaignData.LoadedTheater.GetAirfields(m_CampaignData.StartDate)->LoadedAirfields.size() == 0)
	{
		m_CampaignData.LoadedTheater.LoadAirfields(m_CampaignData.StartDate);
	}

	m_Initilized = true;
}

std::unique_ptr<CrosswindWindow> SelectSquadronWindow::CreateNextWindow(std::shared_ptr<WindowContext> context)
{
	auto newPilotContext = std::static_pointer_cast<NewPilotContext>(context);
	return std::make_unique<CreateNewPilotWindow>(newPilotContext);
}

std::unique_ptr<CrosswindWindow> SelectSquadronWindow::CreateBackWindow(std::shared_ptr<WindowContext> context)
{
	auto campaignContext = std::static_pointer_cast<CampaignSetupContext>(context);
	return std::make_unique<NewCampaignWindow>(campaignContext);
}

void SelectSquadronWindow::SelectSquadron(const Squadron* selectedSquadron)
{
	std::string squadronId = selectedSquadron->id;

	for (int i = 0; i < AllSquadrons.size(); ++i)
	{
		if (AllSquadrons[i].id == squadronId)
		{
			SelectedSquadronIndex = i;
		}
	}
}
