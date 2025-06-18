#pragma once

#include "UI/CrosswindWindow.h"
#include "UI/WindowContexts/CampaignContext.h"

class SquadronInfoWindow : public CrosswindWindow
{
public:
	SquadronInfoWindow(std::shared_ptr<CampaignContext> context);

	virtual void RenderContents() override;

	virtual void OnUpdate(float deltaTime) override;

	std::shared_ptr<CampaignContext> m_Context;

private:

	void DrawPilotTable(std::vector<PilotData>& pilots, EPilotStatus statusFilter, std::string tableId, bool bShowWoundedDaysColumn = false);

	void DrawAircraftTable(const std::vector<SquadronAircraft>& aircraftList, ESquadronAircraftStatus filterStatus, std::string tableId, bool showMaintDuration = false);

	int& m_SelectedPlayerPilotIndex;

	std::shared_ptr<Walnut::Image> m_SquadronLogo = nullptr;

	CampaignData& m_CampaignData;
};