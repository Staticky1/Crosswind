#pragma once

#include <string>
#include <imgui.h>
#include <vector>
#include <memory>
#include "Walnut/Image.h"

struct ListItem {
    std::shared_ptr<Walnut::Image> m_iconTexture = nullptr;
    std::string texturePath;
    std::string label;
    std::string id;
};


//class containning static helper functions for UI
class CrosswindUI {
public:
    // Draws a centered label
    static void CenterText(const std::string& text);

    // Draws an image+label row inside a selectable
    static bool ImageSelectableListBox(const std::string& label, std::vector<ListItem>& ListItems, int& SelectedIndex ,ImVec2 size = ImVec2(32, 32), ImVec2 ImageSize = ImVec2(32,32), float itemHeight = 40.0f);

    // Draws a vertical spacer
    static void Spacer(float height = 10.0f);

    //Safely remove listitems
    static void ClearListItem(ListItem& ItemToClear);
};