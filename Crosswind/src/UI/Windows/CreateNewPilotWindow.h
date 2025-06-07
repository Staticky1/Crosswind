#pragma once

#include "UI/CrosswindWindow.h"
#include "UI/WindowContexts/NewPilotContext.h"

class CreateNewPilotWindow : public CrosswindWindow
{
public:
	CreateNewPilotWindow(std::shared_ptr<WindowContext> context);

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	std::unique_ptr<CrosswindWindow> CreateNextWindow(std::shared_ptr<WindowContext> context) override;
	std::unique_ptr<CrosswindWindow> CreateBackWindow(std::shared_ptr<WindowContext> context) override;

	std::shared_ptr<NewPilotContext> m_Context;



private:


	int m_RandomBackgroundIndex = 0;
	bool isEditing = false;

	void InsertLineBreaks(std::string& text, int maxCharsPerLine = 80);
	void RemoveLineBreaks(std::string& text);

	std::string PilotDefaultName;
	char m_NameBuffer[64] = {};
	char m_BackgroundBuffer[1024] = {};
	char m_TempBackgroundBuffer[1024] = {};
	bool m_NameInitialized = false;
	bool m_BackgroundInitialized = false;

	CampaignSetupData& m_CampaignData;
	PilotData& m_PilotData;

	void ClearPilotImages();

	void SavePilotData();

	std::shared_ptr<Walnut::Image> RandomBackgroundButtonImage = nullptr;
	std::shared_ptr<Walnut::Image> RandomNameButtonImage = nullptr;
	std::string RandomImagePath = "data/images/icons/random.png";

	ImVec2 DesiredWindowSize = ImVec2(700.0f, 700.0f);

	int SelectedRankIndex = 0;


	std::vector<std::shared_ptr<Walnut::Image>> m_PilotImages;
	int m_SelectedImageIndex = 0;


};