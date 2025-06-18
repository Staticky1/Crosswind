#pragma once

#include "UI/WindowContexts/CampaignContext.h"

struct NewsItem
{
	std::string newsHeading;
	std::string newsText;
	std::string newsImagePath;
	std::shared_ptr<Walnut::Image> newsImage = nullptr;
	std::function<void()> customRender = nullptr;  // Optional custom content
	NewsItem() = default;
};

class NewsPopupWindow
{
public:
	NewsPopupWindow();

	void AddNewsItem(std::string Heading, std::string Text = "", std::string ImagePath = "", std::function<void()> customContent = nullptr);
	void ClearNewsItems();

	void ShowNewsCarouselModal(const std::vector<NewsItem>& items, int& currentIndex, bool& isOpen);

private:

	std::vector<NewsItem> newsItems;

	


	int selectedItemIndex = 0;
};