#pragma once

#include "logger/reader/logger_reader.hpp"

#include <ftxui/component/component_base.hpp>
#include <functional>

class LogsViewer : public ftxui::ComponentBase
{
private:
    LoggerReader& TradeLoggerReader;
    LoggerReader& TechnicalLoggerReader;
    int* LogsSelected;
    int ScrollOffset {0};
    int HorizontalOffset {0};
    ftxui::Box LogsBox;
    int LastLogsSelected {0};
    std::function<int()> GetScreenHeight;

public:
    explicit LogsViewer(LoggerReader& tradeLoggerReader, LoggerReader& technicalLoggerReader, int* logsSelected,
        std::function<int()> getScreenHeight)
        : TradeLoggerReader(tradeLoggerReader),
          TechnicalLoggerReader(technicalLoggerReader),
          LogsSelected(logsSelected),
          GetScreenHeight(getScreenHeight)
    {
    }

    ftxui::Element OnRender() override;
    bool OnEvent(ftxui::Event) override;
    bool Focusable() const override;
};

ftxui::Component MakeLogsViewer(LoggerReader& tradeLoggerReader, LoggerReader& technicalLoggerReader, int* logsSelected,
    std::function<int()> getScreenHeight);