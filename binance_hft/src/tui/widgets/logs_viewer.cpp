#include "tui/widgets/logs_viewer.hpp"

#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/mouse.hpp>
#include <algorithm>

ftxui::Element LogsViewer::OnRender()
{
    if (*LogsSelected != LastLogsSelected)
    {
        ScrollOffset = 0;
        HorizontalOffset = 0;
        LastLogsSelected = *LogsSelected;
    }

    const LoggerReader& selectedLoggerReader = *LogsSelected ? TechnicalLoggerReader : TradeLoggerReader;
    const std::deque<std::string> loggedLines = selectedLoggerReader.GetLines();

    std::size_t maxLineLength = 0;
    for (const auto& line : loggedLines)
    {
        maxLineLength = std::max(maxLineLength, line.size());
    }

    const int visibleWidth = LogsBox.x_max - LogsBox.x_min + 1;
    const int maxHorizontalOffset = std::max(0, static_cast<int>(maxLineLength) - visibleWidth);
    HorizontalOffset = std::clamp(HorizontalOffset, 0, maxHorizontalOffset);

    int currentHeight = GetScreenHeight();
    currentHeight = std::max(0, currentHeight - 8);
    const int maxOffset = std::max(0, static_cast<int>(loggedLines.size()) - currentHeight);

    ScrollOffset = std::clamp(ScrollOffset, 0, maxOffset);

    const int endIndex = loggedLines.size() - ScrollOffset;
    const int startIndex = std::max(0, endIndex - currentHeight);

    std::vector<ftxui::Element> elements;
    for (int i = startIndex; i < endIndex; i++)
    {
        const std::string& line = loggedLines[i];
        ftxui::Color textColor = ftxui::Color::Default;
        const auto firstOpen = line.find('[');
        const auto firstClose = line.find(']', firstOpen);
        const auto secondOpen = line.find('[', firstClose);
        const auto secondClose = line.find(']', secondOpen);

        if (firstOpen != std::string::npos &&
            firstClose != std::string::npos &&
            secondOpen != std::string::npos &&
            secondClose != std::string::npos
            )
        {
            const std::string level = line.substr(secondOpen + 1, secondClose - secondOpen - 1);

            if (level == "error")
            {
                textColor = ftxui::Color::Red;
            }
            else if (level == "warning")
            {
                textColor = ftxui::Color::Orange1;
            }
            else if (level == "info")
            {
                textColor = ftxui::Color::Green;
            }
        }

        const std::string visibleLine = HorizontalOffset < static_cast<int>(line.size()) ? line.substr(HorizontalOffset) : "";
        elements.emplace_back(ftxui::text(visibleLine) | ftxui::color(textColor));
    }

    return ftxui::vbox({
        ftxui::separator(),
        ftxui::vbox(elements)
        | ftxui::yframe
        | ftxui::vscroll_indicator
        | ftxui::reflect(LogsBox)
    });
}

bool LogsViewer::OnEvent(ftxui::Event event)
{
    // Scroll Up
    if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelUp
        && event.mouse().x >= LogsBox.x_min && event.mouse().x <= LogsBox.x_max
        && event.mouse().y >= LogsBox.y_min && event.mouse().y <= LogsBox.y_max)
    {
        ScrollOffset += 3;
        return true;
    }

    // Scroll Down
    if (event.is_mouse() && event.mouse().button == ftxui::Mouse::WheelDown
        && event.mouse().x >= LogsBox.x_min && event.mouse().x <= LogsBox.x_max
        && event.mouse().y >= LogsBox.y_min && event.mouse().y <= LogsBox.y_max)
    {
        ScrollOffset -= 3;
        return true;
    }

    // Keys
    const LoggerReader& selectedLoggerReader = *LogsSelected ? TechnicalLoggerReader : TradeLoggerReader;
    const auto loggedLines = selectedLoggerReader.GetLines();

    const int currentHeight = std::max(0, GetScreenHeight() - 8);
    const int maxOffset = std::max(0, static_cast<int>(loggedLines.size()) - currentHeight);

    // Home Key
    if (event == ftxui::Event::Home)
    {
        ScrollOffset = maxOffset;
        return true;
    }

    // End Key
    if (event == ftxui::Event::End)
    {
        ScrollOffset = 0;
        return true;
    }

    // PageUp Key
    if (event == ftxui::Event::PageUp)
    {
        if (currentHeight > 1)
        {
            ScrollOffset += currentHeight - 1;
        }
        return true;
    }

    // PageDown Key
    if (event == ftxui::Event::PageDown)
    {
        if (currentHeight > 1)
        {
            ScrollOffset -= currentHeight - 1;
        }
        return true;
    }

    // ArrowRight
    if (event == ftxui::Event::ArrowRight)
    {
        HorizontalOffset ++;
        return true;
    }

    // ArrowLeft
    if (event == ftxui::Event::ArrowLeft)
    {
        HorizontalOffset = std::max(0, HorizontalOffset - 1);
        return true;
    }

    // Left Mouse Button Click
    if (event.is_mouse() && event.mouse().button == ftxui::Mouse::Left
        && event.mouse().x >= LogsBox.x_min && event.mouse().x <= LogsBox.x_max
        && event.mouse().y >= LogsBox.y_min && event.mouse().y <= LogsBox.y_max)
    {
        TakeFocus();
        return true;
    }

    return false;
}

bool LogsViewer::Focusable() const
{
    return true;
}

ftxui::Component MakeLogsViewer(LoggerReader& tradeLoggerReader, LoggerReader& technicalLoggerReader, int* logsSelected,
                                std::function<int()> getScreenHeight)
{
    return ftxui::Make<LogsViewer>(tradeLoggerReader, technicalLoggerReader, logsSelected, getScreenHeight);
}