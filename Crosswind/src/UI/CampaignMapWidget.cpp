#include "CampaignMapWidget.h"
#include <algorithm>
#include "tinyxml2/tinyxml2.h"
#include "Core.h"

void CampaignMapWidget::SetMapTexture(std::string texturePath, ImVec2 size, ImVec2 InWorldSize)
{
    m_MapImage = std::make_shared<Walnut::Image>(texturePath);
    
    mapTexture = m_MapImage->GetDescriptorSet();
	mapSize = size;
    WorldSize = InWorldSize;
    scroll.x = m_WidgetSize.x * 0.5f;
    scroll.y = m_WidgetSize.y * 0.5f;

    bMapTextureSet = true;
}

void CampaignMapWidget::AddItem(const MapItem& item) {
	items.push_back(item);
}

void CampaignMapWidget::ClearItems() {
	items.clear();
}

void CampaignMapWidget::Draw(const char* id, ImVec2 size) {
    ImGui::BeginChild(id, size, true, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    m_WidgetSize = size;

    // Panning
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 mousePos = ImGui::GetMousePos();

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
    {
        Vec3 ClickedPos = GetCursorWorldPosition();
        std::cout << "<point x=" << ClickedPos.x << " y=0.0 z=" << ClickedPos.z << " />" << std::endl;
    }
    if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
        bIsLerping = false;
        ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        scroll.x += dragDelta.x;
        scroll.y += dragDelta.y;
        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
    }

    // Zooming
    if (ImGui::IsWindowHovered()) {
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            float zoomDelta = wheel * 0.1f;
            zoom = ImClamp(zoom + zoomDelta, 0.2f, 3.0f);

            ImVec2 widgetPos = ImGui::GetCursorScreenPos();
            ImVec2 cursorMapPos = ImVec2(mousePos.x - widgetPos.x, mousePos.y - widgetPos.y);

            //center map on mouse
            scroll.x = (scroll.x - cursorMapPos.x) * (zoom / prevZoom) + cursorMapPos.x;
            scroll.y = (scroll.y - cursorMapPos.y) * (zoom / prevZoom) + cursorMapPos.y;

            prevZoom = zoom;
        }

    }
    if (bIsLerping)
    {
        LerpScrollOffset();
    }


    ClampPanOffset();

    // Draw map
    ImVec2 displaySize = { mapSize.x * zoom, mapSize.y * zoom };
    ImVec2 topLeft = { canvasPos.x + scroll.x, canvasPos.y + scroll.y };
    ImVec2 bottomRight = { topLeft.x + displaySize.x, topLeft.y + displaySize.y };
    ImGui::GetWindowDrawList()->AddImage(mapTexture, topLeft, bottomRight);
    mapScreenOrigin = ImGui::GetCursorScreenPos();

    //frontlines
    for (int i = 0; i < CurrentFrontlines.size(); ++i)
    {
        std::vector<ImVec2> ScreenPoints;
        for (const Vec3& worldPoint : CurrentFrontlines[i].points) {
            ImVec2 mapPoint = Core::WorldToMap(worldPoint, WorldSize, mapSize);
            ImVec2 screenPos = ImVec2(
                topLeft.x + mapPoint.x * zoom,
                topLeft.y + mapPoint.y * zoom
            );
            ScreenPoints.push_back(screenPos);
        }

        ImU32 LineColour = IM_COL32(0, 0, 0, 100); //default black line
        if (CurrentFrontlines[i].side == "Axis")
        {
            LineColour = IM_COL32(0, 0, 255, 100);
        }
        else if (CurrentFrontlines[i].side == "Allies")
        {
            LineColour = IM_COL32(255, 0, 0, 100);
        }

        ImGui::GetWindowDrawList()->AddPolyline(
            ScreenPoints.data(),
            ScreenPoints.size(),
            LineColour, //line colour
            CurrentFrontlines[i].bIsPocket, //close path if pocket
            4.0f
        );
        
    }



    // Draw map widgets
    for (const auto& item : items) {
        ImVec2 widgetSize = ImVec2(
            std::clamp(item.widgetSize.x * zoom, item.minSize * item.widgetSize.x, item.maxSize * item.widgetSize.x),
            std::clamp(item.widgetSize.y * zoom, item.minSize * item.widgetSize.y, item.maxSize * item.widgetSize.y)
        );

        ImVec2 screenPos = {
            (topLeft.x + item.mapPosition.x * zoom) - widgetSize.x * 0.5f,
            (topLeft.y + item.mapPosition.y * zoom) - widgetSize.y * 0.5f
        };

        ImGui::SetCursorScreenPos(screenPos);
        item.renderWidget(screenPos, widgetSize);
    }

    ImGui::EndChild();
}

ImVec2 CampaignMapWidget::WorldPosToMapPos(Vec3 WorldPos)
{
    return Core::WorldToMap(WorldPos, WorldSize, mapSize);
}

void CampaignMapWidget::LerpToMapPosition(ImVec2 mapTarget, float speed)
{

    LerpTarget = mapTarget;
    LerpSpeed = speed;
    bIsLerping = true;
   
}

Vec3 CampaignMapWidget::GetCursorWorldPosition() const
{
    ImVec2 mouseScreenPos = ImGui::GetIO().MousePos;

    // Convert screen space to local map (texture) space
    ImVec2 localMapPos = ImVec2(
        (mouseScreenPos.x - mapScreenOrigin.x - scroll.x) / zoom,
        (mouseScreenPos.y - mapScreenOrigin.y - scroll.y) / zoom
    );

    // 3. Convert map position to world coordinates
    return Core::MapToWorld(localMapPos, WorldSize, mapSize);
}

void CampaignMapWidget::UpdateFrontlines(std::vector<Frontlines> NewFrontlines)
{
    CurrentFrontlines = NewFrontlines;
}

bool CampaignMapWidget::HasFrontlines()
{
    return CurrentFrontlines.size() > 0;
}

void CampaignMapWidget::ClearMapTexture()
{
    bMapTextureSet = false;
    TextureGarbageCollector::Submit(m_MapImage);
}

void CampaignMapWidget::ClampPanOffset()
{
    ImVec2 MapSizePx = {
    mapSize.x* zoom,
    mapSize.y* zoom
    };

    ImVec2 WidgetSize = m_WidgetSize;
    // Calculate bounds
    float maxOffsetX = m_OverscrollMargin;
    float maxOffsetY = m_OverscrollMargin;

    float minOffsetX = std::min(0.0f, WidgetSize.x - MapSizePx.x) - m_OverscrollMargin;
    float minOffsetY = std::min(0.0f, WidgetSize.y - MapSizePx.y) - m_OverscrollMargin;

    scroll.x = std::clamp(scroll.x, minOffsetX, maxOffsetX);
    scroll.y = std::clamp(scroll.y, minOffsetY, maxOffsetY);
}

ImVec2 CampaignMapWidget::GetClampedZoomSize(float baseSize, float zoom, float minSize, float maxSize)
{
    float scaled = baseSize * zoom;
    float clamped = std::clamp(scaled, minSize, maxSize);
    return ImVec2(clamped, clamped);
}

void CampaignMapWidget::LerpScrollOffset()
{
    ImVec2 screenTarget = {
    -LerpTarget.x * zoom + m_WidgetSize.x * 0.5f,
    -LerpTarget.y * zoom + m_WidgetSize.y * 0.5f
    };

    // Distance between current and target scroll
    float dist = ImLengthSqr(ImVec2(scroll.x - screenTarget.x , scroll.y - screenTarget.y));

    if (dist < LerpSnapDistance * LerpSnapDistance)
    {
        scroll = screenTarget;
        bIsLerping = false;
    }

    float t = 1.0f - std::exp(-LerpSpeed * ImGui::GetIO().DeltaTime);
    scroll = Core::Lerp(scroll, screenTarget, std::clamp(t, 0.0f, 1.0f));
}