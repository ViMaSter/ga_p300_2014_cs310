#pragma once
#include "main.h"
#include "IScreenRenderer.h"
#include "IImage.h"
#include <string>
class Button
{
public:
	enum State {
		NORMAL = 0,
		HOVER = 1,
		ACTIVE = 2
	};

	RECT Position = RECT{ 0, 0, 0, 0 };
	RECT BackgroundImageSize = RECT{ 0, 0, 0, 0 };

	State OldState = Button::State::NORMAL;
	State CurrentState = Button::State::NORMAL;

	IImage* BackgroundImage;
	std::string TextContent;

	Button();
	Button(RECT position, IImage* backgroundImage, std::string textContent);
	~Button();

	void Draw(IScreenRenderer* renderer);
	bool InButton(int mouseX, int mouseY);
	void UpdateStateByMouseInput(int mouseX, int mouseY, bool clicked);
	void ChangeState(State newState);
};

