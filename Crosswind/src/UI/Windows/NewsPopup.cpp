#include "NewsPopup.h"
#include "Core/CrosswindApp.h"

NewsPopupWindow::NewsPopupWindow()
{

}

void NewsPopupWindow::AddNewsItem(std::string Heading, std::string Text, std::string ImagePath, std::function<void()> customContent)
{
	NewsItem newItem;
	newItem.newsHeading = Heading;
	newItem.newsText = Text;
	newItem.newsImagePath = ImagePath;
    if (ImagePath != "")
	newItem.newsImage = std::make_shared<Walnut::Image>(ImagePath);
    if (customContent)
        newItem.customRender = customContent;
	newsItems.push_back(newItem);
	
}

void NewsPopupWindow::ClearNewsItems()
{
    newsItems.clear();
}

void NewsPopupWindow::ShowNewsCarouselModal(const std::vector<NewsItem>& items, int& currentIndex, bool& isOpen)
{
    if (!isOpen || items.empty()) return;

    // Must be called from somewhere before rendering
    ImGui::OpenPopup("NewsModal");

    // Get main viewport (usually the application window)
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImVec2 m_WindowSize = ImVec2(viewport->Size.x * 0.4f, viewport->Size.y * 0.6f);

    // Calculate center position
    ImVec2 popupPos = ImVec2(
        viewport->Pos.x + (viewport->Size.x - m_WindowSize.x) * 0.5f,
        viewport->Pos.y + (viewport->Size.y - m_WindowSize.y) * 0.5f
    );

    ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);    // Or ImGuiCond_Once
    ImGui::SetNextWindowSize(m_WindowSize, ImGuiCond_Always);  // Or ImGuiCond_Once

    if (ImGui::BeginPopupModal("NewsModal", &isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
    {
        const int totalItems = static_cast<int>(items.size());

        ImGui::PushID("NewsCarouselModal");



        ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize("Updates:").x) * 0.5f);
        ImGui::Text("Updates:");
        ImGui::Separator();
 

        ImGui::SameLine();

        // Show image + text
        ImGui::BeginGroup();
        const NewsItem& item = items[currentIndex];

        if (item.newsImage)
        {
            float ImageWidth = m_WindowSize.x * 0.8f;
            float aspectRatio = (float)item.newsImage->GetHeight() / item.newsImage->GetWidth();
            ImVec2 imageSize = ImVec2(ImageWidth, ImageWidth * aspectRatio);

            ImGui::SetCursorPosX((m_WindowSize.x - imageSize.x) * 0.5f);
            ImGui::Image((ImTextureID)item.newsImage->GetDescriptorSet(), imageSize);
        }
        if (item.newsHeading != "")
        {
            ImGui::PushFont(Core::Instance().GetApp()->GetFont("Heading"));
            ImGui::SetCursorPosX((m_WindowSize.x - ImGui::CalcTextSize(item.newsHeading.c_str()).x) * 0.5f);
            ImGui::Text("%s", item.newsHeading.c_str());
            ImGui::PopFont();
        }
        if (item.newsText != "")
            ImGui::TextWrapped("%s", item.newsText.c_str());
        if (item.customRender) {
            item.customRender(); // Render custom content
        }

        ImGui::EndGroup();



        // Dots
        if (totalItems > 1)
        {


            ImGui::Spacing();
            ImGui::SameLine();
            if (ImGui::ArrowButton("##Left", ImGuiDir_Left)) {
                currentIndex = (currentIndex - 1 + totalItems) % totalItems;
            }
            ImGui::SameLine();



            ImGui::SetCursorPosX((m_WindowSize.x - (totalItems * (ImGui::CalcTextSize("-").x + ImGui::GetStyle().ItemSpacing.x))) * 0.5f);
            for (int i = 0; i < totalItems; ++i)
            {
                if (i == currentIndex)
                    ImGui::TextColored(ImVec4(1, 1, 1, 1), "-");
                else
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), "-");

                ImGui::SameLine();
            }

            ImGui::SameLine();
            ImGui::SetCursorPosX(m_WindowSize.x - 40);

            if (ImGui::ArrowButton("##Right", ImGuiDir_Right)) {
                currentIndex = (currentIndex + 1) % totalItems;
            }
        }
        ImGui::SetCursorPosX((m_WindowSize.x - (ImGui::CalcTextSize("Close").x + ImGui::GetStyle().FramePadding.x * 2)) * 0.5f);
        if (ImGui::Button("Close")) {
            isOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopID();
        ImGui::EndPopup();
    }
}