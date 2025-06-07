#pragma once

#include "Core.h"
#include "UI/WindowContexts/WindowContexts.h"


enum EWindowActions
{
	NONE,
	NEXT,
	BACK,
	CLOSE,
};

//The base class for crosswind GUI windows
class CrosswindWindow
{
public:
	CrosswindWindow(const std::string& name);

	virtual ~CrosswindWindow() = default;

	virtual void OnUpdate(float deltaTime) {} //logic updates
	virtual void OnRender();

	virtual std::unique_ptr<CrosswindWindow> CreateNextWindow(std::shared_ptr<WindowContext> context);
	virtual std::unique_ptr<CrosswindWindow> CreateBackWindow(std::shared_ptr<WindowContext> context);

	virtual void RenderContents() = 0;

	bool IsOpen();
	void Open();
	void Close();

	EWindowActions GetRequestedAction();

protected:
	std::string m_Name;
	bool m_Open;
	ImGuiWindowFlags m_WindowFlags = ImGuiWindowFlags_None;
	bool m_hasCloseButton = true;
	ImVec2 m_WindowSize = ImVec2(200, 200);
	ImVec2 m_WindowPos = ImVec2(0, 0);
	EWindowActions CurrentAction = NONE;
};