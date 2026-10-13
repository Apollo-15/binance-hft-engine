#include "logger/logger_manager.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

LoggerManager::LoggerManager(const std::string& tradeFilePath, bool truncateTradeFile, const std::string& technicalFilePath, bool truncateTechnicalFile)
{
    const auto tradeSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(tradeFilePath, truncateTradeFile);
    const auto technicalSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(technicalFilePath, truncateTechnicalFile);
    const std::string patternString = "[%Y-%m-%d %H:%M:%S] [%l] %v";

    TradeLogger = std::make_shared<spdlog::logger>("Trade Logger", tradeSink);
    TechnicalLogger = std::make_shared<spdlog::logger>("Technical Logger", technicalSink);

    TradeLogger->flush_on(spdlog::level::info);
    TechnicalLogger->flush_on(spdlog::level::info);
    TradeLogger->set_pattern(patternString);
    TechnicalLogger->set_pattern(patternString);
}