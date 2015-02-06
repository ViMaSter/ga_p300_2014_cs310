#include "Button.h"

Button::Button(RECT position, IImage* backgroundImage, std::string textContent)
{
	Position = position;
	BackgroundImage = backgroundImage;
	TextContent = textContent;

	ChangeState(Button::State::NORMAL);
}

Button::Button() {

}

Button::~Button() {

}

bool Button::InButton(int mouseX, int mouseY) {
	return (mouseX > Position.left && mouseX < Position.left+Position.right && mouseY > Position.top && mouseY < Position.top + Position.bottom);
}

void Button::UpdateStateByMouseInput(int mouseX, int mouseY, bool clicked)
{
	if ( InButton(mouseX, mouseY ) && clicked ) {
		Button::ChangeState( Button::State::ACTIVE );
	}
	else if ( InButton(mouseX, mouseY) && !clicked ) {
		Button::ChangeState( Button::State::HOVER );
	}
	else if ( !InButton(mouseX, mouseY) && !clicked ) {
		Button::ChangeState( Button::State::NORMAL );
	}
}

void Button::Draw(IScreenRenderer* renderer) {
	renderer->DrawImage(BackgroundImage, (const RECT*)&Position, (const RECT*)&BackgroundImageSize);

	RECT textRect = RECT{ Position.left, Position.top, Position.left+Position.right, Position.top+Position.bottom };

	renderer->DrawText(&textRect, TextContent.c_str());
	return;
}

void Button::ChangeState(State newState)
{
	CurrentState = newState;

	// Move background image according to the new button state
	BackgroundImageSize = RECT{
		0,
		(Position.bottom) * newState,
		Position.right,
		Position.bottom
	};
}
