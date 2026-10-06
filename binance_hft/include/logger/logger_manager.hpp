#pragma once

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>

class LoggerManager
{
private:
	std::shared_ptr<spdlog::logger> TradeLogger;
	std::shared_ptr<spdlog::logger> TechnicalLogger;

public:
	explicit LoggerManager(const std::string& tradeFilePath, bool truncateTradeFile,
		const std::string& technicalFilePath, bool truncateTechnicalFile)
	{
		std::shared_ptr<spdlog::sinks::sink> consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		auto tradeSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(tradeFilePath, truncateTradeFile);
		auto technicalSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(technicalFilePath, truncateTechnicalFile);

		TradeLogger = std::make_shared<spdlog::logger>("Trade Logger", std::vector<spdlog::sink_ptr>{ consoleSink, tradeSink });
		TechnicalLogger = std::make_shared<spdlog::logger>("Technical Logger", std::vector<spdlog::sink_ptr>{ consoleSink, technicalSink });
	}


	[[nodiscard]] std::shared_ptr<spdlog::logger> GetTradeLogger() const { return TradeLogger; }
	[[nodiscard]] std::shared_ptr<spdlog::logger> GetTechnicalLogger() const { return TechnicalLogger; }
};


