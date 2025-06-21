#pragma once

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"
#include <vector>
#include <memory>     
#include <utility> 
#include <functional>
#include "Core.h"
#include "Core/Theater.h"

struct MapItem {
    std::string id;
    ImVec2 mapPosition;                         // Coordinates in map space
    ImVec2 widgetSize;
    float maxSize = 1.0f;
    float minSize = 0.25f;
    std::function<void(ImVec2 screenPos, ImVec2 scaledSize)> renderWidget;       // Lambda that renders UI
};

struct AirfieldWidget {
    ImVec2 mapPos;
    std::vector<std::pair<std::shared_ptr<Walnut::Image>, const Squadron*>> squadronIcons;
};

class CampaignMapWidget {
public:
    void SetMapTexture(std::string texturePath, ImVec2 size, ImVec2 InWorldSize);
    void AddItem(const MapItem& item);
    void ClearItems();

    bool bMapTextureSet = false;

    void Draw(const char* id, ImVec2 size);

    ImVec2 WorldPosToMapPos(Vec3 WorldPos);

    void LerpToMapPosition(ImVec2 mapTarget, float speed = 5.0f);

    Vec3 GetCursorWorldPosition() const;

    void UpdateFrontlines(std::vector<Frontlines> NewFrontlines);
    bool HasFrontlines();
    void ClearMapTexture();

private:
    void ClampPanOffset();
    ImVec2 GetClampedZoomSize(float baseSize, float zoom, float minSize, float maxSize);

    void LerpScrollOffset();


    std::shared_ptr<Walnut::Image> m_MapImage;

    ImTextureID mapTexture = nullptr;
    ImVec2 mapSize = { 0, 0 };
    ImVec2 WorldSize = { 0, 0 };
    std::vector<MapItem> items;

    ImVec2 m_WidgetSize;
    float m_OverscrollMargin = 100.0f;

    // Viewport state
    ImVec2 scroll = { 0, 0 };
    float zoom = 1.0f;
    float prevZoom = 1.0f;
    bool isDragging = false;
    ImVec2 dragStart = { 0, 0 };

    ImVec2 LerpTarget;
    float LerpSpeed = 0.0f;
    bool bIsLerping = false;
    float LerpSnapDistance = 2.0f;

    ImVec2 mapScreenOrigin;

    std::vector<Frontlines> CurrentFrontlines;
};