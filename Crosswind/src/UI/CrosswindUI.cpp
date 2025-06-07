#include "CrosswindUI.h"
#include "Core.h"

void CrosswindUI::CenterText(const std::string& text)
{
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth = ImGui::CalcTextSize(text.c_str()).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::TextUnformatted(text.c_str());
}

bool CrosswindUI::ImageSelectableListBox(const std::string& label, std::vector<ListItem>& ListItems, int& SelectedIndex, ImVec2 size, ImVec2 imageSize, float itemHeight)
{
	if (!ImGui::BeginListBox(label.c_str(), size))
	{
		return false;
	}

	for (int i = 0; i < ListItems.size(); ++i) {
		ImGui::PushID(i);

		bool isSelected = (SelectedIndex == i);
		if (ImGui::Selectable("##item", isSelected, 0, ImVec2(0, itemHeight))) {
			SelectedIndex = i;
		}


		ImVec2 itemPos = ImGui::GetItemRectMin();
		ImVec2 itemSize = ImGui::GetItemRectSize();

		// Y center: (Item Y + (Item Height - Image Height) / 2)
		float imageY = itemPos.y + (itemSize.y - imageSize.y) * 0.5f;
		float imageX = itemPos.x + 4;
		if (ListItems[i].m_iconTexture && ListItems[i].m_iconTexture->GetDescriptorSet())
		{
			ImGui::GetWindowDrawList()->AddImage(
				ListItems[i].m_iconTexture->GetDescriptorSet(),
				ImVec2(imageX, imageY),
				ImVec2(imageX + imageSize.x, imageY + imageSize.y)
			);
		}

		ImGui::GetWindowDrawList()->AddRect(
			ImVec2(imageX, imageY),
			ImVec2(imageX + imageSize.x, imageY + imageSize.y),
			IM_COL32(255, 0, 0, 255)
		);

		// Align text next to image, also vertically centered
		float textY = itemPos.y + (itemSize.y - ImGui::GetTextLineHeight()) * 0.5f;
		ImGui::SetCursorScreenPos(ImVec2(imageX + imageSize.x + 8, textY));
		ImGui::TextUnformatted(ListItems[i].label.c_str());

		ImGui::PopID();

		ImGui::Dummy(ImVec2(0, (itemHeight - imageSize.y) * 0.5f));

	}

	ImGui::EndListBox();

	return true;
}

void CrosswindUI::Spacer(float height)
{
	ImGui::Dummy(ImVec2(0, height));
}

void CrosswindUI::ClearListItem(ListItem& ItemToClear)
{
	ItemToClear.id = "";
	ItemToClear.label = "";
	ItemToClear.texturePath = "";
	TextureGarbageCollector::Submit(ItemToClear.m_iconTexture);
}
