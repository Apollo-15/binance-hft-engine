#pragma once

#include <ftxui/screen/color.hpp>

inline ftxui::Color RestStatusToColor(unsigned const int resultCode)
{
	if (resultCode == 0)
	{
		return ftxui::Color::GrayLight;
	}
	else if (resultCode == 200)
	{
		return ftxui::Color::Green;
	}
	else if (resultCode == 429 || resultCode == 418)
	{
		return ftxui::Color::Orange1;
	}
	else if (resultCode >= 400 && resultCode < 500)
	{
		return ftxui::Color::Red;
	}
	else if (resultCode >= 500 && resultCode < 600)
	{
		return ftxui::Color::Red;
	}
	else
	{
		return ftxui::Color::Red;
	}
}