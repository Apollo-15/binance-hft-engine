#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <ftxui/component/component_base.hpp>
#include <ftxui/screen/box.hpp>

class WheelPicker : public ftxui::ComponentBase
{
private:
	std::vector<std::string>* Entries;
	int* Selected;
	std::string::size_type MaxSlotWidth;

	ftxui::Box LeftBox;
	ftxui::Box RightBox;

public:
	WheelPicker(std::vector<std::string>* const entries, int* const selected)
		: Entries(entries),
		  Selected(selected)
	{
		std::vector<std::string>::iterator iterator = std::ranges::max_element(*Entries, [](const std::string& left, const std::string& right)
			{
				return left.size() < right.size();
			});

		MaxSlotWidth = iterator->size();
	}

	ftxui::Element OnRender() override;
	bool OnEvent(ftxui::Event) override;
	bool Focusable() const override;
};

ftxui::Component MakeWheelPicker(std::vector<std::string>* const entries, int* const selected);