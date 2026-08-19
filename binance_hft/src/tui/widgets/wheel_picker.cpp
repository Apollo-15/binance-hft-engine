#include "tui/widgets/wheel_picker.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>

ftxui::Element WheelPicker::OnRender()
{
	ftxui::Element leftElement;
	const ftxui::Element centerElement = ftxui::text((*Entries)[*Selected]);
	ftxui::Element rightElement;
	const bool isFocused = Focused();

	if (*Selected - 1 >= 0)
	{
		leftElement = ftxui::text((*Entries)[*Selected - 1]);
	}
	else
	{
		leftElement = ftxui::text(std::string(MaxSlotWidth, ' '));
	}

	if (*Selected + 1 <= static_cast<int>(Entries->size()) - 1)
	{
		rightElement = ftxui::text((*Entries)[*Selected + 1]);
	}
	else
	{
		rightElement = ftxui::text(std::string(MaxSlotWidth, ' '));
	}

	return ftxui::hbox({ftxui::dim(leftElement | ftxui::reflect(LeftBox)),
						   ftxui::separator(), ftxui::bold(centerElement), ftxui::separator(), 
		                   ftxui::dim(rightElement | ftxui::reflect(RightBox)) }) 
		| ftxui::border 
		| ftxui::color(isFocused ? ftxui::Color::White : ftxui::Color::GrayDark);
}

bool WheelPicker::OnEvent(ftxui::Event event)
{
	bool isHandled = false;

	if (event == ftxui::Event::ArrowLeft)
	{
		if (*Selected - 1 >= 0)
		{
			(*Selected)--;
		}
		isHandled = true;
	}

	if (event == ftxui::Event::ArrowRight)
	{
		if (*Selected + 1 <= static_cast<int>(Entries->size()) - 1)
		{
			(*Selected)++;
		}
		isHandled = true;
	}

	if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelUp && Focused())
	{
		if (*Selected - 1 >= 0)
		{
			(*Selected)--;
		}
		isHandled = true;
	}

	if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelDown && Focused())
	{
		if (*Selected + 1 <= static_cast<int>(Entries->size()) - 1)
		{
			(*Selected)++;
		}
		isHandled = true;
	}

	if (event.is_mouse() && 
		event.mouse().button == ftxui::Mouse::Left && 
		event.mouse().motion == ftxui::Mouse::Pressed &&
		LeftBox.Contain(event.mouse().x, event.mouse().y))
	{
		if (*Selected - 1 >= 0)
		{
			(*Selected)--;
		}
		TakeFocus();
		isHandled = true;
	}

	if (event.is_mouse() &&
		event.mouse().button == ftxui::Mouse::Left &&
		event.mouse().motion == ftxui::Mouse::Pressed &&
		RightBox.Contain(event.mouse().x, event.mouse().y))
	{
		if (*Selected + 1 <= static_cast<int>(Entries->size()) - 1)
		{
			(*Selected)++;
		}
		TakeFocus();
		isHandled = true;
	}

	return isHandled;
}

bool WheelPicker::Focusable() const
{
	return true;
}

ftxui::Component MakeWheelPicker(std::vector<std::string>* const entries, int* const selected)
{
	return ftxui::Make<WheelPicker>(entries, selected);
}
