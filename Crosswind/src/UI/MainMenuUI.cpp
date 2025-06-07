#include "MainMenuUI.h"
#include <iostream>
#include "Localization/LocalizationManager.h"
#include "Core.h"
#include "tinyxml2/tinyxml2.h"
#include "Walnut/Image.h"
#include "MainMenu/MainMenuButtons.h"
#include "UI/MainMenu/NewCampaignWindow.h"
#include "UI/MainMenu/SquadronSelectionWindow.h"
#include "UI/MainMenu/CreateNewCampaignPopupWindow.h"
#include "UI/Windows/CreateNewPilotWindow.h"
#include "Core/CrosswindApp.h"
#include "UI/MainMenu/LoadCampaignWindow.h"

void MainMenuLayer::OnUIRender()
{
	CrosswindLayer::OnUIRender();

	ImGui::ShowDemoWindow();

}

void MainMenuLayer::OnAttach()
{
	m_CampaignSetupContext = std::make_shared<CampaignSetupContext>(CampaignSetup);
	m_NewPilotContext = std::make_shared<NewPilotContext>(CampaignSetup,NewPilot);

	PushWindow(std::make_unique<MainMenuButtons>());
}

void MainMenuLayer::OnUpdate(float DeltaTime)
{
	CrosswindLayer::OnUpdate(DeltaTime);

	for (size_t i = 0; i < m_Windows.size(); ++i) {
		CrosswindWindow* window = m_Windows[i].get();

		if (auto* mainMenuButtons = dynamic_cast<MainMenuButtons*>(m_Windows[i].get()))
		{
			mainMenuButtons->bShowButtons = bShowButtons;
			if (mainMenuButtons->bNewCampaignPressed)
			{
				PushWindow(std::make_unique<NewCampaignWindow>(m_CampaignSetupContext));
				bShowButtons = false;
				mainMenuButtons->bNewCampaignPressed = false;
			}
			if (mainMenuButtons->bLoadCampaignPressed)
			{
				PushWindow(std::make_unique<LoadCampaignWindow>());
				bShowButtons = false;
				mainMenuButtons->bLoadCampaignPressed = false;
			}
			if (mainMenuButtons->bSettingsPressed)
			{
				mainMenuButtons->bSettingsPressed = false;
			}
		}
		
		switch (window->GetRequestedAction()) {
		case EWindowActions::CLOSE:
			bShowButtons = true;
			RemoveWindow(window);
			break;

		case EWindowActions::NEXT: {
			std::unique_ptr<CrosswindWindow> next;
			auto* selectSquadronWindow = dynamic_cast<SelectSquadronWindow*>(m_Windows[i].get());
			auto* newCampaignWin = dynamic_cast<NewCampaignWindow*>(m_Windows[i].get());
			auto* newPilot = dynamic_cast<CreateNewPilotWindow*>(m_Windows[i].get());
			auto* createPopup = dynamic_cast<CreateNewCampaignPopupWindow*>(m_Windows[i].get());
			auto* LoadWindow = dynamic_cast<LoadCampaignWindow*>(m_Windows[i].get());
			if (newCampaignWin) {

				next = window->CreateNextWindow(m_CampaignSetupContext);
			}
			else if (selectSquadronWindow)
			{
				next = window->CreateNextWindow(m_NewPilotContext);
			}
			else if (newPilot)
			{
				next = window->CreateNextWindow(m_NewPilotContext);
			}
			else if (createPopup || LoadWindow)
			{
				Core::Instance().GetApp()->PushCampaignScreen();
			}
			else
			{
				next = window->CreateNextWindow(std::make_shared<WindowContext>());
			}
			if (next)
				m_Windows.push_back(std::move(next));
			RemoveWindow(window);
			break;
		}
		case EWindowActions::BACK: {
			std::unique_ptr<CrosswindWindow> prev;
			auto* newCampaignWin = dynamic_cast<NewCampaignWindow*>(m_Windows[i].get());
			auto* selectSquadronWindow = dynamic_cast<SelectSquadronWindow*>(m_Windows[i].get());
			auto* newPilot = dynamic_cast<CreateNewPilotWindow*>(m_Windows[i].get());
			if (newCampaignWin || selectSquadronWindow || newPilot) {

				prev = window->CreateBackWindow(m_CampaignSetupContext);
			}
			else
			{
				prev = window->CreateBackWindow(std::make_shared<WindowContext>());
			}
			if (prev)
				m_Windows.push_back(std::move(prev));
			RemoveWindow(window);
			break;
		}
		default:
			break;
		}
	}
}

